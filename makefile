CC := g++
CFLAG := 

OBJECT := render_object.cpp
OBJ_learn := $(wildcard common/*.*) main.cpp

LFLAG := -lglut -lGL -lGLU -lGLEW -lglfw

default: 
	$(CC) $(CFLAG) -o run.o $(OBJ_learn) $(LFLAG)
