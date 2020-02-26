TARGET       := a.out
TEST_TARGET  := runtests
DEST         ?= /group/cs1050

SRCS         := $(wildcard src/*.cpp)
OBJS         := $(SRCS:%.cpp=build/%.o)
OBJS_NO_MAIN := $(filter-out %/main.o,$(OBJS))

TEST_SRCS    := $(wildcard test/*.cpp)
TEST_OBJS    := $(TEST_SRCS:%.cpp=build/%.o)

ALL_OBJS     := $(OBJS_NO_MAIN) $(TEST_OBJS)

SCRIPTS      := $(wildcard bin/mucs-*)

CFLAGS       := -std=c++11 -pedantic -Wall -Werror -Wno-noexcept-type
LDFLAGS      := -Llibmucs/build
LDLIBS       := -lmucs

define can-find-exe
$(shell whereis $1 | awk 'NF>1 {print "yes"; exit}')
endef

CXX          := g++ -Iinclude -Ilibmucs/include
ifeq ($(call can-find-exe,gccfilter),yes)
CXX          := gccfilter -c -n -a $(CXX)
endif

INSTALL      := install -g cs1050-ta

ifeq ($(call can-find-exe,lcov),yes)
LCOV         := lcov -c --no-external
endif

ifeq ($(call can-find-exe,genhtml),yes)
GENHTML      := genhtml --legend --function-coverage --demangle-cpp
endif

TESTING      := $(shell echo $(MAKECMDGOALS) | grep -Ewq 'test|$(TEST_TARGET)|coverage' && echo yes)

define local-mucs-root
$(eval DATA_CONFIG := -DMUCS_ROOT_X='$(shell pwd)/test_root')
endef


main: $(TARGET)
.PHONY: main

test: $(TEST_TARGET)
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

$(TARGET): libmucs $(OBJS)
	@echo "build/src/*.o -> $@"
	@$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(TEST_TARGET): libmucs $(ALL_OBJS)
	@echo "build/**/*.o -> $@"
	@$(CXX) $(LDFLAGS) $(ALL_OBJS) -o $@ $(LDLIBS)

build/%.o: %.cpp | build_dirs
	@echo "$< -> $@"
ifeq ($(MUCS_ROOT),local)
	$(call local-mucs-root)
endif
ifeq ($(TESTING),yes)
	$(call local-mucs-root)
ifneq ($(LCOV)$(GENHTML),)
	$(eval COVFLAGS := --coverage -O0)
endif
endif
	@$(CXX) $(CFLAGS) $(COVFLAGS) -c -MMD $(DATA_CONFIG) $< -o $@

install: $(TARGET)
	mkdir -p $(DEST)/{bin,config.d,submissions}
	chmod 775 $(DEST)/{bin,config.d}
	chmod 770 $(DEST)/submissions
	$(INSTALL) -m 755 $(SCRIPTS) $(DEST)/bin
	$(INSTALL) -m 4755 $(TARGET) $(DEST)/bin
.PHONY: install

clean:
	@$(MAKE) -C libmucs clean
	rm -rf $(TARGET) $(TEST_TARGET) build coverage
.PHONY: clean

wipe: clean
	rm -rf test_root/submissions
.PHONY: wipe

build_dirs:
	@mkdir -p build/src build/test
.PHONY: build_dirs


-include $(OBJS:%.o=%.d)
