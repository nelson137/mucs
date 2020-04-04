EXE    := build/libmucs.a

SRCS   := $(wildcard src/*.cpp)
OBJS   := $(SRCS:src/%.cpp=build/%.o)

CFLAGS := -std=c++11 -pedantic -Wall -Werror -Iinclude

ifneq ($(shell uname -s),Linux)
CFLAGS += -Wa,-mbig-obj -D_XOPEN_SOURCE=700 -U__STRICT_ANSI__
endif

define can-find-exe
$(shell which $1 >/dev/null 2>&1 && echo yes)
endef

CXX  := /usr/bin/g++
ifeq ($(call can-find-exe,gccfilter),yes)
CXX  := gccfilter -c -n -a $(CXX)
endif


main: $(EXE)
.PHONY: main

$(EXE): $(OBJS)
	@ar rv $@ $^

build/%.o: src/%.cpp | build_dir
	@echo "$< -> $@"
	@$(CXX) -c -MMD $(CFLAGS) $< -o $@

clean:
	rm -rf build
.PHONY: clean

build_dir:
	@mkdir -p build
.PHONY: build_dir


-include $(OBJS:%.o=%.d)
