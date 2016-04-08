#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include "net/operator_code.h"
//#include "client/linux/handler/exception_handler.h"

// TODO: 设置请求类型
#define GET_TYPE		USER_REGISTER
#define POST_TYPE		USER_REGISTER

#if defined (FCGI_STD)
#include <fcgi_stdio.h>
#elif defined(FCGI_PLUS)
#include <fcgio.h>
#include <fcgi_config.h>
#endif

#include "log/mig_log.h"
#include "config/config.h"
#include "comm/core_interface.h"

static bool DumpCallBack(const char* dump_path, \
                       const char* minidump_id, \
                                 void* contect, \
                                bool succeeded) {
  printf("Dump path:%s/%s.dump\n", dump_path, minidump_id);
  MIG_LOG(USER_LEVEL, "Dump path:%s/%s.dump\n", dump_path, minidump_id);
  return succeeded;
}

static void test_net_ipc(std::string& respone) {
  std::string content;
  int flag;
  int code;
  content = "type=regedit&username=flaght&";
  content += "password=123456&nickname=kerry&source=1\n";
  MIG_DEBUG(USER_LEVEL, "%s", content.c_str());
  net::core_get(0, \
  content.c_str(), \
  content.length(), \
          respone, \
             flag, \
            code);
}


#if defined (FCGI_STD)
static void GetRequestMethod(const char* query) {
  std::string content;
  std::string respone;
  std::stringstream os;
  int flag;
  int code;
  bool r = false;
  char* addr = getenv("REMOTE_ADDR");
  os << std::string(query) \
     << "&remote_addr=" \
     << addr \
     << "&type=" \
     << GET_TYPE \
     << "\n";
  content = os.str();
  /*content.assign(query);
  content.append("&addr=");
  content.append(addr);
  content.append("&type=" std::to_string(GET_TYPE) "\n");
  */
  MIG_INFO(USER_LEVEL, "%s", content.c_str());
  r = net::core_get(0, \
      content.c_str(), \
     content.length(), \
              respone, \
                 flag, \
                 code);
  MIG_INFO(USER_LEVEL, "%s", respone.c_str());
  if (!respone.empty())
    printf("Content-type: text/html\r\n"
    "\r\n"
    "%s", respone.c_str());
}

static void PostRequestMethod(std::string& content) {
  std::string respone;
  int flag;
  int code;
  std::stringstream os;
  bool r = false;
  //  若CONTENT_TYPE非application/x-www-form-urlencode 直接pass掉
  char* content_type = getenv("CONTENT_TYPE");
#if defined (CHECK_HEADER)
  if (strcmp(content_type, "application/x-www-form-urlencoded") != 0)
    return;
#endif
  char* addr = getenv("REMOTE_ADDR");
  os << content << "&remote_addr=" << addr << "&type1=" << POST_TYPE << "\n";
  content = os.str();
  MIG_INFO(USER_LEVEL, "%s", content.c_str());
  r = net::core_get(0, \
      content.c_str(), \
     content.length(), \
              respone, \
                 flag, \
                code);
  if (!respone.empty() && r)
    printf("Content-type: application/json;charset=utf-8\r\n"
      "\r\n"
      "%s", respone.c_str());
  }

static void PutRequestMethod(std::string& content) {
}

static void DeleteRequestMethod(std::string& content) {
}
#elif defined (FCGI_PLUS)

static void GetRequestMethod(FCGX_Request * request) {
}

static void PostRequestMethod(FCGX_Request * request) {
  char * clenstr = FCGX_GetParam("CONTENT_LENGTH", request->envp);
  if (clenstr) {
    int64 clen = strtol(clenstr, &clenstr, 10);
    char* content = new char[clen];
    std::cin.read(content, clen);
    clen = std::cin.gcount();
//  UserMgr::GetInstance()->PostUserInfo(content, clen);
    if (content) {
      delete []content;
      content = NULL;
    }
  }
  return;
}

static void PutRequestMethod(FCGX_Request * request) {
}

static void DeleteRequestMethod(FCGX_Request * request) {
}
#endif

void lock_file(int fd, int cmd, int type, off_t offset, int whence, off_t len) {
  struct flock lock;
  lock.l_whence = whence;
  lock.l_start = offset;
  lock.l_len = len;
  while (1) {
    lock.l_type = type;
    bool r = false;
    r = (cmd == F_SETLK);
    if (fcntl(fd, cmd, &lock) != -1) {
      if (lock.l_type == F_RDLCK) {
        MIG_DEBUG(USER_LEVEL, "read lock set by %d", getpid());
      } else if (lock.l_type == F_WRLCK) {
        MIG_DEBUG(USER_LEVEL, "write lock set by %d", getpid());
      } else {
        MIG_DEBUG(USER_LEVEL, "release lock set by %d", getpid());
      }
      break;
    } else {
      if (fcntl(fd, F_GETLK, &lock) == 0) {
        if (lock.l_type != F_UNLCK) {
          if (lock.l_type == F_RDLCK) {
            MIG_DEBUG(USER_LEVEL, "read lock already set by %d", lock.l_pid);
          } else {
            MIG_DEBUG(USER_LEVEL, "write lock already set by %d", lock.l_pid);
          }
        } else {
          MIG_DEBUG(USER_LEVEL, "no lock already set  %d", 0);
        }
      } else {
        MIG_DEBUG(USER_LEVEL, \
          "cannot get the description of struct flock  %d", 0);
      }
    }
  }
}

int main(int agrc, char* argv[]) {
    //  google_breakpad::ExceptionHandlereh\
    //   (".", NULL, DumpCallBack, NULL, true);
  std::string path = "./config.xml";
  std::string ipc_conn = "/var/www/tmp/jindowincorefile";
    // config::FileConfig file_config;
  std::string content;
  const char* query;
  bool r = false;
  MIG_INFO(USER_LEVEL, "11111init fastcgi id:%d", getpid());
  net::core_connect_ipc(ipc_conn.c_str());
  MIG_DEBUG(USER_LEVEL, "path[%s]", ipc_conn.c_str());
  //  test_net_ipc(content);
#if defined (FCGI_PLUS)
  FCGX_Request request;
  FCGX_Init();
  FCGX_InitRequest(&request, 0, 0);
#endif
#if defined (FCGI_STD)
  while (FCGI_Accept() == 0) {
#elif defined (FCGI_PLUS)
    while (FCGX_Accept_r(&request) == 0) {
#endif

#if defined (FCGI_PLUS)
  fcgi_streambuf cin_fcgi_streambuf(request.in);
  fcgi_streambuf cout_fcgi_streambuf(request.out);
  fcgi_streambuf cerr_fcgi_streambuf(request.err);

#if HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
  std::cin  = &cin_fcgi_streambuf;
  std::cout = &cout_fcgi_streambuf;
  std::cerr = &cerr_fcgi_streambuf;
#else
  std::cin.rdbuf(&cin_fcgi_streambuf);
  std::cout.rdbuf(&cout_fcgi_streambuf);
  std::cerr.rdbuf(&cerr_fcgi_streambuf);
#endif

#endif

#if defined (FCGI_STD)

#if defined (TEST)
  char* request_method = "POST";
#else
  char *request_method = getenv("REQUEST_METHOD");
#endif
  char *contentLength = getenv("CONTENT_LENGTH");

  if (strcmp(request_method, "POST") == 0 || \
       strcmp(request_method, "GET") == 0 || \
       strcmp(request_method, "PUT") == 0 || \
      strcmp(request_method, "DELETE") == 0) {
    int fd = open("/var/www/cgi-bin/northsea/prsim/mutx.txt", O_RDWR);
    if (fd < 0) {
      MIG_INFO(USER_LEVEL, "%s", "无法打开文件");
      exit(1);
    }
    lock_file(fd, F_SETLK, F_WRLCK, 0, 0, 0);
    if (strcmp(request_method, "POST") == 0) {
      int len = strtol(contentLength, NULL, 10);
      for (int i = 0; i < len; i++) {
        char ch;
        ch = getchar();
        content.append(1, ch);
      }
      PostRequestMethod(content);
      content.clear();
    } else if (strcmp(request_method, "GET") == 0) {
      query = getenv("QUERY_STRING");
      GetRequestMethod(query);
    } else if (strcmp(request_method, "PUT") == 0) {
      std::cin >> content;
      PutRequestMethod(content);
    } else if (strcmp(request_method, "DELETE") == 0) {
      std::cin >> content;
      DeleteRequestMethod(content);
    }
    lock_file(fd, F_SETLK, F_UNLCK, 0, 0, 0);
    close(fd);
  }

#elif defined (FCGI_PLUS)
      char *request_method = FCGX_GetParam("REQUEST_METHOD", request.envp);
      if (strcmp(request_method, "POST") == 0)
        PostRequestMethod(&request);
      else if (strcmp(request_method, "GET") == 0)
        GetRequestMethod(&request);
      else if (strcmp(request_method, "PUT") == 0)
        PutRequestMethod(&request);
      else if (strcmp(request_method, "DELETE") == 0)
        DeleteRequestMethod(&request);
#endif
    }

    net::core_close();
    MIG_INFO(USER_LEVEL, "deinit fastcgi id:%d", getpid());
    // UserMgr::FreeInstance();
    return 0;
}
