HOM_PUB = ../../../public
HOM_CLIB = $(HOM_PUB)/CLIB
HOM_SNACC = $(HOM_PUB)/snacc-1.3
HOM_UTILITY = $(HOM_PUB)/utility
HOM_CLIENT = /data/bp/client
HOM_TFC = $(HOM_PUB)/tfc
HOM_BP_LIB = $(HOM_PUB)/bp_lib
HOM_LIBS = $(HOM_PUB)/libs

INC = -I$(HOM_CLIB)/include -I$(HOM_CLIB)/include/spp_incl -I$(HOM_SNACC)/c++-lib/inc/ -I$(HOM_PUB)/pb -I$(HOM_SNACC) -I$(HOM_UTILITY) -I$(HOM_CLIENT) -I$(HOM_TFC) -I$(HOM_BP_LIB)/inc
LNKDIR = -L$(HOM_CLIB)/lib -L$(HOM_CLIB)/lib/freetds -L$(HOM_SNACC)/c++-lib -L$(HOM_CLIENT) -L$(HOM_BP_LIB)/lib -L$(HOM_LIBS)
LNK = -lbp_lib -lccommon -lmessage -lrdkafka -lprotobuf -lhiredis
LIB = $(LNKDIR) $(LNK)

CC    = g++
CFLAG = -fPIC -Wall -Wno-deprecated

HPP =

vpath %.cpp %.cc

OBJ = kafka_consume.o \
	  kafka_helper.o

TAR = kfk_consume

all: $(TAR)

$(TAR): $(OBJ)
	@$(CC) $(LFLAG) -o $@ $(OBJ) $(INC) $(LIB)
	@echo $@

static: $(OBJ)
	$(CC) $(LFLAG) -g -static -o $(TAR) $(OBJ) $(OOB) $(INC) $(LIB)
	@echo $@

%.o: %.cpp $(HPP)
	$(CC) $(CFLAG) -g -c $< -o $@ $(INC)
	@echo $@

%.o: %.cc $(HPP)
	$(CC) $(CFLAG) -g -c $< -o $@ $(INC)
	@echo $@

clean: 
	rm -f *.o
	rm -f *.d
	rm -f *.so
	rm -rf kfk_consume
	rm -rf *~
