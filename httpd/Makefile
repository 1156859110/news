TARGET   = news
CC_FILES = $(shell ls *.cc)

BASE     = $(basename $(CC_FILES))

OBJS     = $(addsuffix .o, $(BASE))

$(TARGET):$(OBJS)
	g++ -std = c++0x -pthread -o $(TARGET)$(OBJS)
	obj/%.o:%.cc
		g++ -std = c++0x -pthread  -o $@ $<
		clean:
			-rm -f news

			-rm -f *.o