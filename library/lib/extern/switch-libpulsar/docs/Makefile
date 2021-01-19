include $(CURDIR)/../deps.mk

BUILD		:= build

export DOCS_DIR		:= $(BUILD)
export DOCS_VERSION	:= $(shell git describe --dirty --always --tags)
export DOCS_CONF	:= $(CURDIR)/Doxyfile
export DOCS_EXAMPLE	:= $(PLSR_DIR)/examples
export DOCS_INPUT	:= $(addprefix $(PLSR_DIR), $(PLSR_INCLUDES) README.md LICENSE.md)
export DOCS_MAIN	:= $(addprefix $(PLSR_DIR), README.md)
export DOCS_DEFINES	:= __SWITCH__
DOCS_DEFINES += $(shell grep -ihR '\#define _' $(DOCS_INPUT) | sed 's/\#define //g; s/ /=/g')

export THEME_VER	:= 95d0f34acb
export THEME_URL	:= "https://github.com/jl-wynen/that_style/archive/$(THEME_VER).tar.gz"
export THEME_FILE	:= $(BUILD)/theme-$(THEME_VER).tar.gz
export THEME_DIR	:= $(BUILD)/theme-$(THEME_VER)
export THEME_COLOR	:= \#256aa7

BIN_CURL	:= $(shell which curl)
BIN_WGET	:= $(shell which wget)
BIN_DOXY	:= $(shell which doxygen)

ifeq ($(strip $(BIN_CURL))$(strip $(BIN_WGET)),)
	$(error "curl or wget not found in path (at least one is needed)")
endif

ifeq ($(strip $(BIN_DOXY))),)
	$(error "doxygen not found in path")
endif

.PHONY: clean docs docs-watch

all: docs

clean:
	@echo clean ...
	@rm -rf $(DOCS_DIR)

theme: $(THEME_DIR)

docs: $(THEME_DIR)
	$(BIN_DOXY) $(DOCS_CONF)

docs-watch: docs
	@echo Watching for changes...
	@while inotifywait -rq $(DOCS_CONF) $(DOCS_INPUT) -e modify,move,create,delete; do $(MAKE) docs; done

$(BUILD):
	@[ -d $@ ] || mkdir -p $@

$(THEME_FILE):
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@[ -z $(BIN_CURL) ] || $(BIN_CURL) -L "$(THEME_URL)" > "$(THEME_FILE)"
	@[ -z $(BIN_WGET) ] || $(BIN_WGET) "$(THEME_URL)" -O "$(THEME_FILE)"

$(THEME_DIR): $(THEME_FILE)
	@[ -d $@ ] || mkdir -p $@
	tar xvf $(THEME_FILE) --strip-components=1 -C $(THEME_DIR)
	sed -i "s/#5f082b/$(THEME_COLOR)/g" "$(THEME_DIR)/that_style.css"

