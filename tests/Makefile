TARGET_NAME = all_tests
EXTERNAL_DIR ?= $(realpath ./external)
SRC_DIR = ./src
ROOT_DIR = $(realpath ../)

EXTERNAL_INCLUDES = \
	$(GLOBAL_INCLUDES) \
	-I$(ROOT_DIR)/include

EXTERNAL_LIBRARIES = \
	-L$(ROOT_DIR)/lib -lc-terra-client \
	$(GLOBAL_LIBS)

LOCAL_INCLUDES = \
	-I$(SRC_DIR)

LOCAL_LIBRARIES =

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g $(EXTERNAL_INCLUDES) $(LOCAL_INCLUDES)

LDFLAGS = $(LOCAL_LIBRARIES) $(EXTERNAL_LIBRARIES)

SRC = $(wildcard $(SRC_DIR)/main.c $(SRC_DIR)/impl/*.c)

OBJ_DIR = build
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

TARGET_DIR = ./bin
TARGET = $(TARGET_DIR)/$(TARGET_NAME)
#
#
#

all: $(TARGET)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

$(TARGET): $(OBJ) | $(TARGET_DIR)
	$(CC) $(OBJ) $(INCLUDES) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(EXTERNAL_INCLUDES) $(LOCAL_INCLUDES) -c $< -o $@

terrafy:
	$(MAKE) -C $(SRC_DIR)/terrafy EXTERNAL_DIR=$(EXTERNAL_DIR)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(TARGET_DIR)

.PHONY: all clean
