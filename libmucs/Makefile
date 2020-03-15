EXE  := build/libmucs.a

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=build/%.o)

define can-find-exe
$(shell which $1 2>/dev/null | awk 'NF>1 {print "yes"; exit}')
endef

CXX  := /usr/bin/g++ -std=c++11 -pedantic -Wall -Werror -Iinclude
ifeq ($(call can-find-exe,gccfilter),yes)
CXX  := gccfilter -c -n -a $(CXX)
endif


main: $(EXE)
.PHONY: main

$(EXE): $(OBJS)
	@ar rv $@ $^

build/%.o: src/%.cpp | build_dir
	@echo "$< -> $@"
	@$(CXX) -c -MMD $< -o $@

clean:
	rm -rf build
.PHONY: clean

build_dir:
	@mkdir -p build
.PHONY: build_dir


-include $(OBJS:%.o=%.d)
