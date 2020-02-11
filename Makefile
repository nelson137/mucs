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

INSTALL      := /usr/bin/install -g cs1050-ta
LIBS         := -I$(LIB_D)/include -L$(LIB_D)/build -lmucs

GPP          := /usr/bin/g++ -std=c++11 -Wall -Werror -Wno-noexcept-type --coverage -O0 -I$(INCLUDE_D)

ifneq ("$(shell which gccfilter)","")
GPP          := gccfilter -c -n -a $(GPP)
endif

LCOV         := /usr/bin/lcov -c --no-external
GENHTML      := /usr/bin/genhtml --legend --function-coverage --demangle-cpp


.PHONY: main
main: $(TARGET)

.PHONY: test
test: $(TEST_TARGET)

.PHONY: coverage
coverage: test
	./runtests
	@mkdir -p $(COVERAGE_D)
	$(LCOV) -b $(SRC_D) -d $(BUILD_D)/$(SRC_D) -o $(COVERAGE_D)/report.info
	$(GENHTML) $(COVERAGE_D)/report.info -o $(COVERAGE_D)

.PHONY: all
all: main test coverage

.PHONY: libmucs
libmucs:
	@cd $(LIB_D) && $(MAKE)

$(TARGET): $(OBJS) | libmucs
	@echo -n "$^ -> "
	@$(GPP) $^ $(LIBS) -o $@
	@echo $@

$(TEST_TARGET): $(OBJS_NO_MAIN) $(TEST_OBJS) | libmucs
	@echo -n "$^ -> "
	@$(GPP) $^ $(LIBS) -o $@
	@echo $@

$(BUILD_D)/%.o: %.cpp | build_dirs
	@echo -n "$< -> "
	@$(GPP) -c -MMD $< $(LIBS) -o $@
	@echo $@

.PHONY: install
install: $(OBJS) | all_dirs $(TARGET)
	[ -d "$(DEST)" ]
	cd $(DEST) && \
		$(INSTALL) -d -m 775 bin config.d && \
		$(INSTALL) -d -m 770 submissions
	$(INSTALL) -m 755 $(TARGET) $(DEST_BIN)/$(TARGET)
	chmod u+s $(DEST_BIN)/$(TARGET)
	$(INSTALL) -C -m 770 $(SCRIPTS) -t $(DEST_BIN)

.PHONY: clean
clean:
	rm -rf $(TARGET) $(TEST_TARGET) $(BUILD_D) $(COVERAGE_D)
	@cd $(LIB_D) && $(MAKE) clean

.PHONY: build_dirs
build_dirs:
	@mkdir -p $(BUILD_D)/$(SRC_D)
	@mkdir -p $(BUILD_D)/$(TEST_D)

.PHONY: all_dirs
all_dirs: | build_dirs
	@mkdir -p $(DEST_BIN)


-include $(OBJS:%.o=%.d)
