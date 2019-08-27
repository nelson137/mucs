HERE = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRCS = $(wildcard $(HERE)mucs/*)
DEST = /group/cs1050

INSTALL = install -g cs1050-ta

install:
	# Create directory structure
	cd "$(DEST)"; \
		$(INSTALL) -d -m 775 bin mucs; \
		$(INSTALL) -d -m 770 config.d submissions
	# Copy project files
	$(INSTALL) -C -m 664 $(SRCS) -t "$(DEST)/mucs"
	# Setup project files
	chmod +x "$(DEST)/mucs/mucs.py"
	ln -fs "$(DEST)/mucs/mucs.py" "$(DEST)/bin/mucs"
