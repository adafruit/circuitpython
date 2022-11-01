# Platform-specific defaults/overrides for Darwin

MACOS_VERSION=$(shell sw_vers -productVersion)
-include mk/platform-$(shell uname -s | tr ' ' -)-$(MACOS_VERSION).mk
-include mk/platform-$(shell uname -sm | tr ' ' -).mk
-include mk/platform-$(shell uname -sm | tr ' ' -)-$(MACOS_VERSION).mk

# Check for gnu make 4.2 or higher
MAKE_MIN_VER=4.2
MAKE_VERSION=$(shell $(MAKE) --version | head -1)
MAKE_VERSION_OK=$(shell echo $(MAKE_VERSION) | awk '/GNU Make / && $$3 >= $(MAKE_MIN_VER) { print 1 }')

ifeq ($(origin MAKE),default)
ifneq ($(MAKE_VERSION_OK),true)
# try gmake instead, if found
GMAKE=$(shell which gmake)
ifneq ($(GMAKE),)
ifeq ($(shell $(GMAKE) --version | awk '/GNU Make / && $$3 >= $(MAKE_MIN_VER) { print "true"}'),true)
MAKE:=$(GMAKE)
endif
endif
endif
endif

ifdef $(MAKE_VERSION_OK)
$(error GNU Make >= $(MAKE_MIN_VER) required, you have $(MAKE_VERSION) at $(MAKE) \
Install gnu make with 'brew install make' or specify make with MAKE=path/to/gmake-4.2\
THe install-deps target can be used to install dependencies)
endif

install-deps::
	@if [[ ! -x "$$(which brew)" ]]; then \
		echo "Homebrew must be installed; See https://brew.sh/"; \
		exit 1; \
  	fi
	brew install -q git python3 gettext uncrustify rust
	brew install -q gcc-arm-embedded

check-deps::
	@which $(brew --prefix)/git || echo "Homebrew-installed git not found" && err=1; \
	[ -n "$err" ] && exit 1
