TARGET     = $(BUILD_D)/libmucs.a

SRC_D     := src
INCLUDE_D := include
BUILD_D   := build

SRCS      := $(wildcard $(SRC_D)/*.cpp)
OBJS      := $(SRCS:$(SRC_D)/%.cpp=$(BUILD_D)/%.o)

GPP_BASE  := /usr/bin/g++ -std=c++11 -Wall -Werror -I$(INCLUDE_D)
ifeq ("$(shell whereis gccfilter >/dev/null)","")
GPP       := $(GPP_BASE)
else
GPP       := gccfilter -c -n -a $(GPP_BASE)
endif

.PHONY: main clean build_dir


main: $(TARGET)

$(TARGET): $(OBJS)
	@ar rv $@ $^

$(BUILD_D)/%.o: $(SRC_D)/%.cpp | build_dir
	@echo -n "$< -> "
	@$(GPP) -c -MMD $< -o $@
	@echo $@

clean:
	rm -rf $(BUILD_D)

build_dir:
	@mkdir -p $(BUILD_D)


-include $(OBJS:%.o=%.d)
