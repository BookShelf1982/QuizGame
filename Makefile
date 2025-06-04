MKDIR    = mkdir
RMDIR    = rm -rf
CC       = clang    
BIN      = build/bin
OBJ      = build/obj
#INCLUDE  = include
SRC      = src
CFLAGS   = -O3 -Wall -g `pkg-config --cflags libcurl` `pkg-config --cflags ncurses`
LDLIBS   = -lm `pkg-config --libs libcurl` `pkg-config --libs ncurses`
SRCS     = $(wildcard $(SRC)/*.c)
OBJS     = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
EXE      = $(BIN)/game

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS) | $(BIN)

# linker
$(BIN)/%: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# compiler  
$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN) $(OBJ):
	$(MKDIR) $@

clean:
	$(RMDIR) $(OBJ) $(BIN)