CFLAGS := -MMD -Wall -g
LDLIBS := -lncurses -lcurl

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
DEP = $(patsubst src/%.c, obj/%.d, $(SRC))
EXE = bin/game

.PHONY: all clean

all: obj bin $(EXE)

bin:
	mkdir -p bin

obj:
	mkdir -p obj

clean:
	$(RM) -rf obj bin

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

-include $(DEP)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<