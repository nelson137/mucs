HERE := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
HERE := $(HERE:%/=%)

DEST ?= /group/cs1050
DEST_BIN := $(DEST)/bin

PY_SRCS := $(wildcard $(HERE)/mucs/*.py)
PY_DEST := $(DEST)/mucs
CPP_SRC := $(HERE)/cpp/mucs-submit.cpp
CPP_DEST := $(DEST)/bin/mucs-submit
SCRIPTS := $(addprefix $(HERE)/bin/,mucs mucs-gen-roster)

INSTALL := install -g cs1050-ta
GPP := g++ -std=c++11 -Wall -Werror

install:
	[ -d "$(DEST)" ] || false
	# Create directory structure
	cd "$(DEST)" && \
		$(INSTALL) -d -m 775 bin mucs && \
		$(INSTALL) -d -m 770 config.d submissions
	# Install Python files
	$(INSTALL) -C -m 664 $(PY_SRCS) -t "$(PY_DEST)"
	# Install C++ files
	$(GPP) "$(CPP_SRC)" -o "$(CPP_DEST)"
	chown nwewnh:cs1050-ta "$(CPP_DEST)"
	chmod u+s "$(CPP_DEST)"
	# Install scripts
	$(INSTALL) -C -m 770 $(SCRIPTS) -t "$(DEST_BIN)"
