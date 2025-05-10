# makefile pthipth

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:%.c=%.o)

INC = $(wildcard $(SRC_DIR)/*.h)

DEMO_DIR = demo
DEMO_SRCS = $(wildcard $(DEMO_DIR)/demo_*.c)
DEMO_EXE = $(DEMO_SRCS:%.c=%)
DEMO_HEADER = $(DEMO_DIR)/pthipth.h

TEST_DIR = test
TEST_SRCS = $(wildcard $(TEST_DIR)/test_*.c)
TEST_EXE = $(TEST_SRCS:%.c=%)
TEST_HEADER = $(INC:$(SRC_DIR)/%.h=$(TEST_DIR)/%.h)

CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -Isrc -fPIC
LIB_STATIC = libpthipth.a
LIB_SHARED = libpthipth.so

AR = /usr/bin/ar
RM = /usr/bin/rm

DEBUG = 0

ifeq ($(DEBUG), 1)
EXTRA_CFLAGS += -g
endif

.PHONY: all lib static shared header test demo clean tags re

all: lib header demo test

lib: static shared

static: $(LIB_STATIC)

shared: $(LIB_SHARED)

header: $(TEST_HEADER) $(DEMO_HEADER)

test: lib $(TEST_DIR)/$(LIB_STATIC) $(TEST_HEADER) $(TEST_EXE)

demo: lib $(DEMO_DIR)/$(LIB_STATIC) $(DEMO_HEADER) $(DEMO_EXE)

clean:
	$(RM) -f $(OBJS) $(DEMO_EXE) $(TEST_EXE) $(LIB_STATIC) $(LIB_SHARED) \
		$(DEMO_DIR)/$(LIB_STATIC) $(TEST_DIR)/$(LIB_STATIC) $(TEST_HEADER) $(DEMO_HEADER) *~

tags:
	find ./src -name "*.[cChH]" | xargs ctags

re: clean all

libpthipth.a: $(OBJS)
	$(AR) rcs $@ $^

libpthipth.so: $(OBJS)
	$(CC) -shared ${EXTRA_CFLAGS} -o $@ $^

$(SRC_DIR)/%.o:$(SRC_DIR)/%.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

$(TEST_DIR)/%:$(TEST_DIR)/%.c $(OBJS)
	$(CC) $< -L./test -lpthipth -o $@

$(DEMO_DIR)/%:$(DEMO_DIR)/%.c $(OBJS)
	$(CC) $< -L./demo -lpthipth -o $@

$(TEST_DIR)/$(LIB_STATIC): $(LIB_STATIC)
	ln -sf ../$(LIB_STATIC) ./test

$(DEMO_DIR)/$(LIB_STATIC): $(LIB_STATIC)
	ln -sf ../$(LIB_STATIC) ./demo

$(TEST_DIR)/%.h: $(SRC_DIR)/%.h
	ln -snf ../$< ./test

$(DEMO_DIR)/pthipth.h: $(SRC_DIR)/pthipth.h
	ln -snf ../src/pthipth.h ./demo
