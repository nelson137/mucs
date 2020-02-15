TARGET     = $(BUILD_D)/libmucs.a

SRC_D     := src
INCLUDE_D := include
BUILD_D   := build

SRCS      := $(wildcard $(SRC_D)/*.cpp)
OBJS      := $(SRCS:$(SRC_D)/%.cpp=$(BUILD_D)/%.o)

GPP       := /usr/bin/g++ -std=c++11 -pedantic -Wall -Werror -I$(INCLUDE_D)

ifneq ("$(shell whereis gccfilter >/dev/null)","")
GPP       := gccfilter -c -n -a $(GPP)
endif


.PHONY: main
main: $(TARGET)

$(TARGET): $(OBJS)
	@ar rv $@ $^

$(BUILD_D)/%.o: $(SRC_D)/%.cpp | build_dir
	@echo "$< -> $@"
	@$(GPP) -c -MMD $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_D)

.PHONY: build_dir
build_dir:
	@mkdir -p $(BUILD_D)


-include $(OBJS:%.o=%.d)
