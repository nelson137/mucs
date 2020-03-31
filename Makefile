EXE       := a.out
TEST_EXE  := runtests
DEST      ?= /group/cs1050

SRCS      := $(wildcard src/*.cpp)
OBJS      := $(SRCS:%.cpp=build/%.o)

TEST_SRCS := $(wildcard test/*.cpp)
TEST_OBJS := $(TEST_SRCS:%.cpp=build/%.o)

ALL_OBJS  := $(OBJS) $(TEST_OBJS)

SCRIPTS   := $(wildcard bin/mucs-*)

CFLAGS    := -std=c++11 -g -pedantic -Wall -Werror -Wno-noexcept-type
LDFLAGS   := -Llibmucs/build
LDLIBS    := -lmucs
COVFLAGS  :=
DEFINES   :=

define can-find-exe
$(shell which $1 2>/dev/null | awk 'NF>1 {print "yes"; exit}')
endef

# Return a non-empty string if any element of $2 is found in $1
# Read as: $1 contains any of $2
define contains-any-of
$(strip $(foreach t,$2,$(if $(filter $t,$1),$t)))
endef

# Setup flags for local mucs root
define local-mucs-root
ifeq ($(findstring _MUCS_ROOT_LOCAL,$(DEFINES)),)
DEFINES += -D_MUCS_ROOT_LOCAL
endif
endef

CXX       := g++ -Iinclude -Ilibmucs/include
ifeq ($(call can-find-exe,gccfilter),yes)
CXX       := gccfilter -c -n -a $(CXX)
endif

INSTALL   := install -g cs1050-ta

ifeq ($(call can-find-exe,lcov),yes)
LCOV      := lcov -c --no-external
endif

ifeq ($(call can-find-exe,genhtml),yes)
GENHTML   := genhtml --legend --function-coverage --demangle-cpp
endif

ifeq ($(MUCS_ROOT),local)
$(eval $(call local-mucs-root))
endif

# Setup flags for testing
ifneq ($(call contains-any-of,$(MAKECMDGOALS),test $(TEST_EXE) coverage),)
DEFINES   += -DMUCS_TEST
$(eval $(call local-mucs-root))
ifneq ($(LCOV)$(GENHTML),)
COVFLAGS  := --coverage -O0
endif
else
endif


main: $(EXE)
.PHONY: main

test: $(TEST_EXE)
.PHONY: test

ifneq ($(LCOV)$(GENHTML),)
coverage: test
	./runtests
	@mkdir -p coverage
	$(LCOV) -b src -d build/src -o coverage/report.info
	$(GENHTML) coverage/report.info -o coverage
.PHONY: coverage
endif

libmucs:
	@$(MAKE) -C libmucs
.PHONY: libmucs

$(EXE): libmucs $(OBJS)
	@echo "build/src/*.o -> $@"
	@$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(TEST_EXE): libmucs $(ALL_OBJS)
	@echo "build/**/*.o -> $@"
	@$(CXX) $(LDFLAGS) $(ALL_OBJS) -o $@ $(LDLIBS) -lgcov

build/%.o: %.cpp | build_dirs
	@echo "$< -> $@"
	@$(CXX) $(CFLAGS) $(COVFLAGS) -c -MMD $(DEFINES) $< -o $@

install: $(EXE)
	mkdir -p $(DEST)/{bin,config.d,submissions}
	chmod 775 $(DEST)/{bin,config.d}
	chmod 770 $(DEST)/submissions
	$(INSTALL) -m 755 $(SCRIPTS) $(DEST)/bin
	$(INSTALL) -m 4755 $(EXE) $(DEST)/bin
.PHONY: install

clean:
	@$(MAKE) -C libmucs clean
	rm -rf $(EXE) $(TEST_EXE) build coverage
.PHONY: clean

wipe: clean
	rm -rf test_root/submissions
.PHONY: wipe

build_dirs:
	@mkdir -p build/src build/test
.PHONY: build_dirs


-include $(OBJS:%.o=%.d)
