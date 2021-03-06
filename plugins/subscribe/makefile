# Please tweak it to suit your environment and project.  You
# may want to move it to your project's root directory.
#
# SYNOPSIS:
#
#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make clean  - removes all files generated by make.

# medium obj
OBJS = subscribe_init.o subscribe_logic.o db_comm.o redis_comm.o

# Where to find user code.

LIB_DIR = ../../../../library
USER_DIR = ../../pub
SO_DIR = ../../bin/plugins
BASE_DIR = ./../base
STOCKSVC = ../../plugins

THIRD_PATH = $(LIB_DIR)/public

ARITHMETIC=$(USER_DIR)/arithmetic

CUSTOM_THIRD_PATH = $(LIB_DIR)/third_party

LINK_PATHS = -L./ -L/usr/local/libmemcached-1.0.7/lib/  -L/usr/lib64/mysql

# Flags passed to the preprocessor.
CPPFLAGS += -isystem -shared -fPIC -D_DB_POOL_ -D_DEBUG -DUSR_LOG_MSG -DPROCESS_WORK

# Flags passed to the C++ compiler.
CXXFLAGS += -g  -Wextra -pthread -shared -fPIC

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
PLUGIN = ../../bin/plugins/subscribe/subscribe.so
                
INCLUDE_PATHS = -I/usr/local/libmemcached-1.0.7/include \
				-I/usr/local/libmemcached/include\
				-I/usr/local/freetds/include\
				-I$(LIB_DIR)\
				-I$(THIRD_PATH)\
				-I/usr/local/include\
				-I/usr/include/mysql\
				-I$(USER_DIR)\
				-I$(CUSTOM_THIRD_PATH)/hiredis\
				-I$(CUSTOM_THIRD_PATH)\
				-I$(LIB_DIR)/base\
				-I$(BASE_DIR)\
				-I$(LIB_DIR)/public\
				-I$(STOCKSVC)\
				-I$(LIB_DIR)/third_party/hiredis\
				-I$(LIB_DIR)/third_party\
				-I$(ARITHMETIC)
				
			


# House-keeping build targets.

ALL_HEADERS = $(USER_HEADERS)
all : $(PLUGIN)

clean :
	rm -f $(PLUGIN)  *.o

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
#GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
#gtest-all.o : $(GTEST_SRCS_)
#	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

#gtest_main.o : $(GTEST_SRCS_)
#	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

#gtest.a : gtest-all.o
#	$(AR) $(ARFLAGS) $@ $^

#gtest_main.a : gtest-all.o gtest_main.o
#	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

$(OBJS) : %.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_PATHS) $<

error_comm.o :  $(USER_DIR)/net/error_comm.cc $(USER_DIR)/net/error_comm.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/net/error_comm.cc $(INCLUDE_PATHS)

http_data.o :  $(USER_DIR)/net/http_data.cc $(USER_DIR)/net/http_data.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/net/http_data.cc $(INCLUDE_PATHS)

logic_basic_info.o :  $(USER_DIR)/logic/logic_basic_info.cc $(USER_DIR)/logic/logic_basic_info.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/logic/logic_basic_info.cc $(INCLUDE_PATHS)

logic_unit.o :  $(USER_DIR)/logic/logic_unit.cc $(USER_DIR)/logic/logic_unit.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/logic/logic_unit.cc $(INCLUDE_PATHS)
	
http_packet.o: $(THIRD_PATH)/protocol/http_packet.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(THIRD_PATH)/protocol/http_packet.cc $(INCLUDE_PATHS)
	
mig_log.o: $(THIRD_PATH)/log/mig_log.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(THIRD_PATH)/log/mig_log.cc $(INCLUDE_PATHS)
	
basictypes.o: $(THIRD_PATH)/basic/basictypes.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(THIRD_PATH)/basic/basictypes.cc $(INCLUDE_PATHS)
	
basic_util.o: $(THIRD_PATH)/basic/basic_util.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(THIRD_PATH)/basic/basic_util.cc $(INCLUDE_PATHS)
	
config.o: $(THIRD_PATH)/config/config.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(THIRD_PATH)/config/config.cc $(INCLUDE_PATHS)
	
md5sum.o:$(THIRD_PATH)/basic/md5sum.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(THIRD_PATH)/basic/md5sum.cc $(INCLUDE_PATHS)

radom_in.o:$(THIRD_PATH)/basic/radom_in.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(THIRD_PATH)/basic/radom_in.cc $(INCLUDE_PATHS)

tools.o : $(USER_DIR)/tools/tools.cc $(USER_DIR)/tools/tools.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/tools/tools.cc $(INCLUDE_PATHS)

tea.o:$(ARITHMETIC)/tea/tea.cc $(ARITHMETIC)/tea/tea.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(ARITHMETIC)/tea/tea.cc $(INCLUDE_PATHS)

redis_warrper.o:$(THIRD_PATH)/storage/redis_warrper.c
	gcc $(CPPFLAGS) $(CXXFLAGS) -std=c99 -pedantic -c $(INCLUDE_PATHS) -O3 -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb  $(THIRD_PATH)/storage/redis_warrper.c $(INCLUDE_PATHS)


$(PLUGIN) :$(OBJS) \
	http_packet.o mig_log.o basictypes.o http_data.o error_comm.o\
	logic_basic_info.o logic_unit.o basic_util.o config.o \
	md5sum.o radom_in.o redis_warrper.o tools.o tea.o\
	$(CUSTOM_THIRD_PATH)/dmg_fp/lib/dmg_fp.a $(LIB_DIR)/base/lib/mybase.a $(LIB_DIR)/third_party/hiredis/libhiredis.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LINK_PATHS) -lpthread -lexpat -lrt -lpthread -ldl -liconv -lmysqlclient -lcurl	\
	$^ -o $@
