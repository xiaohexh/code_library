CC = gcc
CXX = g++

BOOST_DIR = /home/xiaohe/Documents/boost_1_60_0/boost
THRIFT_DIR = /usr/local/include/thrift
LIB_DIR = /usr/local/lib
CPP_OPTS   =-Wall -O2

GEN_SRC = VOSServer.cpp \
		  vos_constants.cpp \
		  vos_types.cpp \
		  vos_log.cpp \
		  vos_conf.cpp

SERVER=VOSServer
CLIENT=VOSClient
  
all: ${SERVER} ${CLIENT}

${SERVER} : my_vosserver.cpp
	${CXX} ${CPP_OPTS} -o ${SERVER} -I${THRIFT_DIR} -I${BOOST_DIR}  -I../gen-cpp -L${LIB_DIR}  my_vosserver.cpp ${GEN_SRC} -lthrift
${CLIENT} : my_vosclient.cpp
	${CXX} ${CPP_OPTS} -o ${CLIENT} -I${THRIFT_DIR} -I${BOOST_DIR}  -I../gen-cpp -L${LIB_DIR} -lthrift my_vosclient.cpp ${GEN_SRC} -lthrift
clean:
	$(RM) -r ${SERVER} ${CLIENT} *~
