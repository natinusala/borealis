#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
#---------------------------------------------------------------------------------
TARGET		:=	nanovg
BUILD		:=	build
SOURCES		:=	source source/nx source/nx/framework
DATA		:=	data
INCLUDES	:=	include
SHADERS		:=	shaders

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIC -ftls-model=local-exec

CFLAGS	:=	-g -Wall \
			-ffunction-sections \
			-fdata-sections \
			$(ARCH)

CFLAGS	+=	$(INCLUDE) -DNDEBUG=1 -D__SWITCH__ -O2

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=c++20

ASFLAGS	:=	-g $(ARCH)

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(SHADERS),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
GLSLFILES	:=	$(foreach dir,$(SHADERS),$(notdir $(wildcard $(dir)/*.glsl)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

ifneq ($(strip $(DATA)),)
	DATA_SHADERS := $(DATA)
	DATA_TARGETS := $(patsubst %.glsl, $(DATA_SHADERS)/%.dksh, $(GLSLFILES))
	DATA_FOLDERS := $(DATA_SHADERS)

	export DATA_DEPS := $(foreach file,$(DATA_TARGETS),$(CURDIR)/$(file))
endif

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

.PHONY: clean all

#---------------------------------------------------------------------------------
all: $(DATA_TARGETS) lib/lib$(TARGET).a

lib:
	@[ -d $@ ] || mkdir -p $@

build:
	@[ -d $@ ] || mkdir -p $@

lib/lib$(TARGET).a : lib build $(DATA_TARGETS)
	@$(MAKE) --no-print-directory OUTPUT=$(CURDIR)/$@ -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------

ifneq ($(strip $(DATA_TARGETS)),)

$(DATA_TARGETS): | $(DATA_FOLDERS)

$(DATA_FOLDERS):
	@mkdir -p $@

$(DATA_SHADERS)/%_vsh.dksh: %_vsh.glsl
	@echo {vert} $(notdir $<)
	@uam -s vert -o $@ $<

$(DATA_SHADERS)/%_tcsh.dksh: %_tcsh.glsl
	@echo {tess_ctrl} $(notdir $<)
	@uam -s tess_ctrl -o $@ $<

$(DATA_SHADERS)/%_tesh.dksh: %_tesh.glsl
	@echo {tess_eval} $(notdir $<)
	@uam -s tess_eval -o $@ $<

$(DATA_SHADERS)/%_gsh.dksh: %_gsh.glsl
	@echo {geom} $(notdir $<)
	@uam -s geom -o $@ $<

$(DATA_SHADERS)/%_fsh.dksh: %_fsh.glsl
	@echo {frag} $(notdir $<)
	@uam -s frag -o $@ $<

$(DATA_SHADERS)/%.dksh: %.glsl
	@echo {comp} $(notdir $<)
	@uam -s comp -o $@ $<

endif

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr build lib

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT)	:	$(DATA_DEPS) $(OFILES)

$(OFILES_SRC)	: $(HFILES)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h		:	%.bin
%.dksh.o %_dksh.h	:	%.dksh
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
