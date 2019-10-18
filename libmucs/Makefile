TARGET   := libmucs.a
INCLUDE  := include
SRC_D    := src
BUILD    := build

SRCS     := $(wildcard $(SRC_D)/*.cpp)
OBJS     := $(patsubst src/%.cpp,$(BUILD)/%.o,$(SRCS))

GPP_BASE := /usr/bin/g++ -std=c++11 -Wall -Werror -I$(INCLUDE)
ifeq ("$(shell which gccfilter)","")
GPP      := $(GPP_BASE)
else
GPP      := gccfilter -c -n -a $(GPP_BASE)
endif


main: $(BUILD)/$(TARGET)

clean:
	rm -rf $(BUILD)

$(BUILD)/$(TARGET): $(OBJS)
	@ar rv $@ $^

$(BUILD)/%.o: $(SRC_D)/%.cpp | build_dir
	@echo -n "$< -> "
	@$(GPP) -c -MMD $< -o $@
	@echo "$@"

build_dir:
	@mkdir -p $(BUILD)

-include $(OBJS:.o=.d)
