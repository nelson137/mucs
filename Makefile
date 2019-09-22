DEST ?= /group/cs1050
DEST_BIN := $(DEST)/bin

SRCS := $(wildcard ./src/*.cpp)
CPP_DEST := $(DEST)/bin/mucs-submit
TEST_SRCS := $(wildcard ./test/*.cpp)
SCRIPTS := ./bin/mucs
TA_SCRIPTS := ./bin/mucs-gen-roster

INSTALL := install -g cs1050-ta
GPP := g++ -std=c++11 -Wall -Werror -I./include

test:
	$(GPP) $(filter-out %/main.cpp,$(SRCS)) $(TEST_SRCS) -o runtests

cpp:
	$(GPP) $(SRCS) -o cpp

install:
	[ -d "$(DEST)" ] || false
	# Create directory structure
	cd "$(DEST)" && \
		$(INSTALL) -d -m 775 bin mucs config.d && \
		$(INSTALL) -d -m 770 submissions
	# Install C++ files
	$(GPP) $(SRCS) -o "$(CPP_DEST)"
	chown nwewnh:cs1050-ta "$(CPP_DEST)"
	chmod u+s "$(CPP_DEST)"
	# Install scripts
	$(INSTALL) -C -m 775 $(SCRIPTS) -t "$(DEST_BIN)"
	$(INSTALL) -C -m 770 $(TA_SCRIPTS) -t "$(DEST_BIN)"

.PHONY: test cpp
