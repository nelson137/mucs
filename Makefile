TARGET       := a.out
TEST_TARGET  := runtests
DEST         ?= /group/cs1050

SRCS         := $(wildcard src/*.cpp)
OBJS         := $(SRCS:%.cpp=build/%.o)
OBJS_NO_MAIN := $(filter-out %/main.o,$(OBJS))

TEST_SRCS    := $(wildcard test/*.cpp)
TEST_OBJS    := $(TEST_SRCS:%.cpp=build/%.o)

SCRIPTS      := $(wildcard bin/mucs-*)

CFLAGS       := -std=c++11 -pedantic -Wall -Werror -Wno-noexcept-type
LDFLAGS      := -Llibmucs/build
LDLIBS       := -lmucs

define can-find-exe
$(shell whereis $1 | awk '{print (NF>1) ? "yes" : "no"; exit}')
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

$(TARGET): $(OBJS) | libmucs
	@echo "build/src/*.o -> $@"
	@$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(TEST_TARGET): $(OBJS_NO_MAIN) $(TEST_OBJS) | libmucs
	@echo "build/**/*.o -> $@"
	@$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

build/%.o: %.cpp | build_dirs
	@echo "$< -> $@"
ifneq ($(LCOV)$(GENHTML),)
ifeq ($(shell echo $(MAKECMDGOALS) | grep -Ewq 'test|coverage' && echo yes),yes)
	$(eval COVFLAGS := --coverage -O0)
endif
endif
	@$(CXX) $(CFLAGS) $(COVFLAGS) -c -MMD $< -o $@

install: $(OBJS) | all_dirs $(TARGET)
	mkdir -p $(DEST)/{bin,config.d,submissions}
	chmod 775 $(DEST)/{bin,config.d}
	chmod 770 $(DEST)/submissions
	$(INSTALL) -m 755 $(SCRIPTS) $(DEST)/bin
	$(INSTALL) -m 4755 $(TARGET) $(DEST)/bin
.PHONY: install

clean:
	rm -rf $(TARGET) $(TEST_TARGET) build coverage
	@$(MAKE) -C libmucs
.PHONY: clean

build_dirs:
	@mkdir -p build/src
	@mkdir -p build/test
.PHONY: build_dirs

all_dirs: | build_dirs
	@mkdir -p $(DEST)/bin
.PHONY: all_dirs


-include $(OBJS:%.o=%.d)
