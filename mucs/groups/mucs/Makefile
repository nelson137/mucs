CFLAGS = -Wall -Werror
GCC = gcc $(CFLAGS) -std=c11
GPP = g++ $(CFLAGS) -std=c++11

HERE = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
NAME = $(notdir $(HERE:%/=%))
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TESTNAME = $(NAME).test
TEST_SRC = test.cpp
TEST_HEADER = catch.hpp
TEST_OBJS = $(patsubst %.cpp,%.o,$(TEST_SRC)) $(filter-out main.o,$(OBJS))

.PHONY: test runtest cleanobj clean

%.o: %.c
	$(GCC) -c $<

%.o: %.cpp
	$(GPP) -c $<

$(NAME): $(OBJS)
	$(GCC) $(OBJS) -o $(NAME)

test: $(TEST_OBJS) $(TEST_HEADER)
	$(GPP) $(TEST_OBJS) -o $(TESTNAME)

runtest: test
	./$(TESTNAME)

cleanobj:
	rm -f *.o

clean: cleanobj
	rm -f $(NAME) $(TESTNAME)
