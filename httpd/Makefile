TARGET   = news
CC_FILES = $(shell ls *.cc)

BASE     = $(basename $(CC_FILES))

OBJS     = $(addsuffix .o, $(BASE))
LIBS    := -lpthread
CFLAGS  := -std=c++11 -g -Wall
CXXFLAGS	:= $(CFLAGS)
$(TARGET):$(OBJS)
	g++	$(CXXFLAGS) -o $(TARGET)$(OBJS)	$(LIBS)
	%.o:%.cc
	g++	$(CXXFLAGS) -o $@ $< $(LIBS)
	clean:
	-rm -f news

	-rm -f *.o