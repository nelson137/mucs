TARGET    := build/libmucs.a

SRCS      := $(wildcard src/*.cpp)
OBJS      := $(SRCS:src/%.cpp=build/%.o)

define can-find-exe
$(shell whereis $1 | awk 'NF>1 {print "yes"; exit}')
endef

CXX       := /usr/bin/g++ -std=c++11 -pedantic -Wall -Werror -Iinclude
ifeq ($(call can-find-exe,gccfilter),yes)
CXX       := gccfilter -c -n -a $(CXX)
endif


.PHONY: main
main: $(TARGET)

$(TARGET): $(OBJS)
	@ar rv $@ $^

build/%.o: src/%.cpp | build_dir
	@echo "$< -> $@"
	@$(CXX) -c -MMD $< -o $@

.PHONY: clean
clean:
	rm -rf build

.PHONY: build_dir
build_dir:
	@mkdir -p build


-include $(OBJS:%.o=%.d)
