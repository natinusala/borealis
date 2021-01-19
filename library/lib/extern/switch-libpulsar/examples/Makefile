#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

ROOTDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules
include $(ROOTDIR)/../deps.mk

#---------------------------------------------------------------------------------

BUILD		:= build
EXAMPLE_DIR	:= $(abspath $(CURDIR))

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections \
			$(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -l$(PLSR_TARGET) -lnx
LIBDIRS	:= $(LIBNX) $(PLSR_DIR)

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export ROOTDIR	:=	$(CURDIR)
export VPATH	:=	$(EXAMPLE_DIR)
export DEPSDIR	:=	$(CURDIR)/$(BUILD)
export LD		:=	$(CC)

export INCLUDE	:=	$(foreach dir,$(LIBDIRS),-I$(dir)/include)
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

CFILES			:=	$(notdir $(wildcard $(EXAMPLE_DIR)/*.c))

#---------------------------------------------------------------------------------

export OFILES		:=	$(CFILES:.c=.o)
export ELFFILES		:=	$(OFILES:.o=.elf)
export NROFILES		:=	$(ELFFILES:.elf=.nro)

.PHONY: $(BUILD) clean all

all: $(BUILD)

$(BUILD):
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $(NROFILES)

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD)

#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

$(NROFILES): %.nro: %.elf
	@echo packing $(notdir $@)
	@elf2nro $< $@

$(ELFFILES): %.elf: %.o
	@echo link $(notdir $@)
	@$(LD) $(LDFLAGS) $< $(LIBPATHS) $(LIBS) -o $@


all	: $(ELFFILES)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------