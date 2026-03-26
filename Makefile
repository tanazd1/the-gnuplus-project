# Build GNU+ Compiler + Test Program
SRC_DIR := src
BUILD_DIR := build
TEST_DIR := tests

# Compiler executables
PREPROCESSOR := $(BUILD_DIR)/preprocessor
PARSERGEN := $(BUILD_DIR)/parser_codegen

all: $(BUILD_DIR) $(PREPROCESSOR) $(PARSERGEN) test

# Create build dir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build preprocessor
$(PREPROCESSOR): $(SRC_DIR)/preprocessor.c
	gcc $(SRC_DIR)/preprocessor.c -o $(PREPROCESSOR)

# Build parser+codegen
$(PARSERGEN): $(SRC_DIR)/parser_codegen.c
	gcc $(SRC_DIR)/parser_codegen.c -o $(PARSERGEN)

# Compile test programs
test: $(PREPROCESSOR) $(PARSERGEN)
	@echo "Preprocessing test program..."
	$(PREPROCESSOR) $(TEST_DIR)/hello.c+++ $(BUILD_DIR)/hello.i+++
	@echo "Parsing + generating C..."
	$(PARSERGEN) $(BUILD_DIR)/hello.i+++ $(BUILD_DIR)/hello.c
	@echo "Compiling final executable..."
	gcc $(BUILD_DIR)/hello.c -o $(BUILD_DIR)/hello
	chmod +x $(BUILD_DIR)/hello
	@echo "Running test..."
	$(BUILD_DIR)/hello

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all test clean
