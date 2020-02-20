TARGET       := a.out
TEST_TARGET  := runtests
DEST         ?= /group/cs1050
DEST_BIN     := $(DEST)/bin

SRC_D        := src
INCLUDE_D    := include
BUILD_D      := build
LIB_D        := libmucs
TEST_D       := test
COVERAGE_D   := coverage

SRCS         := $(wildcard $(SRC_D)/*.cpp)
OBJS         := $(SRCS:%.cpp=$(BUILD_D)/%.o)
OBJS_NO_MAIN := $(filter-out %/main.o,$(OBJS))

TEST_SRCS    := $(wildcard $(TEST_D)/*.cpp)
TEST_OBJS    := $(TEST_SRCS:%.cpp=$(BUILD_D)/%.o)

SCRIPTS      := $(wildcard bin/mucs-*)

CFLAGS       := -std=c++11 -pedantic -Wall -Werror -Wno-noexcept-type
LDFLAGS      := -L$(LIB_D)/build
LDLIBS       := -lmucs

define can-find-exe
$(shell whereis $1 | awk '{print (NF>1) ? "yes" : "no"; exit}')
endef

CXX          := g++ -I$(INCLUDE_D) -I$(LIB_D)/include
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
	@mkdir -p $(COVERAGE_D)
	$(LCOV) -b $(SRC_D) -d $(BUILD_D)/$(SRC_D) -o $(COVERAGE_D)/report.info
	$(GENHTML) $(COVERAGE_D)/report.info -o $(COVERAGE_D)
.PHONY: coverage
endif

libmucs:
	@$(MAKE) -C $(LIB_D)
.PHONY: libmucs

$(TARGET): $(OBJS) | libmucs
	@echo "$(BUILD_D)/src/*.o -> $@"
	@$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(TEST_TARGET): $(OBJS_NO_MAIN) $(TEST_OBJS) | libmucs
	@echo "$(BUILD_D)/**/*.o -> $@"
	@$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD_D)/%.o: %.cpp | build_dirs
	@echo "$< -> $@"
ifneq ($(LCOV)$(GENHTML),)
ifeq ($(shell echo $(MAKECMDGOALS) | grep -Ewq 'test|coverage' && echo yes),yes)
	$(eval COVFLAGS := --coverage -O0)
endif
endif
	@$(CXX) $(CFLAGS) $(COVFLAGS) -c -MMD $< -o $@

install: $(OBJS) | all_dirs $(TARGET)
	[ -d "$(DEST)" ]
	cd $(DEST) && \
		$(INSTALL) -d -m 775 bin config.d && \
		$(INSTALL) -d -m 770 submissions
	$(INSTALL) -m 755 $(TARGET) $(DEST_BIN)/$(TARGET)
	chmod u+s $(DEST_BIN)/$(TARGET)
	$(INSTALL) -C -m 770 $(SCRIPTS) -t $(DEST_BIN)
.PHONY: install

clean:
	rm -rf $(TARGET) $(TEST_TARGET) $(BUILD_D) $(COVERAGE_D)
	@$(MAKE) -C $(LIB_D)
.PHONY: clean

build_dirs:
	@mkdir -p $(BUILD_D)/$(SRC_D)
	@mkdir -p $(BUILD_D)/$(TEST_D)
.PHONY: build_dirs

all_dirs: | build_dirs
	@mkdir -p $(DEST_BIN)
.PHONY: all_dirs


-include $(OBJS:%.o=%.d)
