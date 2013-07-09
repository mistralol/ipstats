

EXE=../ipstats

SRCS= PCap.cpp Exception.cpp TimeUtil.cpp Config.cpp Counter.cpp Hosts.cpp Gui.cpp Version.cpp main.cpp

OBJS=$(SRCS:%.cpp=%.o)
MKDEP=gcc -E -MM
OPTS=-Wall -Werror -D_GNU_SOURCE -std=gnu++0x -ggdb
CPP=g++
INCLUDE=-I../include/
LIBS=-lpcap -lboost_thread -lncurses

all: .depend $(EXE)

%.o: %.cpp Makefile
	$(CPP) $(OPTS) $(INCLUDE) -o $@ -c $<

$(EXE): $(OBJS)
	$(CPP) $(OBJS) $(LIBS) -o $(EXE)

dep: .depend

.depend: $(SRCS)
	$(MKDEP) $(INCLUDE) $(SRCS) > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif

clean:
	$(RM) $(EXE) $(OBJS) .depend


