include Makefile.define
include Makefile.files
default:all

JSONLIBSRC = ../extlib/src/lib_json/json_value.o \
	  ../extlib/src/lib_json/json_writer.o \
	  ../extlib/src/lib_json/json_reader.o \
	
JSONLIB = ./libs/json_value.o \
	  ./libs/json_writer.o \
	  ./libs/json_reader.o \


BASE_SRC = ../LBase/LActive.o \
  	    ../LBase/LCharge.o \
  	    ../LBase/LIni.o \
  	    ../LBase/LLog.o \
  	    ../LBase/LMsg.o \
  	    ../LBase/LNet.o \
  	    ../LBase/LRunnable.o \
  	    ../LBase/LSocket.o \
  	    ../LBase/LTabFile.o \
  	    ../LBase/LTime.o \
  	    ../LBase/LTool.o \
  	    ../LBase/LUser.o \
  	    ../LBase/LVideo.o \
  	    ../LBase/LVipLog.o \
  	    ../LBase/Func.o \
		../LBase/BaseCommonFunc.o\
		../LBase/IPFilter.o\
		../LBase/TCPClient.o\
		../LBase/GarbageCollectionService.o\
		../LBase/RLogHttp.o\
		../LBase/RemoteLogThread.o\

BASE_LIBS = ./libs/LActive.o \
  	    ./libs/LCharge.o \
  	    ./libs/LIni.o \
  	    ./libs/LLog.o \
  	    ./libs/LMsg.o \
  	    ./libs/LNet.o \
  	    ./libs/LRunnable.o \
  	    ./libs/LSocket.o \
  	    ./libs/LTabFile.o \
  	    ./libs/LTime.o \
  	    ./libs/LTool.o \
  	    ./libs/LVideo.o \
  	    ./libs/LVipLog.o \
  	    ./libs/LUser.o \
  	    ./libs/Func.o \
		./libs/BaseCommonFunc.o\
		./libs/IPFilter.o\
		./libs/TCPClient.o\
		./libs/GarbageCollectionService.o\
		./libs/RLogHttp.o\
		./libs/RemoteLogThread.o\


LIB_BASE_OBJS =  $(JSONLIB) \
		 $(BASE_LIBS) \
		 $(MODULE_LIBS) \


PATS =  $(JSONLIBSRC) \
	$(BASE_SRC) \
	$(MODULE_SRC) \



TARGET_BIN = XLLogicServer
COPY=@cp -f 
REMOVE=@\rm -rf

all: $(TARGET_BIN) Makefile 


#$(CC) -shared -o $(LIB_BASE) $(LIB_BASE_OBJS) $(LIBS) -lpthread  -Wl,zdefs,-Bsymbolic 
$(TARGET_BIN):$(PATS)
	$(REMOVE) ./libs/*
	$(COPY) $(PATS) ./libs/
	$(CC) -o $(TARGET_BIN) $(LIB_BASE_OBJS) $(LIBS) -lpthread -fPIC -m64   -std=c++11 -lstdc++
	rm -rf *.o   $(LIB_BASE)  $(LIB_BASE_OBJS) $(PATH)   $(PATS)


clean:
	rm -rf *.o   $(LIB_BASE)  $(LIB_BASE_OBJS) $(PATH)   $(PATS)
	rm -rf $(TARGET_BIN)

core:
	rm -rf core.* $(TARGET_BIN)
