CC	= gcc
CXX	= g++

INC = .
LIB = -lzmq
CFLAGS = -g -Wall -O2 -pipe -fno-ident -MMD -D_GNU_SOURCE -D_REENTRANT -Wno-deprecated
CXXFLAGS += $(CFLAGS)

CLIENT = client
SERVER = server 

CLIENT_OBJ = client.o
SERVER_OBJ = server.o

all	: $(CLIENT) $(SERVER)

$(CLIENT) : $(CLIENT_OBJ)
	@echo
	@echo "========== Building $@ =========="
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)

$(SERVER) : $(SERVER_OBJ)
	@echo
	@echo "========== Building $@ =========="
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)

clean:
	@echo
	@echo "cleaning ..."
	rm -rf $(CLIENT) $(SERVER) *.o *.d *~ *.orig
