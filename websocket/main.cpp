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

struct Session {
  Session() : _conn(nullptr), _response(0), _status(0), _open(false) {}

  virtual ~Session() {
  }

  mg_connection *_conn;
  std::string _send;
  std::string _recv;
  int _response;
  int _status;
  bool _open;
};

int nextHandle = 0;
std::map<int, Session *> sessions;
mg_mgr manager;

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

static void server_handler(mg_connection *conn, int event, void *session, void *eventData) {
  websocket_message *wm;
  mg_str d;

  switch (event) {
  case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    // New websocket connection. Tell everybody.
    broadcast(conn, mg_mk_str("++ joined"));
    break;

  case MG_EV_WEBSOCKET_FRAME:
    wm = (struct websocket_message *)eventData;
    // New websocket message. Tell everybody.
    d = {(char *)wm->data, wm->size};
    broadcast(conn, d);
    break;

  case MG_EV_HTTP_REQUEST:
    break;

  case MG_EV_CLOSE:
    // Disconnect. Tell everybody.
    if (is_websocket(conn)) {
      broadcast(conn, mg_mk_str("-- left"));
    }
    break;
  }
}

static void connect(Session *session, int status) {
  session->_status = status;
}

static void handshake(Session *session, http_message *message) {
  session->_response = message->resp_code;
  session->_open = (message->resp_code == 101);
}

static void poll(Session *session) {
  //char msg[500];
  //mg_send_websocket_frame(conn, WEBSOCKET_OP_TEXT, msg, sizeof(msg));
}

static void receive(Session *session, websocket_message *message) {
  session->_recv.clear();
  session->_recv.append((char *)message->data, message->size);
}

static void close(Session *session) {
  session->_open = false;
}

static void client_handler(mg_connection *conn, int event, void *session, void *eventData) {
  switch (event) {
  case MG_EV_CONNECT:
    connect((Session *)session, *((int *)eventData));
    break;

  case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    handshake((Session *)session, (http_message *)eventData);
    break;

  case MG_EV_POLL:
    poll((Session *)session);
    break;

  case MG_EV_WEBSOCKET_FRAME:
    receive((Session *)session, (websocket_message *)eventData);
    break;

  case MG_EV_CLOSE:
    close((Session *)session);
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
  int result;
  auto session = getSession(argc, params);
  if (session != nullptr) {
    result = 1;
  } else {
    result = 0;
  }
  v_setint(retval, result);
  return result;
  return 1;
}

//
// msg = ws.receive(conn)
//
int cmd_receive(int argc, slib_par_t *params, var_t *retval) {
  int result;
  auto session = getSession(argc, params);
  if (session != nullptr) {
    result = 1;
  } else {
    result = 0;
  }
  v_setint(retval, result);
  return result;
}

//
// while ws.open(conn)
//
int cmd_open(int argc, slib_par_t *params, var_t *retval) {
  int result;
  auto session = getSession(argc, params);
  if (session != nullptr) {
    result = 1;
  } else {
    result = 0;
  }
  v_setint(retval, result);
  return result;
}

//
// ws.send(conn, "hello")
//
int cmd_send(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  auto session = getSession(argc, params);
  if (session != nullptr) {
    const char *message = get_param_str(argc, params, 1, nullptr);
    if (message != nullptr && message[0] != '\0') {
      session->_send.clear();
      session->_send.append(message);
      result = 1;
    }
  }
  v_setint(retval, result);
  return result;
}

//
// conn = ws.create("ws://127.0.0.1:8000", "ws_chat")
//
int cmd_create(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  const char *url = get_param_str(argc, params, 0, nullptr);
  const char *protocol = get_param_str(argc, params, 1, nullptr);
  if (url != nullptr && protocol != nullptr) {
    Session *session = new Session();
    mg_connection *conn = mg_connect_ws(&manager, client_handler, session, url, protocol, nullptr);
    if (conn == nullptr) {
      delete session;
      v_setstr(retval, "Connection failed");
    } else {
      sessions[++nextHandle] = session;
      session->_conn = conn;
      v_setint(retval, nextHandle);
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
  return 0;
}

int sblib_init(void) {
  mg_mgr_init(&manager, nullptr);
  return 1;
}

void sblib_close(void) {
  mg_mgr_free(&manager);
}
