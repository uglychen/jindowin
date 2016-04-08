
###############################################################################
## 程序名
PROC_NAME = query_industry
## 输出文件名
OUTPUT_FILE = /var/www/cgi-bin/northsea/prsim/subscribe/1/$(PROC_NAME).fcgi
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
##CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD #-DTEST
## for release
CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I../../../../../../library/public/ -I /usr/local/libmemcached-1.0.7/include\
		-I/usr/local/lib -I../../../../../../library/third_party/hiredis\
	        -I../../../../../../library/third_party/breakpad/src -I/usr/include/mysql -I../../../../pub

## 库
LINK_PATHS =  -L/usr/local/libmemcached-1.0.7/lib/ -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil -lrt -lpthread  -liconv -lfcgi++ -lmysqlclient -lexpat -lcurl\
					../../../../../../library/lib/libbreakpad_client.a ../../../../../../library/lib/libhiredis.a $(LINK_CORE_PATH)

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file

output: $(PROC_NAME).o mig_log.o core_interface.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译
$(PROC_NAME).o:
	$(COMPILE) ./$(PROC_NAME).cc

mig_log.o:
	$(COMPILE) ../../../../../../library/public/log/mig_log.cc

core_interface.o:
	$(COMPILE) ../../../../../../library/public/comm/core_interface.cc
