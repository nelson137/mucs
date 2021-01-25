EXE       := a.out
TEST_EXE  := runtests
DEST      ?= /group/cs1050

SRCS      := $(shell find src -type f -name '*.cpp')
TEST_SRCS := $(shell find test -type f -name '*.cpp')
OBJS      := $(SRCS:%.cpp=build/%.o)
TEST_OBJS := $(TEST_SRCS:%.cpp=build/%.o)

SCRIPTS   := $(wildcard scripts/mucs-*)

CFLAGS    := -std=c++11 -pedantic -Wall -Werror -Wno-noexcept-type
INCLUDES  := -Iinclude

DEFINES   := -DONLY_C_LOCALE=1
TEST_MUCS_ROOT := test_root
LOCAL_COMPILE_SCRIPT := scripts/compile

# Setup flags for testing if the target recipes include `test`
ifneq ($(filter test,$(MAKECMDGOALS)),)
DEFINES   += -D_MUCS_TEST -D_MUCS_ROOT=$(TEST_MUCS_ROOT) -D_COMPILE_SCRIPT=$(LOCAL_COMPILE_SCRIPT)
endif

# Setup flags for debugging
# Usage: make DEBUG=1
ifneq ($(DEBUG),)
CFLAGS    += -g
endif

# Setup flags for a custom MUCS_ROOT
# Usage: make MUCS_ROOT=my_root
# A directory other than `test_root` should be used as it contains files that
# could break tests if they are changed.
ifneq ($(MUCS_ROOT),)
DEFINES   += -D_MUCS_ROOT=$(MUCS_ROOT) -D_COMPILE_SCRIPT=$(LOCAL_COMPILE_SCRIPT)
endif

# Setup flags for generating coverage data
# Usage: make test COVERAGE=1
# Note: should only be used with target `test`
ifneq ($(COVERAGE),)
CFLAGS    += --coverage -O0
endif

ifneq ($(shell uname -s),Linux)
CFLAGS    += -Wa,-mbig-obj
DEFINES   += -D_XOPEN_SOURCE=700 -U__STRICT_ANSI__
endif

CXX       := g++ $(INCLUDES)
INSTALL   := install -g cs1050-ta


main: $(OBJS)
	@echo "build/src/*.o -> $(EXE)"
	@$(CXX) $^ -o $(EXE)
.PHONY: main

test: $(OBJS) $(TEST_OBJS)
	@echo "build/**/*.o -> $(TEST_EXE)"
	@$(CXX) $^ -o $(TEST_EXE) -lgcov
.PHONY: test

build/%.o: %.cpp | build_dirs
	@echo "$< -> $@"
	@$(CXX) -c -MMD $(CFLAGS) $(DEFINES) $< -o $@

install: $(EXE)
	mkdir -p $(DEST)/{bin,config.d,submissions}
	chmod 775 $(DEST)/{bin,config.d}
	chmod 770 $(DEST)/submissions
	$(INSTALL) -m 755 $(SCRIPTS) $(DEST)/bin
	$(INSTALL) -m 4755 $(EXE) $(DEST)/bin
.PHONY: install

clean:
	rm -rf $(EXE) $(TEST_EXE) build
.PHONY: clean

build_dirs:
	@mkdir -p build/src/models build/test
.PHONY: build_dirs


-include $(OBJS:%.o=%.d)
