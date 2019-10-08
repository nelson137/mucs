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
GPP_BASE    := /usr/bin/g++ -std=c++11 -Wall -Werror -Iinclude
ifeq ("$(shell which gccfilter)","")
GPP         := $(GPP_BASE)
else
GPP         := gccfilter -c -n -a $(GPP_BASE)
endif


.PHONY: test libmucs install clean build_dirs all_dirs

test: $(TEST_OBJS) | libmucs
	$(GPP) $(TEST_OBJS) $(LIBS) -o runtests

libmucs:
	cd $(LIBMUCS) && $(MAKE)

install: $(OBJS) | all_dirs libmucs
	@[ -d "$(DEST)" ] || { echo "Destination does not exist: $(DEST)"; false; }
	# Create directory structure
	cd "$(DEST)" && \
		$(INSTALL) -d -m 775 bin config.d && \
		$(INSTALL) -d -m 770 submissions
	# Install C++ files
	$(GPP) $(OBJS) $(LIBS) -o "$(DEST_BIN)/$(TARGET)"
	chown nwewnh:cs1050-ta "$(TARGET)"
	chmod u+s "$(TARGET)"
	# Install scripts
	$(INSTALL) -C -m 770 $(SCRIPTS) -t "$(DEST_BIN)"

clean:
	@rm -rf $(OBJ_D) runtests cpp
	@cd $(LIBMUCS) && make clean

$(OBJ_D)/%.o: %.cpp | build_dirs
	$(GPP) -c $< $(LIBS) -o $@

build_dirs:
	@mkdir -p "$(OBJ_D)/$(SRC_D)"
	@mkdir -p "$(OBJ_D)/$(TEST_D)"

all_dirs: | build_dirs
	@mkdir -p "$(DEST_BIN)"
