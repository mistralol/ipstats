
include ../Makefile.Rules

EXE=../ipstats

SRCS= PCap.cpp Exception.cpp TimeUtil.cpp Config.cpp Counter.cpp Hosts.cpp Gui.cpp Version.cpp main.cpp

OBJS=$(SRCS:%.cpp=%.o)

INCLUDE+=-I../include/
LIBS+=-lpcap -lboost_thread -lboost_system -lncurses -lpthread

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


