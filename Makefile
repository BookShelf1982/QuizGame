CFLAGS := -MMD -Wall -O3 `pkg-config --cflags libcurl` `pkg-config --cflags ncurses`
LDLIBS := `pkg-config --libs libcurl` `pkg-config --libs ncurses`

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