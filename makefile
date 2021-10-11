CC 	:= gcc
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := .

BIN := $(BIN_DIR)/chip8
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS := -Iinclude -MMD -MP
LDFLAGS  := -Llib
LFLAGS   := -lSDL2

all: $(BIN)

$(BIN) : $(OBJ) | $(BIN_DIR)
	$(CC) $(LFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

-include $(OBJ:.o=.d)
