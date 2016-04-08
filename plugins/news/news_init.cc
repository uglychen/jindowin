#include "news/news_init.h"
#include <signal.h>
#include <stdlib.h>
#include "core/common.h"
#include "core/plugins.h"
#include "news/news_logic.h"
#include "net/comm_struct.h"
#include "base/logic/logic_comm.h"

static void *OnNewsStart() {
  signal(SIGPIPE, SIG_IGN);
  struct PluginInfo* news = (struct PluginInfo*) \
    calloc(1, sizeof(struct PluginInfo));
  news->id = "news";
  news->name = "news";
  news->version = "1.0.0";
  news->provider = "paco";
  if (!news_logic::NewsLogic::GetInstance())
    assert(0);
  return news;
}

static handler_t OnNewsShutdown(struct server* srv , void* pd) {
  news_logic::NewsLogic::FreeInstance();
  return HANDLER_GO_ON;
}

static handler_t OnNewsConnect(struct server *srv, int fd, \
    void *data, int len) {
  news_logic::NewsLogic::GetInstance()->OnNewsConnect(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnNewsMessage(struct server *srv, int fd, \
    void *data, int len) {
  bool r = news_logic::NewsLogic:: \
           GetInstance()->OnNewsMessage(srv , fd, data, len);
  if (r)
    return HANDLER_FINISHED;
  else
    return HANDLER_GO_ON;
}

static handler_t OnNewsClose(struct server *srv, int fd) {
  news_logic::NewsLogic::GetInstance()->OnNewsClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, \
    void *data, int len) {
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server* srv, int fd, \
    void *data, int len) {
  news_logic::NewsLogic::\
    GetInstance()->OnBroadcastConnect(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server* srv, int fd) {
  news_logic::NewsLogic::GetInstance()->OnBroadcastClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server* srv, int fd, \
    void *data, int len) {
  news_logic::NewsLogic::\
    GetInstance()->OnBroadcastMessage(srv, fd, data, len);
  return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server* srv) {
  news_logic::NewsLogic::GetInstance()->OnIniTimer(srv);
  return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server* srv, char* id, \
    int opcode, int time) {
  news_logic::NewsLogic::\
    GetInstance()->OnTimeout(srv, id, opcode, time);
  return HANDLER_GO_ON;
}

int news_plugin_init(struct plugin *pl) {
  pl->init = OnNewsStart;
  pl->clean_up = OnNewsShutdown;
  pl->connection = OnNewsConnect;
  pl->connection_close = OnNewsClose;
  pl->connection_close_srv = OnBroadcastClose;
  pl->connection_srv = OnBroadcastConnect;
  pl->handler_init_time = OnIniTimer;
  pl->handler_read = OnNewsMessage;
  pl->handler_read_srv = OnBroadcastMessage;
  pl->handler_read_other = OnUnknow;
  pl->time_msg = OnTimeOut;
  pl->data = NULL;
  return 0;
}
