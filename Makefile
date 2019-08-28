HERE = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
DEST ?= /group/cs1050

SRCS = $(wildcard $(HERE)mucs/*)
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
	# Copy project files
	$(INSTALL) -C -m 664 $(SRCS) -t "$(DEST)/mucs"
	# Setup project files
	chmod +x "$(DEST)/mucs/mucs.py"
	ln -fs "$(DEST)/mucs/mucs.py" "$(DEST)/bin/mucs"
	# Compile mucs-submit
	$(GPP) "$(CPP_SRC)" -o "$(CPP_DEST)"
	# Setup mucs-submit
	chown :cs1050-ta "$(CPP_DEST)"
	chmod g+s "$(CPP_DEST)"
