// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"
#include <string.h>
#include <map>

#include "mongoose/mongoose.h"
#include "var.h"
#include "var_map.h"
#include "param.h"

struct Session {
  Session() : _conn(nullptr), _send(nullptr), _recv(nullptr), _open(false) {}

  virtual ~Session() {
    delete _send;
    delete _recv;
  }

  mg_connection *_conn;
  char *_send;
  char *_recv;
  bool _open;
};

int nextHandle = 1;
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

static void client_handler(mg_connection *conn, int event, void *sess, void *eventData) {
  int status;
  http_message *hm;
  websocket_message *wm;

  switch (event) {
  case MG_EV_CONNECT:
    status = *((int *)eventData);
    if (status != 0) {
      printf("-- Connection error: %d\n", status);
    }
    break;

  case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    hm = (struct http_message *)eventData;
    if (hm->resp_code == 101) {
      printf("-- Connected\n");
    } else {
      printf("-- Connection failed! HTTP code %d\n", hm->resp_code);
      // Connection will be closed after this.
    }
    break;

  case MG_EV_POLL: {
    //char msg[500];
    //mg_send_websocket_frame(conn, WEBSOCKET_OP_TEXT, msg, sizeof(msg));
    break;
  }

  case MG_EV_WEBSOCKET_FRAME:
    wm = (struct websocket_message *)eventData;
    printf("%.*s\n", (int) wm->size, wm->data);
    break;

  case MG_EV_CLOSE:
    break;
  }
}

// ws.close(conn)
int cmd_close(int argc, slib_par_t *params, var_t *retval) {
  fprintf(stderr, "close!\n");
  return 1;
}

// msg = ws.receive(conn)
int cmd_receive(int argc, slib_par_t *params, var_t *retval) {
  return 1;
}

// while ws.open(conn)
int cmd_open(int argc, slib_par_t *params, var_t *retval) {
  v_setint(retval, 1);
  return 1;
}

// ws.send("hello")
int cmd_send(int argc, slib_par_t *params, var_t *retval) {
  return 1;
}

int cmd_create(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  const char *url = get_param_str(argc, params, 0, nullptr);
  const char *protocol = get_param_str(argc, params, 1, nullptr);
  if (url != nullptr && protocol != nullptr) {
    Session *session = new Session();
    mg_connection *conn = mg_connect_ws(&manager, client_handler, session, url, protocol, nullptr);
    if (conn == nullptr) {
      delete session;
      v_setstr(retval, "Invalid address");
    } else {
      sessions[++nextHandle] = session;
      session->_conn = conn;
      result = 1;
    }
  } else {
    v_setstr(retval, "Invalid arguments");
  }
  return result;
}

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
  {"SEND", cmd_send},
  {"RECEIVE", cmd_receive},
  {"CLOSE", cmd_close}
};

int sblib_proc_count() {
  return 0;
}

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

int sblib_events(int wait_flag, int *w, int *h) {
  fprintf(stderr, "sblib_events\n");
  mg_mgr_poll(&manager, 100);
  return 0;
}

int sblib_init(void) {
  mg_mgr_init(&manager, nullptr);
  return 1;
}

void sblib_close(void) {
  mg_mgr_free(&manager);
}
