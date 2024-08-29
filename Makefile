C        = g++
FLAGS    = -Wall -g 
INCLUDES = -I src/include
LIBS   	 = -L src/lib -lmingw32 -lSDL2main -lSDL2
SRC      = main.cpp Game.cpp
OBJS	 = $(SRC:.c=.o)
TARGET   = main.exe

$(TARGET): $(OBJS)
	$(C) $(FLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LIBS) 

.c.o: 
	$(C) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm *.o *.exe 