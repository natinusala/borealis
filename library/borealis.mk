mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(BOREALIS_PATH)/$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

LIBRETRO_COMMON := $(current_dir)/lib/libretro-common/compat $(current_dir)/lib/libretro-common/encodings $(current_dir)/lib/libretro-common/features

LIBS		:= -lglfw3 -lEGL -lglapi -ldrm_nouveau -lm $(LIBS)
SOURCES		:= $(SOURCES) $(current_dir)/lib $(current_dir)/lib/nanovg $(LIBRETRO_COMMON)
INCLUDES	:= $(INCLUDES) $(current_dir)/include $(current_dir)/include/nanovg $(current_dir)/include/libretro-common