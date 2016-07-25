CC	= gcc
CXX	= g++

INC = -L/usr/local/include
LIB = -L/usr/local/lib/ -lrdkafka -lpthread
CFLAGS = -g -Wall -O2 -pipe -fno-ident -MMD -D_GNU_SOURCE -D_REENTRANT -Wno-deprecated
CXXFLAGS += $(CFLAGS)

OBJ = kafka_consume.o \
      kafka_helper.o

TARGET = kfk_consume

all	: $(TARGET)

$(TARGET) : $(OBJ)
	@echo
	@echo "========== Building $@ =========="
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LIB)

clean:
	@echo
	@echo "cleaning ..."
	rm -rf $(TARGET) *.o *.d *~ *.orig
