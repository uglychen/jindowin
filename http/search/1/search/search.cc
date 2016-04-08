// Copyright (c) 2015-2015 The restful Authors. All rights reserved.
// Created on: 2016/4/7 Author: jiaoyongqing

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <sstream>

#if defined (FCGI_STD)
#include <fcgi_stdio.h>
#elif defined(FCGI_PLUS)
#include <fcgio.h>
#include <fcgi_config.h>
#endif

#include "config/config.h"
#include "comm/core_interface.h"
#include "log/mig_log.h"
#include "net/operator_code.h"
#include "logtrace/log_trace.h"
#include "fcgimodule/fcgimodule.h"
// 设置请求类型
#define API_TYPE            SEARCH
#define LOG_TYPE            log_trace::TRACE_API_LOG

int main(int agrc, char* argv[]) {
  fcgi_module::FcgiModule fcgi_client;
  fcgi_client.Init("/var/www/tmp/jindowincorefile", API_TYPE, LOG_TYPE);
  fcgi_client.Run();
  fcgi_client.Close();     
  return 0;
}
