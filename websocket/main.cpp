// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"
#include <string.h>
#include <signal.h>
#include <unordered_map>
#include <vector>
#include <string>

extern "C" {
  #include "mongoose/mongoose.h"
}

#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

#define MAX_POLL_SLEEP 40

sig_atomic_t signalReceived = 0;
mg_mgr manager;
struct Session;
std::unordered_map<int, Session *> sessions;

enum ConnectionState {
  kInit = 0,
  kClient,
  kServer,
  kClosed
};

struct Session {
  Session() :
    _conn(nullptr),
    _state(kInit),
    _handle(-1) {
  }

  Session(mg_connection *conn) :
    _conn(conn),
    _state(kInit),
    _handle(conn->id) {
    sessions[_handle] = this;
  }

  virtual ~Session() {
    if (_handle != -1) {
      sessions.erase(_handle);
    }
  }

  void setConnection(mg_connection *conn) {
    _conn = conn;
    _handle = _conn->id;
    sessions[_handle] = this;
  }

  std::string _send;
  std::string _recv;
  std::vector<Session *> _conns;
  mg_connection *_conn;
  ConnectionState _state;
  int _handle;
};

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  signalReceived = sig_num;
}

static void send(mg_connection *conn, const char *msg, int len) {
  mg_ws_send(conn, msg, len, WEBSOCKET_OP_TEXT);
}

static void server_http_msg(mg_connection *conn, mg_http_message *message, Session *session) {
  if (mg_http_match_uri(message, "/")) {
    mg_ws_upgrade(conn, message, NULL);
    session->_conns.push_back(new Session(conn));
  } else {
    mg_http_reply(conn, 200, "", session->_send.c_str());
  }
}

static void server_send(Session *session, const char *buf, size_t len, int id) {
  if (id != -1) {
    Session *target = sessions[id];
    if (target != nullptr) {
      send(target->_conn, buf, len);
    }
  } else {
    for (auto it = session->_conns.begin(); it != session->_conns.end();) {
      send((*it)->_conn, buf, len);
      it++;
    }
  }
}

static void server_ws_msg(mg_connection *conn, mg_ws_message *message) {
  Session *session = sessions[conn->id];
  if (session != nullptr) {
    session->_recv.append((char *)message->data.ptr, message->data.len);
  }
  mg_iobuf_delete(&conn->recv, conn->recv.len);
}

static void server_ev_close(mg_connection *conn, Session *session) {
  for (auto it = session->_conns.begin(); it != session->_conns.end();) {
    if ((*it)->_conn == conn) {
      Session *next = *it;
      delete next;
      it = session->_conns.erase(it);
    } else {
      it++;
    }
  }
  if (session->_handle == (int)conn->id) {
    delete session;
  }
}

static void server_handler(struct mg_connection *conn, int event, void *eventData, void *session) {
  switch (event) {
  case MG_EV_HTTP_MSG:
    server_http_msg(conn, (mg_http_message *)eventData, (Session *)session);
    break;
  case MG_EV_WS_MSG:
    server_ws_msg(conn, (mg_ws_message *)eventData);
    break;
  case MG_EV_CLOSE:
    server_ev_close(conn, (Session *)session);
    break;
  default:
    break;
  }
}

static void server_receive(var_t *retval, Session *session) {
  for (auto it = session->_conns.begin(); it != session->_conns.end();) {
    Session *next = *it;
    if (!next->_recv.empty()) {
      map_init(retval);
      v_setint(map_add_var(retval, "id", 0), next->_handle);
      v_setstr(map_add_var(retval, "data", 0), next->_recv.c_str());
      next->_recv.clear();
      break;
    }
    it++;
  }
}

static void client_ws_open(mg_connection *conn, Session *session) {
  session->_state = kClient;
  if (!session->_send.empty()) {
    send(conn, session->_send.c_str(), session->_send.length());
    session->_send.clear();
  }
}

static void client_ws_msg(mg_connection *conn, mg_ws_message *message, Session *session) {
  if (message->data.len && session->_state == kClient) {
    if (!session->_recv.empty()) {
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

static void set_session(var_p_t var, Session *session, mg_connection *conn) {
  session->setConnection(conn);
  map_init_id(var, conn->id);
  v_setint(map_add_var(var, "port", 0), conn->peer.port);
  v_setint(map_add_var(var, "ip", 0), conn->peer.ip);
}

static Session *get_session(int argc, slib_par_t *params, var_t *retval) {
  Session *result;
  if (is_param_map(argc, params, 0)) {
    result = sessions[params[0].var_p->v.m.id];
  } else {
    result = nullptr;
  }
  if (result == nullptr) {
    v_setstr(retval, "Invalid connection");
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
  if (session != nullptr) {
    if (session->_state == kServer) {
      server_receive(retval, session);
    } else {
      v_setstr(retval, session->_recv.c_str());
    }
    session->_recv.clear();
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
// ws.send(conn, "hello", [clientId])
//
static int cmd_send(int argc, slib_par_t *params, var_t *retval) {
  auto session = get_session(argc, params, retval);
  if (session != nullptr) {
    const char *message = get_param_str(argc, params, 1, nullptr);
    if (message != nullptr && message[0] != '\0') {
      int id = get_param_int(argc, params, 2, -1);
      switch (session->_state) {
      case kInit:
        session->_send.append(message);
        break;
      case kClient:
        send(session->_conn, message, strlen(message));
        break;
      case kServer:
        server_send(session, message, strlen(message), id);
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
      set_session(retval, session, conn);
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
  const char *url = get_param_str(argc, params, 0, nullptr);
  if (url != nullptr) {
    auto session = new Session();
    mg_connection *conn = mg_http_listen(&manager, url, server_handler, session);
    if (conn == nullptr) {
      delete session;
      v_setstr(retval, "Listen failed");
    } else {
      session->_state = kServer;
      set_session(retval, session, conn);
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
  return signalReceived ? -2 : 0;
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
