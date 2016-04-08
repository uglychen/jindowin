/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/runner/northsea/restful/http/news/1/get_news/get_news.cc
#    Author       : tianyiheng
#    Email        : tianyiheng@kunyan-inc.com
#    Date         : 2016-03-16 18:47
#    Description  : 
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <sstream>

// #include "client/linux/handler/exception_handler.h"

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
#define API_TYPE            QUERY_SECTION 
#define LOG_TYPE            log_trace::TRACE_API_LOG

int main(int agrc, char* argv[]) {
  fcgi_module::FcgiModule fcgi_client;
  fcgi_client.Init("/var/www/tmp/jindowincorefile", API_TYPE, LOG_TYPE);
  fcgi_client.Run();
  fcgi_client.Close();     
  return 0;
}
