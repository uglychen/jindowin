#ifndef __DISPATCH__LOGIN_SVC__LOGIN_LOGIC___
#define __DISPATCH__LOGIN_SVC__LOGIN_LOGIC___

#include "core/common.h"
#include "net/http_data.h"

#define DEFAULT_CONFIG_PATH     "./plugins/usersvc/login.xml"

namespace login_svc_logic{

class Loginlogic{

public:
    Loginlogic();
    virtual ~Loginlogic();

private:
    class crelease {
      ~crelease() {
        if (instance_ != NULL) {
          delete instance_;
          instance_ = NULL;
        }
      }
    };

    static crelease release_;

    static Loginlogic    *instance_;

public:
    static Loginlogic *GetInstance();

public:
    bool OnLoginConnect (struct server *srv,const int socket);

    bool OnLoginMessage (struct server *srv, const int socket,const void *msg, const int len);

    bool OnLoginClose (struct server *srv,const int socket);

    bool OnBroadcastConnect(struct server *srv,const int socket,const void *data, const int len);

    bool OnBroadcastMessage (struct server *srv, const int socket, const void *msg, const int len);

    bool OnBroadcastClose (struct server *srv, const int socket);

    bool OnIniTimer (struct server *srv);

    bool OnTimeout (struct server *srv, char* id, int opcode, int time);


private:
    bool OnLogin(struct server *srv, \
                   const int socket, \
                   NetBase* netbase, \
             const void* msg = NULL, \
                   const int len = 0);

    bool OnRegister(struct server *srv, \
                   const int socket, \
                   NetBase* netbase, \
             const void* msg = NULL, \
                   const int len = 0);
    bool Init();
};


}

#endif

