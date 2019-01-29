TARGET   = news
CC_FILES = $(shell ls *.cc)

BASE     = $(basename $(CC_FILES))

OBJS     = $(addsuffix .o, $(BASE))
LIBS    := -lpthread	
CFLAGS  := -std=c++11 -g -Wall	
CXXFLAGS	:= $(CFLAGS)
LIBNEWS     = libnew.a
$(TARGET):$(OBJS)
	g++	  -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient $(CXXFLAGS) -o $(TARGET)	$(OBJS) -lrt $(LIBS)
clean:
	-rm -f news

	-rm -f *.o
	-rm -f *.a