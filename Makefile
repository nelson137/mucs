DEST ?= /group/cs1050
DEST_BIN := $(DEST)/bin

CPP_SRCS := $(wildcard ./cpp/*.cpp)
CPP_DEST := $(DEST)/bin/mucs-submit
SCRIPTS := ./bin/mucs
TA_SCRIPTS := ./bin/mucs-gen-roster

INSTALL := install -g cs1050-ta
GPP := g++ -std=c++11 -Wall -Werror

install:
	[ -d "$(DEST)" ] || false
	# Create directory structure
	cd "$(DEST)" && \
		$(INSTALL) -d -m 775 bin mucs config.d && \
		$(INSTALL) -d -m 770 submissions
	# Install C++ files
	$(GPP) $(CPP_SRCS) -o "$(CPP_DEST)"
	chown nwewnh:cs1050-ta "$(CPP_DEST)"
	chmod u+s "$(CPP_DEST)"
	# Install scripts
	$(INSTALL) -C -m 775 $(SCRIPTS) -t "$(DEST_BIN)"
	$(INSTALL) -C -m 770 $(TA_SCRIPTS) -t "$(DEST_BIN)"
