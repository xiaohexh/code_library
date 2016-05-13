BOOST_DIR = /home/xiaohe/Documents/boost_1_60_0/boost
THRIFT_DIR = /usr/local/include/thrift
LIB_DIR = /usr/local/lib
CPP_OPTS   =-Wall -O2

GEN_SRC = Serv.cpp student_constants.cpp student_types.cpp

SERVER=CppServer
CLIENT=CppClient
  
all: ${SERVER} ${CLIENT}

${SERVER} : student_server.cpp
	g++ ${CPP_OPTS} -o ${SERVER} -I${THRIFT_DIR} -I${BOOST_DIR}  -I../gen-cpp -L${LIB_DIR}  student_server.cpp ${GEN_SRC} -lthrift
${CLIENT} : client.cpp
	g++ ${CPP_OPTS} -o ${CLIENT} -I${THRIFT_DIR} -I${BOOST_DIR}  -I../gen-cpp -L${LIB_DIR} -lthrift client.cpp ${GEN_SRC} -lthrift
clean:
	$(RM) -r ${SERVER} ${CLIENT} *~
