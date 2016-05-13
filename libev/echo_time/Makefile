CC	= gcc
CXX	= g++

INC = .
LIB = -lev
CFLAGS = -g -Wall -O2 -pipe -fno-ident -MMD -D_GNU_SOURCE -D_REENTRANT -Wno-deprecated
CXXFLAGS += $(CFLAGS)

TARGET	= server

OBJ	= server.o		\
	  my_log.o		\
	  my_signal.o

all	: $(TARGET)

$(TARGET) : $(OBJ)
	@echo
	@echo "========== Building $@ =========="
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB)

clean:
	@echo
	@echo "cleaning ..."
	rm -rf $(TARGET) *.o *.d *~ *.orig
