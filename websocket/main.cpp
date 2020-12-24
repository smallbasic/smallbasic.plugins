// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"
#include <string.h>
#include <unordered_map>
#include <vector>
#include <string>

extern "C" {
  #include "mongoose/mongoose.h"
}

#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

#define MAX_POLL_SLEEP 100

sig_atomic_t signalReceived = 0;
mg_mgr manager;
int nextHandle = 0;
struct Session;
std::unordered_map<int, Session *> sessions;

enum ConnectionState {
  kInit = 0,
  kClient,
  kServer,
  kClosed
};

struct Session {
  Session() : _conn(nullptr), _state(kInit) {
    _handle = ++nextHandle;
    sessions[_handle] = this;
  }

  virtual ~Session() {
    sessions.erase(_handle);
  }

  std::string _send;
  std::string _recv;
  std::vector<mg_connection *> _clients;
  mg_connection *_conn;
  ConnectionState _state;
  int _handle;
};

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  signalReceived = sig_num;
}

static void server_http_msg(mg_connection *conn, mg_http_message *message, Session *session) {
  if (mg_http_match_uri(message, "/")) {
    mg_ws_upgrade(conn, message);
    session->_clients.push_back(conn);
  } else {
    mg_http_reply(conn, 200, "", session->_send.c_str());
  }
}

static void server_send(Session *session, const char *buf, size_t len) {
  for (auto it = session->_clients.begin(); it != session->_clients.end();) {
    mg_ws_send(*it, buf, len, WEBSOCKET_OP_TEXT);
    it++;
  }
}

static void server_ws_msg(mg_connection *conn, mg_ws_message *message, Session *session) {
  session->_recv.append((char *)message->data.ptr, message->data.len);
  server_send(session, message->data.ptr, message->data.len);
  mg_iobuf_delete(&conn->recv, conn->recv.len);
}

static void server_ev_close(mg_connection *conn, Session *session) {
  for (auto it = session->_clients.begin(); it != session->_clients.end();) {
    if (*it == conn) {
      it = session->_clients.erase(it);
    } else {
      it++;
    }
  }
}

static void server_handler(struct mg_connection *conn, int event, void *eventData, void *session) {
  switch (event) {
  case MG_EV_HTTP_MSG:
    server_http_msg(conn, (mg_http_message *)eventData, (Session *)session);
    break;
  case MG_EV_WS_MSG:
    server_ws_msg(conn, (mg_ws_message *)eventData, (Session *)session);
    break;
  case MG_EV_ACCEPT:
    break;
  case MG_EV_CLOSE:
    server_ev_close(conn, (Session *)session);
    break;
  case MG_EV_POLL:
    break;
  default:
    break;
  }
}

static void client_ws_open(mg_connection *conn, Session *session) {
  session->_state = kClient;
  if (!session->_send.empty()) {
    mg_ws_send(conn, session->_send.c_str(), session->_send.length(), WEBSOCKET_OP_TEXT);
    session->_send.clear();
  }
}

static void client_ws_msg(mg_connection *conn, mg_ws_message *message, Session *session) {
  if (message->data.len && session->_state == kClient) {
    if (!session->_recv.empty()) {
      session->_recv.insert(0, "|");
      session->_recv.insert(0, (char *)message->data.ptr, message->data.len);
    } else {
      session->_recv.append((char *)message->data.ptr, message->data.len);
    }
  }
}

static void client_handler(mg_connection *conn, int event, void *eventData, void *fnData) {
  switch (event) {
  case MG_EV_ERROR:
    fprintf(stderr, "ERROR: [%p] %s", conn->fd, (char *)eventData);
    break;
  case MG_EV_WS_OPEN:
    client_ws_open(conn, (Session *)fnData);
    break;
  case MG_EV_WS_MSG:
    client_ws_msg(conn, (mg_ws_message *)eventData, (Session *)fnData);
    break;
  case MG_EV_CLOSE:
    ((Session *)fnData)->_state = kClosed;
    break;
  default:
    break;
  }
}

static Session *get_session(int argc, slib_par_t *params, var_t *retval) {
  Session *result;
  int handle = get_param_int(argc, params, 0, -1);
  if (handle != -1) {
    result = sessions[handle];
  } else {
    result = nullptr;
  }
  if (result == nullptr) {
    v_setstr(retval, "Invalid connection identifier");
  } else if (result->_state == kClosed) {
    v_setstr(retval, "Connection closed");
    delete result;
    result = nullptr;
  }
  return result;
}

//
// ws.close(conn)
//
static int cmd_close(int argc, slib_par_t *params, var_t *retval) {
  auto session = get_session(argc, params, retval);
  if (session != nullptr && session->_conn != nullptr) {
    session->_conn->is_closing = 1;
  }
  return session != nullptr;
}

//
// msg = ws.receive(conn)
//
static int cmd_receive(int argc, slib_par_t *params, var_t *retval) {
  auto session = get_session(argc, params, retval);
  if (session != nullptr && !session->_recv.empty())  {
    v_setstr(retval, session->_recv.c_str());
    session->_recv.clear();
  } else {
    v_setstr(retval, "");
  }
  return session != nullptr;
}

//
// while ws.open(conn)
//
static int cmd_open(int argc, slib_par_t *params, var_t *retval) {
  auto session = get_session(argc, params, retval);
  if (session != nullptr) {
    v_setint(retval, session->_state != kClosed && signalReceived == 0);
  }
  return session != nullptr;
}

//
// ws.send(conn, "hello")
//
static int cmd_send(int argc, slib_par_t *params, var_t *retval) {
  auto session = get_session(argc, params, retval);
  if (session != nullptr) {
    const char *message = get_param_str(argc, params, 1, nullptr);
    if (message != nullptr && message[0] != '\0') {
      switch (session->_state) {
      case kInit:
        session->_send.append(message);
        break;
      case kClient:
        mg_ws_send(session->_conn, message, strlen(message), WEBSOCKET_OP_TEXT);
        break;
      case kServer:
        server_send(session, message, strlen(message));
        break;
      case kClosed:
        v_setstr(retval, "Connection closed");
        session = nullptr;
        break;
      }
    } else {
      v_setstr(retval, "Send failed");
      session = nullptr;
    }
  }
  return session != nullptr;
}

//
// conn = ws.create("ws://127.0.0.1:8000")
//
static int cmd_create(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  const char *url = get_param_str(argc, params, 0, nullptr);
  if (url != nullptr) {
    auto session = new Session();
    mg_connection *conn = mg_ws_connect(&manager, url, client_handler, session, nullptr);
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
// conn = ws.listen("http://localhost:8000/websocket")
//
static int cmd_listen(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  const char *port = get_param_str(argc, params, 0, nullptr);
  if (port != nullptr) {
    auto session = new Session();
    mg_connection *conn = mg_http_listen(&manager, port, server_handler, session);
    if (conn == nullptr) {
      delete session;
      v_setstr(retval, "Listen failed");
    } else {
      v_setint(retval, session->_handle);
      session->_conn = conn;
      session->_state = kServer;
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

int sblib_init(const char *sourceFile) {
  signal(SIGTERM, signal_handler);
  signal(SIGINT, signal_handler);
  mg_mgr_init(&manager);
  return 1;
}

void sblib_close(void) {
  mg_mgr_free(&manager);
  for (unsigned i = 0; i < sessions.size(); i++) {
    delete sessions[i];
  }
}
