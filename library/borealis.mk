mkfile_path	:=	$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir	:=	$(BOREALIS_PATH)/$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

LIBS		:=	-lnanovg -ldeko3d -lnx $(LIBS)

# fmt wrapper is there because dkp's base
# makefile doesn't recognize .cc files as c++

SOURCES		:=	$(SOURCES) \
				$(current_dir)/lib \
				$(current_dir)/lib/extern/libretro-common/compat \
				$(current_dir)/lib/extern/libretro-common/encodings \
				$(current_dir)/lib/extern/libretro-common/features

INCLUDES	:=	$(INCLUDES) \
				$(current_dir)/include \
				$(current_dir)/include/borealis/extern \
				$(current_dir)/nanovg/include
