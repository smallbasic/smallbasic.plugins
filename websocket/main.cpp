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
mg_serve_http_opts s_http_server_opts;

enum ConnectionState {
  kInit = 0,
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

  std::string _recv;
  mg_connection *_conn;
  ConnectionState _state;
  int _handle;
};

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  signalReceived = sig_num;
}

static void session_close(Session *session) {
  if (session != nullptr) {
    delete session;
  }
}

static void server_frame(mg_connection *conn, websocket_message *message, Session *session) {
  if (session != nullptr) {
    session->_recv.clear();
    session->_recv.append((char *)message->data, message->size);
  }
  for (mg_connection *c = mg_next(conn->mgr, nullptr); c != nullptr; c = mg_next(conn->mgr, c)) {
    if (c != conn) {
      mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, message->data, message->size);
    }
  }
}

static void server_handler(mg_connection *conn, int event, void *eventData, void *session) {
  switch (event) {
  case MG_EV_WEBSOCKET_FRAME:
    server_frame(conn, (websocket_message *)eventData, (Session *)session);
    break;

  case MG_EV_HTTP_REQUEST:
    mg_serve_http(conn, (http_message *)eventData, s_http_server_opts);
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
  if (session != nullptr && session->_state == kConnect && message->resp_code == 101) {
    session->_state = kOpen;
  }
}

static void client_receive_frame(Session *session, websocket_message *message) {
  if (session != nullptr) {
    session->_recv.clear();
    session->_recv.append((char *)message->data, message->size);
  }
}

static void client_receive(Session *session, mg_connection *conn) {
  if (session != nullptr && conn->recv_mbuf.len && session->_state == kOpen) {
    session->_recv.clear();
    if ((unsigned char)conn->recv_mbuf.buf[0] > 128) {
      // spurious first char?
      session->_recv.append((char *)conn->recv_mbuf.buf + 1, conn->recv_mbuf.len - 1);
    } else {
      session->_recv.append((char *)conn->recv_mbuf.buf, conn->recv_mbuf.len);
    }
    mbuf_remove(&conn->recv_mbuf, conn->recv_mbuf.len);
  }
}

static void client_handler(mg_connection *conn, int event, void *eventData, void *session) {
  switch (event) {
  case MG_EV_CONNECT:
    client_connect((Session *)session, *((int *)eventData));
    break;

  case MG_EV_RECV:
    client_receive((Session *)session, conn);
    break;

  case MG_EV_CLOSE:
    session_close((Session *)session);
    break;

  case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    client_handshake((Session *)session, (http_message *)eventData);
    break;

  case MG_EV_WEBSOCKET_FRAME:
    client_receive_frame((Session *)session, (websocket_message *)eventData);
    break;

  case MG_EV_WEBSOCKET_CONTROL_FRAME:
    break;

  default:
    break;
  }
}

Session *get_session(int argc, slib_par_t *params) {
  return sessions[get_param_int(argc, params, 0, 0)];
}

//
// ws.close(conn)
//
int cmd_close(int argc, slib_par_t *params, var_t *retval) {
  auto session = get_session(argc, params);
  if (session != nullptr) {
    session->_conn->flags |= MG_F_CLOSE_IMMEDIATELY;
  }
  return argc == 1;
}

//
// msg = ws.receive(conn)
//
int cmd_receive(int argc, slib_par_t *params, var_t *retval) {
  auto session = get_session(argc, params);
  if (session != nullptr && !session->_recv.empty())  {
    v_setstr(retval, session->_recv.c_str());
    session->_recv.clear();
  } else {
    v_setstr(retval, "");
  }
  return argc == 1;
}

//
// while ws.open(conn)
//
int cmd_open(int argc, slib_par_t *params, var_t *retval) {
  auto session = get_session(argc, params);
  if (session != nullptr) {
    v_setint(retval, session->_state != kClose && signalReceived == 0);
  }
  return argc == 1;
}

//
// ws.send(conn, "hello")
//
int cmd_send(int argc, slib_par_t *params, var_t *retval) {
  int result = argc == 2;
  auto session = get_session(argc, params);
  if (session != nullptr) {
    const char *message = get_param_str(argc, params, 1, nullptr);
    if (message != nullptr && message[0] != '\0') {
      mg_send_websocket_frame(session->_conn, WEBSOCKET_OP_TEXT, message, strlen(message));
    } else {
      v_setstr(retval, "Send failed");
      result = 0;
    }
  }
  return result;
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
      session->_conn = conn;
      result = 1;
    }
  } else {
    v_setstr(retval, "Invalid url");
  }
  return result;
}

//
// conn = ws.listen(8080 [, enabled_http])
//
int cmd_listen(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  const char *port = get_param_str(argc, params, 0, nullptr);
  if (port != nullptr) {
    auto session = new Session();
    mg_connection *conn = mg_bind(&manager, port, server_handler, session);
    if (conn == nullptr) {
      delete session;
      v_setstr(retval, "Listen failed");
    } else {
      mg_set_protocol_http_websocket(conn);
      if (get_param_int(argc, params, 1, 0) == 1) {
        s_http_server_opts.document_root = ".";
        s_http_server_opts.enable_directory_listing = "yes";
      } else {
        s_http_server_opts.enable_directory_listing = "no";
      }
      v_setint(retval, session->_handle);
      session->_conn = conn;
      result = 1;
    }
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
  if (!signalReceived) {
    mg_mgr_poll(&manager, MAX_POLL_SLEEP);
  }
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
  for (unsigned i = 0; i < sessions.size(); i++) {
    delete sessions[i];
  }
}
