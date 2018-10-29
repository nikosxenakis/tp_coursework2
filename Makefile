#COMPILER OPTIONS
CFLAGS := -O3 -qopenmp -std=c99
CC := icc

#DIRECTORIES
BASE_DIR := .

SRC_DIR := $(BASE_DIR)/src
HEADER_DIR := $(BASE_DIR)/src
BUILD_DIR := $(BASE_DIR)/src
BIN_DIR := $(BASE_DIR)

BIN := $(BIN_DIR)/loops2

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_DIR)/%.h 
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ_FILES)
	@echo "\tLinking..."
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $@

all: $(BIN)
	@echo "$(BIN) ready."

clean:
	@echo " Cleaning..."
	rm -r $(BIN)