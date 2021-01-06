EXE       := a.out
TEST_EXE  := runtests
DEST      ?= /group/cs1050

SRCS      := $(shell find src -type f -name '*.cpp')
OBJS      := $(SRCS:%.cpp=build/%.o)

TEST_SRCS := $(shell find test -type f -name '*.cpp')
TEST_OBJS := $(TEST_SRCS:%.cpp=build/%.o)

ALL_OBJS  := $(OBJS) $(TEST_OBJS)

SCRIPTS   := $(wildcard scripts/mucs-*)

CFLAGS    := -std=c++11 -g -pedantic -Wall -Werror -Wno-noexcept-type
INCLUDES  := -Iinclude
DEFINES   :=

# Return all elements of $2 that are in $1
# Read as: $1 contains any of $2
define contains-any-of
$(strip $(foreach t,$2,$(if $(filter $t,$1),$t)))
endef

# Setup flags for testing
ifneq ($(call contains-any-of,$(MAKECMDGOALS),test $(TEST_EXE)),)
DEFINES   += -D_MUCS_TEST -D_MUCS_ROOT=test_root -D_COMPILE_SCRIPT=scripts/compile
endif

ifneq ($(COVERAGE),)
CFLAGS    += --coverage -O0
endif

ifneq ($(shell uname -s),Linux)
CFLAGS    += -Wa,-mbig-obj
DEFINES   += -D_XOPEN_SOURCE=700 -U__STRICT_ANSI__
endif

CXX       := g++ $(INCLUDES)
INSTALL   := install -g cs1050-ta


main: $(EXE)
.PHONY: main

test: $(TEST_EXE)
.PHONY: test

$(EXE): $(OBJS)
	@echo "build/src/*.o -> $@"
	@$(CXX) $(OBJS) -o $@

$(TEST_EXE): $(ALL_OBJS)
	@echo "build/**/*.o -> $@"
	@$(CXX) $(ALL_OBJS) -o $@ -lgcov

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

wipe: clean
	rm -rf test_root/submissions
.PHONY: wipe

build_dirs:
	@mkdir -p build/src/models build/test
.PHONY: build_dirs


-include $(OBJS:%.o=%.d)
