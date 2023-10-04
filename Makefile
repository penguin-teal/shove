MKDIR 	:= mkdir -p --
RMDIR   := rm -Rf --
CC      := clang
CFLAGS  := -Wall -g -DDEBUG
BIN     := ./bin
INCLUDE := ./include
SRC     := ./src
SRCS    := $(wildcard $(SRC)/*.c)
OUT		:= $(BIN)/shove

.PHONY: clean

$(OUT): $(SRCS)
	$(MKDIR) $(BIN)
	$(CC) -I$(INCLUDE) $^ -o $(OUT)

clean:
	$(RMDIR) $(BIN) $(OBJ)