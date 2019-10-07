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
GPP_BASE    := /usr/bin/g++ -std=c++11 -Wall -Werror -I./include
ifeq ("$(shell which gccfilter)","")
GPP         := $(GPP_BASE)
else
GPP         := gccfilter -c -n -a $(GPP_BASE)
endif


.PHONY: test install dirs

test: $(TEST_OBJS)
	$(GPP) $(TEST_OBJS) -o runtests

install: $(OBJS) | all_dirs
	@[ -d "$(DEST)" ] || { echo "Destination does not exist: $(DEST)"; false; }
	# Create directory structure
	cd "$(DEST)" && \
		$(INSTALL) -d -m 775 bin mucs config.d && \
		$(INSTALL) -d -m 770 submissions
	# Install C++ files
	$(GPP) $(OBJS) -o "$(TARGET)"
	chown nwewnh:cs1050-ta "$(TARGET)"
	chmod u+s "$(TARGET)"
	# Install scripts
	$(INSTALL) -C -m 770 $(SCRIPTS) -t "$(DEST_BIN)"

$(OBJ_D)/%.o: %.cpp | build_dirs
	$(GPP) -c $< -o $@

build_dirs:
	@mkdir -p "$(OBJ_D)/$(SRC_D)"
	@mkdir -p "$(OBJ_D)/$(TEST_D)"

all_dirs: | build_dirs
	@mkdir -p "$(DEST_BIN)"
