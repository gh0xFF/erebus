CC = gcc-14

ifeq ($(shell $(CC) --version | grep -q "gcc-14" && echo 1),)
$(error This Makefile requires GCC. Detected $(shell $(CC) --version | head -n 1))
endif

STD_FLAGS = -std=c99 -pedantic

WARNING_FLAGS = -Wall -Wextra -Werror \
    -Wconversion -Wsign-conversion -Wfloat-conversion \
    -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Wformat-security \
    -Wnull-dereference -Wstack-protector -Wtrampolines \
    -Warray-bounds=2 -Wstringop-overflow=4 -Wshift-overflow=2 -Wstrict-overflow=5 \
    -Wduplicated-cond -Wduplicated-branches -Wlogical-op \
    -Wrestrict -Wshadow=compatible-local -Wcast-qual -Wcast-align=strict \
    -Wdouble-promotion -Wimplicit-fallthrough=5 -Wmissing-prototypes \
    -Wmissing-declarations -Wstrict-prototypes -Wbad-function-cast \
    -Wold-style-definition -Wredundant-decls -Wnested-externs \
    -Winline -Wvla -Wswitch-default -Wswitch-enum \
    -Wunused-macros -Wunsafe-loop-optimizations -Wundef \
    -fstack-protector-strong -fPIE -fstack-clash-protection \
	-fno-strict-aliasing \
	-fno-builtin-memcpy -fno-builtin-memset -fno-builtin-strcpy \
    -fcf-protection=check -fanalyzer \
    -O2 -D_GLIBCXX_ASSERTIONS \
	-fno-common \
	-fsanitize=leak \
    -fsanitize-address-use-after-scope
	-fsanitize=float-divide-by-zero, unreachable 

HARDENING_FLAGS = -D_FORTIFY_SOURCE=3 -fstack-protector-strong -fPIE -fPIC 
DEBUG_FLAGS = -g3 -O0 -DDEBUG
RELEASE_FLAGS = -O2 -g0 -flto -ffunction-sections -fdata-sections -W

CFLAGS = $(STD_FLAGS) $(WARNING_FLAGS) $(HARDENING_FLAGS)
LDFLAGS = -pie -W -l sqlite3

TARGET = erebus
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

.PHONY: clean test release

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(BUILD_DIR) $(TARGET)

release: CFLAGS += $(RELEASE_FLAGS)
release: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

test_unit:
	$(CC) $(CFLAGS) $(LDFLAGS) $$(find ./src ./tests -name '*.c' ! -name '*main*.c') -o unit_tests && ./unit_tests

test_integration:
	chmod +x integration/test_cli.sh
	./integration/test_cli.sh

TEST_TEMPLATE = "./tests/data/instance_template.txt"
TEST_MESSAGE = "./tests/data/instance_msg.txt"
OUTPUT_FILE = "./tests/data/encrypted_instance_msg.txt"

encode: $(TARGET)
	./$(TARGET) -t $(TEST_TEMPLATE) -m $(TEST_MESSAGE) -o 1 -e
	@echo "=== Encoded output ==="
	@cat $(OUTPUT_FILE)

decode: $(TARGET)
	./$(TARGET) -t $(TEST_TEMPLATE) -m $(OUTPUT_FILE) -o 1 -d
	@echo "=== Decoded output ==="
	@cat $(OUTPUT_FILE)

test_app: $(TARGET)
	@echo "=== Starting encoding ==="
	./$(TARGET) -t $(TEST_TEMPLATE) -m $(TEST_MESSAGE) -o 1 -e
	@echo "=== Encoded content ==="
	@cat $(OUTPUT_FILE)
	@echo -e "\n=== Starting decoding ==="
	./$(TARGET) -t $(TEST_TEMPLATE) -m $(OUTPUT_FILE) -o 1 -d
	@echo "=== Decoded content ==="
	@cat $(OUTPUT_FILE)

test_flow:


test_all: test_unit test_integration

lines: 
	find . -type f \( -name "*.c" -o -name "*.h" \) -exec cat {} + | wc -l

clean:
	rm -rf $(BUILD_DIR) $(TARGET) && rm -f unit_tests

-include $(DEPS)