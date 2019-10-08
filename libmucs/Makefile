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


$(BUILD)/$(TARGET): $(OBJS)
	ar -r $@ $^

clean:
	rm -rf $(BUILD)

$(BUILD)/%.o: $(SRC_D)/%.cpp | build_dir
	$(GPP) -c $< -o $@

build_dir:
	@mkdir -p $(BUILD)
