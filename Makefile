MKDIR 	:= mkdir -p --
RMDIR   := rm -Rf --
CC      := clang
WARNINGS:= -Wall -Wextra -Wfloat-equal -Wundef
CFLAGS  := $(WARNINGS) -O3 -g3 -DDEBUG
BIN     := ./bin
INCLUDE := ./include
SRC     := ./src
SRCS    := $(wildcard $(SRC)/*.c)
OUT		:= $(BIN)/shove

.PHONY: clean

$(OUT): $(SRCS)
	$(MKDIR) $(BIN)
	$(CC) $(CFLAGS) -I$(INCLUDE) $^ -o $(OUT)

clean:
	$(RMDIR) $(BIN) $(OBJ)