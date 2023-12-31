MKDIR 			:= mkdir -p --
RMDIR   		:= rm -Rf --

EXTRA_CFLAGS	:=
WARNINGS		:= -Wall -Wextra -Wfloat-equal -Wundef
ERRORS			:= -Werror=implicit-int -Werror=implicit-function-declaration
CFLAGS 		    := $(WARNINGS) $(ERRORS) -std=c99 -O0 -g3 -DDEBUG $(EXTRA_CFLAGS)

BIN   		    := ./bin
INCLUDE		    := ./include
LIB				:= ./lib
SRC 		    := ./src
SRCS  		    := $(wildcard $(SRC)/*.c)
OUT				:= $(BIN)/shove
LLVMFLAGS		:= $(shell llvm-config --cflags --ldflags --system-libs --libs all)

HASHEDBROWN		:= ./libs/hashedbrown
HASHEDBROWNOUT	:= $(HASHEDBROWN)/bin/libhashedbrown.a
HASHEDBROWNLIB	:= $(LIB)/libhashedbrown.a
HASHEDBROWNINCLUDE:= $(INCLUDE)/hashedbrown.h

INCLUDEFLAGS	:= -I./include -I$(HASHEDBROWN)/include

.PHONY: clean

$(OUT): $(SRCS) $(HASHEDBROWNLIB)
	$(MKDIR) $(BIN)
	$(CC) $(CFLAGS) $(LLVMFLAGS) $(INCLUDEFLAGS) -L$(LIB) -lhashedbrown $^ -o $(OUT)

$(HASHEDBROWNLIB): $(HASHEDBROWNOUT)
	$(MKDIR) $(LIB)
	cp -u $(HASHEDBROWNOUT) $(HASHEDBROWNLIB)

$(HASHEDBROWNOUT): $(HASHEDBROWN)
	make -C $(HASHEDBROWN) debug CC=$(CC)

$(HASHEDBROWN):
	git clone https://github.com/penguin-teal/hashedbrown $(HASHEDBROWN)

clean:
	$(RMDIR) $(BIN) $(LIB) $(HASHEDBROWN)