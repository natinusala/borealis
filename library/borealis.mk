mkfile_path	:=	$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir	:=	$(BOREALIS_PATH)/$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

LIBS		:=	-lglfw3 -lEGL -lglapi -ldrm_nouveau -lm $(LIBS)

SOURCES		:=	$(SOURCES) \
				$(current_dir)/lib \
				$(current_dir)/extern/glad \
				$(current_dir)/extern/nanovg-deko/source \
				$(current_dir)/extern/nanovg-deko/source/framework \
				$(current_dir)/extern/libretro-common/compat \
				$(current_dir)/extern/libretro-common/encodings \
				$(current_dir)/extern/libretro-common/features

INCLUDES	:=	$(INCLUDES) \
				$(current_dir)/include \
				$(current_dir)/extern/glad \
				$(current_dir)/extern/nanovg-deko/include \
				$(current_dir)/extern/libretro-common/include
