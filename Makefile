LIB_NAME = c-terra-client
EXTERNAL_DIR ?= $(realpath ./external)
SRC_DIR = $(realpath ./src)

EXTERNAL_INCLUDES = \
	$(GLOBAL_INCLUDES) \
	-I$(EXTERNAL_DIR)/curl/include \
	-I$(EXTERNAL_DIR)/jansson/include

EXTERNAL_LIBRARIES = \
	-L$(EXTERNAL_DIR)/jansson/lib -ljansson \
	-L$(EXTERNAL_DIR)/curl/lib -lcurl

LOCAL_INCLUDES = \
	-I${SRC_DIR} \

LOCAL_LIBRARIES = \

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g $(EXTERNAL_INCLUDES) $(LOCAL_INCLUDES)

LDFLAGS = $(LOCAL_LIBRARIES) $(EXTERNAL_LIBRARIES)

SRC = $(wildcard \
			$(SRC_DIR)/impl/*.c \
			$(SRC_DIR)/shared/impl/*.c\
			$(SRC_DIR)/requests/impl/*.c\
			)

OBJ_DIR = build
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

LIB_DIR = ./lib
SHARED_LIB = $(LIB_DIR)/lib$(LIB_NAME).so

#
#
#

all: $(SHARED_LIB)

$(SHARED_LIB): $(OBJ) | $(LIB_DIR)
	$(CC) -shared -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(EXTERNAL_INCLUDES) $(LOCAL_INCLUDES) -c $< -o $@

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	$(MAKE) -C ./tests clean

tests: all
	$(MAKE) -C ./tests \
		EXTERNAL_DIR=$(EXTERNAL_DIR) \
		GLOBAL_LIBS="$(LOCAL_LIBRARIES) $(EXTERNAL_LIBRARIES)" \
		GLOBAL_INCLUDES="$(LOCAL_INCLUDES) $(EXTERNAL_INCLUDES)"

run-tests: tests
	./tests/bin/all_tests

.PHONY: all clean tests run-tests
