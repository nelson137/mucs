define get_obj_names
$(patsubst %.cpp,$(OBJ_D)/%.o,$(1))
endef

TARGET      := mucs
DEST        ?= /group/cs1050
DEST_BIN    := $(DEST)/bin
OBJ_D       := obj
SRC_D       := src
TEST_D      := test

SRCS        := $(wildcard $(SRC_D)/*.cpp)
OBJS        := $(call get_obj_names,$(SRCS))

TEST_SRCS   := $(filter-out $(SRC_D)/main.cpp,$(SRCS)) $(wildcard $(TEST_D)/*.cpp)
TEST_OBJS   := $(call get_obj_names,$(TEST_SRCS))

SCRIPTS     := $(wildcard bin/mucs-*)

INSTALL     := /usr/bin/install -g cs1050-ta
LIBMUCS     := libmucs
LIBS        := -I$(LIBMUCS)/include -L$(LIBMUCS)/build -lmucs
GPP_BASE    := /usr/bin/g++ -std=c++11 -Wall -Werror -MMD -Iinclude
ifeq ("$(shell which gccfilter)","")
GPP         := $(GPP_BASE)
else
GPP         := gccfilter -c -n -a $(GPP_BASE)
endif


.PHONY: $(TARGET) cpp libmucs test install clean build_dirs all_dirs

cpp: | libmucs $(TARGET)

libmucs:
	cd $(LIBMUCS) && $(MAKE)

test: $(TEST_OBJS) | libmucs
	@echo -n "$^ -> "
	@$(GPP) $^ $(LIBS) -o runtests
	@echo "runtests"

install: $(OBJS) | all_dirs cpp
	@[ -d "$(DEST)" ] || { echo "Destination does not exist: $(DEST)"; false; }
	# Create directory structure
	cd "$(DEST)" && \
		$(INSTALL) -d -m 775 bin config.d && \
		$(INSTALL) -d -m 770 submissions
	# Install C++ files
	$(INSTALL) -m 755 "$(TARGET)" "$(DEST_BIN)/$(TARGET)"
	chmod u+s "$(DEST_BIN)/$(TARGET)"
	# Install scripts
	$(INSTALL) -C -m 770 $(SCRIPTS) -t "$(DEST_BIN)"

clean:
	rm -rf $(OBJ_D) runtests $(TARGET)
	cd $(LIBMUCS) && make clean

$(TARGET): $(OBJS) | libmucs
	@echo -n "$^ -> "
	@$(GPP) $^ $(LIBS) -o $@
	@echo "$@"

$(OBJ_D)/%.o: %.cpp | build_dirs
	@echo -n "$^ -> "
	@$(GPP) -c $< $(LIBS) -o $@
	@echo "$@"

build_dirs:
	@mkdir -p "$(OBJ_D)/$(SRC_D)"
	@mkdir -p "$(OBJ_D)/$(TEST_D)"

all_dirs: | build_dirs
	@mkdir -p "$(DEST_BIN)"

-include $(OBJS:.o=.d)
