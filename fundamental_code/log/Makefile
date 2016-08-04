CC	= gcc
CXX	= g++

INC = .
LIB = 
#CFLAGS = -g -Wall -O2 -pipe -fno-ident -MMD -D_GNU_SOURCE -D_REENTRANT -Wno-deprecated
CFLAGS = -g -Wall -pipe -fno-ident -MMD -D_GNU_SOURCE -D_REENTRANT -Wno-deprecated -llog4cpp -lpthread
CXXFLAGS += $(CFLAGS)

SIZE_ROLL_TARGET = log_size_rolling
DAILY_ROLL_TARGET = log_daily_rolling

SIZE_ROLL_OBJ = log_size_rolling.o
DAILY_ROLL_OBJ = log_daily_rolling.o

all : $(SIZE_ROLL_TARGET) $(DAILY_ROLL_TARGET) 

$(SIZE_ROLL_TARGET) : $(SIZE_ROLL_OBJ) 
	@echo
	@echo "========== Building $@ =========="
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB)

$(DAILY_ROLL_TARGET) : $(DAILY_ROLL_OBJ) 
	@echo
	@echo "========== Building $@ =========="
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB)

clean:
	@echo
	@echo "cleaning ..."
	rm -rf $(SIZE_ROLL_TARGET) $(DAILY_ROLL_TARGET) *.o *.d *~ *.orig *.log.* *.log
