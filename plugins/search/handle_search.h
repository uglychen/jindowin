// Copyright (c) 2015-2015 The restful Authors. All rights reserved.
// Created on: 2016/4/7 Author: jiaoyongqing

#ifndef _HOME_RUNNER_JINDOWIN_PLUGINS_SEARCH_HANDLE_SEARCH_H_
#define _HOME_RUNNER_JINDOWIN_PLUGINS_SEARCH_HANDLE_SEARCH_H_

#include <string>

#include "net/base.h"

class HandleSearch : public base::Base {
 public:
    HandleSearch(std::string message, \
                std::string jsonp_str, \
                            int socket)
      : message_(message), \
        jsonp_str_(jsonp_str), \
        socket_(socket) {}

    ~HandleSearch() {}

 public:
    bool GetData();
    void SendMessage();

 private:
    bool DataInMySql(time_t start_time, time_t end_time);
    bool DataInRedis(time_t start_time, time_t end_time) {}
    bool DataBetweenRedisAndMySql(time_t start_time, \
                                    time_t end_time, \
                                  time_t middle_time) {}

    bool MatchGn();
    bool MatchHy();
    bool MatchName();
    bool MatchCode();

 private:
    std::string message_;
    std::string jsonp_str_;
    int socket_;

    ContainerStr code_;
    size_t c_num_;

    ContainerStr name_;
    size_t n_num_;

    ContainerStr hy_;
    size_t h_num_;

    ContainerStr gn_;
    size_t g_num_;
};

#endif  //  _HOME_RUNNER_JINDOWIN_PLUGINS_SEARCH_HANDLE_SEARCH_H_
