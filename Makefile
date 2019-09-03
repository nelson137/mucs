HERE = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
HERE = $(HERE:%/=%)

DEST ?= /group/cs1050
DEST_BIN = $(DEST)/bin

PY_SRCS = $(wildcard $(HERE)/mucs/*.py)
PY_DEST = $(DEST)/mucs
CPP_SRC = $(HERE)/cpp/mucs-submit.cpp
CPP_DEST = $(DEST)/bin/mucs-submit

INSTALL = install -g cs1050-ta
GPP = g++ -std=c++11 -Wall -Werror

install:
	[ -d "$(DEST)" ] || false
	# Create directory structure
	cd "$(DEST)" && \
		$(INSTALL) -d -m 775 bin mucs && \
		$(INSTALL) -d -m 770 config.d submissions
	# Install Python files
	$(INSTALL) -C -m 664 $(PY_SRCS) -t "$(PY_DEST)"
	chmod +x "$(DEST)/mucs/mucs.py"
	ln -fs "$(DEST)/mucs/mucs.py" "$(DEST_BIN)"
	# Install C++ files
	$(GPP) "$(CPP_SRC)" -o "$(CPP_DEST)"
	chown nwewnh:cs1050-ta "$(CPP_DEST)"
	chmod g+s "$(CPP_DEST)"
