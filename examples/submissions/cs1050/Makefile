HERE         := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
TARGET       := $(notdir $(HERE:%/=%))
TEST_TARGET  := test.$(TARGET)

MUCS_INCLUDE := /group/cs1050/include

SRCS         := $(wildcard *.c)
OBJS         := $(SRCS:%.c=%.o)

TEST_SRCS    := $(wildcard test_*.cpp)
TEST_OBJS    := $(TEST_SRCS:%.cpp=%.o)

CFLAGS       := -Wall -Werror
GCC          := gcc $(CFLAGS) -std=c11
GPP          := g++ $(CFLAGS) -std=c++11 -I$(MUCS_INCLUDE)

.PHONY: test runtest compiles clean


$(TARGET): $(OBJS)
	$(GCC) $^ -o $@

test: $(TEST_OBJS) $(filter-out main.o,$(OBJS))
	$(GPP) $^ -o $(TEST_TARGET)

runtest: test
	@echo
	./$(TEST_TARGET)

compiles: clean $(TARGET)
	@$(MAKE) clean

%.o: %.c
	$(GCC) -c $<

%.o: %.cpp
	$(GPP) -c $<

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJS) $(TEST_OBJS)
