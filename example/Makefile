CC := gcc
LDFLAGS := -L../build -Wl,-rpath,../build/
CFLAGS := -Wall -Wextra -I../include
LDLIBS := -lsthread

# Path to the source directory
SRC_DIR := .

# Path to the build directory
BUILD_DIR := .

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Target binaries (one for each C file)
TARGETS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.out, $(SRC_FILES))

all: $(TARGETS)

$(BUILD_DIR)/%.out: $(BUILD_DIR)/%.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGETS) $(OBJ_FILES)
