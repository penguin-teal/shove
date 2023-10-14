MKDIR 			:= mkdir -p --
RMDIR   		:= rm -Rf --
CC      		:= clang
WARNINGS		:= -Wall -Wextra -Wfloat-equal -Wundef
CFLAGS 		    := $(WARNINGS) -std=c99 -O0 -g3 -DDEBUG
BIN   		    := ./bin
INCLUDE		    := ./include
LIB				:= ./lib
SRC 		    := ./src
SRCS  		    := $(wildcard $(SRC)/*.c)
OUT				:= $(BIN)/shove

HASHEDBROWN		:= ./libs/hashedbrown
HASHEDBROWNOUT	:= $(HASHEDBROWN)/bin/libhashedbrown.a
HASHEDBROWNLIB	:= $(LIB)/libhashedbrown.a
HASHEDBROWNINCLUDE:= $(INCLUDE)/hashedbrown.h

INCLUDEFLAGS	:= -I./include -I$(HASHEDBROWN)/include

.PHONY: clean

$(OUT): $(SRCS) $(HASHEDBROWNLIB)
	$(MKDIR) $(BIN)
	$(CC) $(CFLAGS) $(INCLUDEFLAGS) -L$(LIB) -lhashedbrown -lLLVM-16 $^ -o $(OUT)

$(HASHEDBROWNLIB): $(HASHEDBROWNOUT)
	$(MKDIR) $(LIB)
	cp -u $(HASHEDBROWNOUT) $(HASHEDBROWNLIB)
	cp -u $(HASHEDBROWN)/include/hashedbrown.h $(HASHEDBROWNINCLUDE)

$(HASHEDBROWNOUT): $(HASHEDBROWN)
	make -C $(HASHEDBROWN)

$(HASHEDBROWN):
	git clone https://github.com/penguin-teal/hashedbrown $(HASHEDBROWN)

clean:
	$(RMDIR) $(BIN) $(LIB) $(HASHEDBROWN)