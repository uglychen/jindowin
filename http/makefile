###############################################################################
VPATH = .
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -D__DEBUG_CORE__ -DFCGI_STD  #-DCHECK_HEADER-DTEST
RCPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD #-DTEST
## for release
##CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

LIB_DIR = ./../../library
BASE_DIR = ./../pub
FCGI_MODULE_PATH = ${BASE_DIR}/fcgimodule/

## 头文件
INCLUDE_PATHS = -I$(LIB_DIR)/public/ -I /usr/local/libmemcached-1.0.7/include\
		-I/usr/local/lib -I$(LIB_DIR)/third_party/hiredis -I$(BASE_DIR)/\
		-I$(LIB_DIR)/third_party/breakpad/src \
		-I${BASE_DIR}/logtrace/ \
		-I${LIB_DIR}/base/ \
		-I${FCGI_MODULE_PATH}/ 

## 库
LINK_PATHS =  -L/usr/local/libmemcached-1.0.7/lib/ -L/usr/lib64/mysql
LINK_LIBRARYS = -lrt -lpthread  -liconv -lfcgi -lmysqlclient -lexpat -lcurl\
					$(LIB_DIR)/lib/libbreakpad_client.a $(LIB_DIR)/lib/libhiredis.a $(LINK_CORE_PATH)

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) $< -o $@
RCOMPILE = $(COMPILER) $(RCPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)
LINKALL = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $?  $(LINK_LIBRARYS)

OUTPUT_FILE = /var/www/cgi-bin/jindowin
#############################################################################################################################
all: clean proc install delete-temp-file

NEWS_OBJ=get_news.o set_news.o news_detail.o news_share.o news_transmit.o
USER_OBJ=user_login.o user_register.o
SEARCH_OBJ=search.o 
SUBSCRIBE_OBJ=add_subscribe.o query_section.o query_industry.o query_stock.o delete_subscribe.o query_subscribe.o
PUBLIC_OBJ =  mig_log.o core_interface.o http_formate.o log_trace.o fcgimodule.o
proc: $(PUBLIC_OBJ) $(NEWS_OBJ) $(USER_OBJ) $(SEARCH_OBJ) $(SUBSCRIBE_OBJ)
######################news###############################################
NEWSNAME = news
NEWSPROC_PATH = /news
NEWS_OBJECT_PATH = ./obj$(NEWSPROC_PATH)
OTJERPROC_VERSION = /1
NEWS_PATH = $(NEWSPROC_PATH)$(OTJERPROC_VERSION)
VPATH += .$(NEWS_PATH)/get_news/ .$(NEWS_PATH)/set_news/ .${NEWS_PATH}/news_detail ./${NEWS_PATH}/news_share \
				 ./${NEWS_PATH}/news_transmit

$(NEWS_OBJ):%.o:%.cc
	$(COMPILE) 
	

######################user###############################################
USERNAME = user_
USERPROC_PATH = /user
USER_OBJECT_PATH = ./obj$(USERPROC_PATH)
OTJERPROC_VERSION = /1
USER_PATH = $(USERPROC_PATH)$(OTJERPROC_VERSION)
VPATH += .$(USER_PATH)/user_login/ .$(USER_PATH)/user_register/

$(USER_OBJ):%.o:%.cc
	$(COMPILE) 
	
######################search####################################################
SEARCHNAME = search_
SEARCHPROC_PATH = /search
SEARCH_OBJECT_PATH = ./obj$(SEARCHPROC_PATH)
OTJERPROC_VERSION = /1
SEARCH_PATH = $(SEARCHPROC_PATH)$(OTJERPROC_VERSION)
VPATH += .$(SEARCH_PATH)/search/

$(SEARCH_OBJ):%.o:%.cc
	$(COMPILE) 

######################subscribe####################################################
SUBSCRIBENAME = subscribe_
SUBSCRIBEPROC_PATH = /subscribe
SUBSCRIBE_OBJECT_PATH = ./obj$(SUBSCRIBEPROC_PATH)
OTJERPROC_VERSION = /1
SUBSCRIBE_PATH=$(SUBSCRIBEPROC_PATH)$(OTJERPROC_VERSION)
VPATH += .$(SUBSCRIBE_PATH)/add_subscribe/ .$(SUBSCRIBE_PATH)/delete_subscribe/ .$(SUBSCRIBE_PATH)/query_industry/ .$(SUBSCRIBE_PATH)/query_section/ .$(SUBSCRIBE_PATH)/query_stock/ .$(SUBSCRIBE_PATH)/query_subscribe

$(SUBSCRIBE_OBJ):%.o:%.cc
	$(COMPILE) 

############public###############################
VPATH += $(LIB_DIR)/public/log/ $(LIB_DIR)/public/comm/ $(LIB_DIR)/public/http/ ${BASE_DIR}/logtrace/ ${FCGI_MODULE_PATH}
$(PUBLIC_OBJ):%.o:%.cc
	$(COMPILE) 

install:
	@echo $(foreach obj, $(NEWS_OBJ), $(shell $(LINKALL) $(obj) $(PUBLIC_OBJ) -o $(patsubst %.o, %, $(OUTPUT_FILE)$(NEWS_PATH)/$(obj)).fcgi))
	@echo $(foreach obj, $(USER_OBJ), $(shell $(LINKALL) $(obj) $(PUBLIC_OBJ) -o $(patsubst %.o, %, $(OUTPUT_FILE)$(USER_PATH)/$(obj)).fcgi))
	@echo $(foreach obj, $(SEARCH_OBJ), $(shell $(LINKALL) $(obj) $(PUBLIC_OBJ) -o $(patsubst %.o, %, $(OUTPUT_FILE)$(SEARCH_PATH)/$(obj)).fcgi))
	@echo $(foreach obj, $(SUBSCRIBE_OBJ), $(shell $(LINKALL) $(obj) $(PUBLIC_OBJ) -o $(patsubst %.o, %, $(OUTPUT_FILE)$(SUBSCRIBE_PATH)/$(obj)).fcgi))

clean:
	find $(OUTPUT_FILE) -type f -name "*.fcgi" | xargs -i rm {}
	@-find ./ -type f -name "*.o" | xargs rm -f

delete-temp-file:
	@-find ./ -type f -name "*.o" | xargs rm -f

