#C
CC = g++
# CC = clang++
CFLAGS = -I
# CFLAGS += -Wall -Wextra -pedantic -Ofast -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector -march=native -ffast-math
# CFLAGS += -Werror -g -fsanitize=address,leak,undefined
CFLAGS += -Wall -Wextra -pedantic -O3 -march=native

.PHONY: clean

LIBS= -lm -lpng -ljpeg

BDIR= bin

#Dependencies
IDIR=src
_DEPS = block.cpp constants.cpp decode.cpp encode.cpp image.cpp main.cpp tf.cpp jpeg.cpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

#Object files
ODIR=obj
_OBJ = block.o constants.o decode.o encode.o image.o main.o tf.o jpeg.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(IDIR)/%.cpp $(DEPS)
		@mkdir -p $(ODIR)
		$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

base: $(OBJ) 
		@mkdir -p $(BDIR)
		$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

run: base
	./bin/base

clean:
		rm $(ODIR)/*.o
		rm *.png
