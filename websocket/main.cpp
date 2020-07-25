// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"
#include <string.h>
#include <map>
#include <string>

#include "mongoose/mongoose.h"
#include "var.h"
#include "var_map.h"
#include "param.h"

#define MAX_POLL_SLEEP 100

sig_atomic_t signalReceived = 0;
mg_mgr manager;
int nextHandle = 0;
struct Session;
std::map<int, Session *> sessions;

enum ConnectionState {
  kInit,
  kConnect,
  kOpen,
  kClose
};

struct Session {
  Session() : _state(kInit) {
    _handle = ++nextHandle;
    sessions[_handle] = this;
  }

  virtual ~Session() {
    sessions.erase(_handle);
  }

  std::string _send;
  std::string _recv;
  ConnectionState _state;
  int _handle;
};

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  signalReceived = sig_num;
}

static int is_websocket(const mg_connection *conn) {
  return conn->flags & MG_F_IS_WEBSOCKET;
}

static void broadcast(struct mg_connection *conn, const mg_str msg) {
  struct mg_connection *c;
  char buf[500];
  char addr[32];

  mg_sock_addr_to_str(&conn->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
  snprintf(buf, sizeof(buf), "%s %.*s", addr, (int) msg.len, msg.p);

  for (c = mg_next(conn->mgr, nullptr); c != nullptr; c = mg_next(conn->mgr, c)) {
    if (c != conn) {
      // don't send to the sender
      mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
  }
}

static void server_handshake(mg_connection *conn) {
  broadcast(conn, mg_mk_str("++ joined"));
}

static void server_frame(mg_connection *conn, websocket_message *message) {
  // New websocket message. Tell everybody.
  mg_str d = {(char *)message->data, message->size};
  broadcast(conn, d);
}

static void server_close(mg_connection *conn) {
  // Disconnect. Tell everybody.
  if (is_websocket(conn)) {
    broadcast(conn, mg_mk_str("-- left"));
  }
}

static void server_handler(mg_connection *conn, int event, void *eventData, void *session) {
  switch (event) {
  case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    server_handshake(conn);
    break;

  case MG_EV_WEBSOCKET_FRAME:
    server_frame(conn, (websocket_message *)eventData);
    break;

  case MG_EV_HTTP_REQUEST:
    break;

  case MG_EV_CLOSE:
    server_close(conn);
    break;

  default:
    break;
  }
}

static void client_connect(Session *session, int status) {
  if (session != nullptr && session->_state == kInit) {
    session->_state = kConnect;
  }
}

static void client_handshake(Session *session, http_message *message) {
  if (session->_state == kConnect && message->resp_code == 101) {
    session->_state = kOpen;
  }
}

static void client_poll(Session *session, mg_connection *conn) {
  if (!session->_send.empty()) {
    mg_send_websocket_frame(conn, WEBSOCKET_OP_TEXT, session->_send.c_str(), session->_send.length());
    session->_send.clear();
  }
}

static void client_receive(Session *session, websocket_message *message) {
  session->_recv.clear();
  session->_recv.append((char *)message->data, message->size);
}

static void client_close(Session *session) {
  if (session != nullptr) {
    delete session;
  }
}

static void client_handler(mg_connection *conn, int event, void *eventData, void *session) {
  switch (event) {
  case MG_EV_CONNECT:
    client_connect((Session *)session, *((int *)eventData));
    break;

  case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    client_handshake((Session *)session, (http_message *)eventData);
    break;

  case MG_EV_POLL:
    client_poll((Session *)session, conn);
    break;

  case MG_EV_WEBSOCKET_FRAME:
    client_receive((Session *)session, (websocket_message *)eventData);
    break;

  case MG_EV_CLOSE:
    client_close((Session *)session);
    break;

  default:
    break;
  }
}

Session *getSession(int argc, slib_par_t *params) {
  return sessions[get_param_int(argc, params, 0, 0)];
}

//
// ws.close(conn)
//
int cmd_close(int argc, slib_par_t *params, var_t *retval) {
  auto session = getSession(argc, params);
  if (session != nullptr) {
    // close socket
  }
  return argc == 1;
}

//
// msg = ws.receive(conn)
//
int cmd_receive(int argc, slib_par_t *params, var_t *retval) {
  auto session = getSession(argc, params);
  if (session != nullptr) {
    v_setstr(retval, session->_recv.c_str());
  }
  return argc == 1;
}

//
// while ws.open(conn)
//
int cmd_open(int argc, slib_par_t *params, var_t *retval) {
  auto session = getSession(argc, params);
  if (session != nullptr) {
    v_setint(retval, session->_state != kClose && signalReceived == 0);
  }
  return argc == 1;
}

//
// ws.send(conn, "hello")
//
int cmd_send(int argc, slib_par_t *params, var_t *retval) {
  auto session = getSession(argc, params);
  if (session != nullptr) {
    const char *message = get_param_str(argc, params, 1, nullptr);
    if (message != nullptr && message[0] != '\0') {
      session->_send.clear();
      session->_send.append(message);
    }
  }
  return argc == 2;
}

//
// conn = ws.create("ws://127.0.0.1:8000", "ws_chat")
//
int cmd_create(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  const char *url = get_param_str(argc, params, 0, nullptr);
  const char *protocol = get_param_str(argc, params, 1, nullptr);
  if (url != nullptr) {
    auto session = new Session();
    mg_connection *conn = mg_connect_ws(&manager, client_handler, session, url, protocol, nullptr);
    if (conn == nullptr) {
      delete session;
      v_setstr(retval, "Connection failed");
    } else {
      v_setint(retval, session->_handle);
      result = 1;
    }
  } else {
    v_setstr(retval, "Invalid url");
  }
  return result;
}

//
// conn = ws.listen(8080)
//
int cmd_listen(int argc, slib_par_t *params, var_t *retval) {
  int result;
  const char *port = get_param_str(argc, params, 0, nullptr);
  if (port != nullptr) {
    mg_connection *conn = mg_bind(&manager, port, server_handler, nullptr);
    mg_set_protocol_http_websocket(conn);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

API lib_func[] = {
  {"CREATE", cmd_create},
  {"OPEN", cmd_open},
  {"LISTEN", cmd_listen},
  {"RECEIVE", cmd_receive},
};

int sblib_func_count() {
  return (sizeof(lib_func) / sizeof(lib_func[0]));
}

int sblib_func_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_func_count()) {
    strcpy(proc_name, lib_func[index].name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_func_count()) {
    result = lib_func[index].command(argc, params, retval);
  } else {
    result = 0;
  }
  return result;
}

API lib_proc[] = {
  {"SEND", cmd_send},
  {"CLOSE", cmd_close}
};

int sblib_proc_count() {
  return (sizeof(lib_proc) / sizeof(lib_proc[0]));
}

int sblib_proc_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_proc_count()) {
    strcpy(proc_name, lib_proc[index].name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int sblib_proc_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_proc_count()) {
    result = lib_proc[index].command(argc, params, retval);
  } else {
    result = 0;
  }
  return result;
}

int sblib_events(int wait_flag, int *w, int *h) {
  mg_mgr_poll(&manager, MAX_POLL_SLEEP);
  return signalReceived ? 2 : 0;
}

int sblib_init(void) {
  signal(SIGTERM, signal_handler);
  signal(SIGINT, signal_handler);
  mg_mgr_init(&manager, nullptr);
  return 1;
}

void sblib_close(void) {
  mg_mgr_free(&manager);
}
