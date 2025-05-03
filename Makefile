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

.PHONY: all lib static shared clean tags test demo

all: lib demo test

lib: static shared

static: $(LIB_STATIC) header
	ln -sf ../libpthipth.a ./test/
	ln -sf ../libpthipth.a ./demo/

header: $(TEST_HEADER) $(DEMO_HEADER)

$(TEST_DIR)/%.h: $(SRC_DIR)/%.h
	ln -snf ../$< ./test

$(DEMO_DIR)/pthipth.h: $(SRC_DIR)/pthipth.h
	ln -snf ../src/pthipth.h ./demo

shared: $(LIB_SHARED)

libpthipth.a: $(OBJS)
	$(AR) rcs $@ $^

libpthipth.so: $(OBJS)
	$(CC) -shared ${EXTRA_CFLAGS} -o $@ $^

$(SRC_DIR)/%.o:$(SRC_DIR)/%.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

clean:
	$(RM) -f $(OBJS) $(DEMO_EXE) $(TEST_EXE) $(LIB_STATIC) $(LIB_SHARED) \
		$(DEMO_DIR)/$(LIB_STATIC) $(TEST_DIR)/$(LIB_STATIC) *~

tags:
	find ./src -name "*.[cChH]" | xargs ctags

re: clean all

test: lib $(TEST_EXE)

$(TEST_DIR)/%:$(TEST_DIR)/%.c
	$(CC) $< -L./demo -lpthipth -o $@

demo: lib $(DEMO_EXE)

$(DEMO_DIR)/%:$(DEMO_DIR)/%.c
	$(CC) $< -L./demo -lpthipth -o $@
