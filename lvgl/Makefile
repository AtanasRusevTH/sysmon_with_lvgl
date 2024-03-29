#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# debian linux:
#   apt-get install libglfw-dev
# manjaro linux:
#   pamac install glfw-x11
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

EXE = lvglWidgets
LIB_DIR = lib
PARENT_DIR = ../

# Source directory (replace with your actual path)
SRC_ROOT = src
DEMOS_ROOT = demos/widgets

# Get all source files recursively (assuming .c extension)
SOURCES := $(shell find $(SRC_ROOT) -type f -name "*.c")
SOURCES_DEMOS := $(shell find $(DEMOS_ROOT) -type f -name "*.c")

SOURCES_ALL += lvgl_main.c mouse_cursor_icon.c $(SOURCES) $(SOURCES_DEMOS)

# Get all header files recursively (assuming .h extension)
HEADERS := $(shell find $(SRC_ROOT) -type f -name "*.h")

INCLUDE_DIRS = -I$(LVGL_DIR)
INCLUDE_DIRS += -I$(LVGL_DIR)/demos
INCLUDE_DIRS += -I$(LVGL_DIR)/demos/widgets
INCLUDE_DIRS += -I$(LVGL_DIR)/examples
INCLUDE_DIRS += -I$(LVGL_DIR)/examples/widgets
INCLUDE_DIRS += -I$(LVGL_DIR)/src/draw/sdl
INCLUDE_DIRS += -I$(LVGL_DIR)/src/drivers/sdl

# Object files (automatically generated)
OBJS = $(addsuffix .o, $(basename $(SOURCES_ALL)))

CFLAGS = -pthread -I$(LVGL_DIR) $(INCLUDE_DIRS)

CFLAGS += -g -Wall -Wformat
#LIBS += -lGL `pkg-config --static --libs glfw3`

#CCFLAGS += `pkg-config --cflags glfw3`
LDFLAGS = -Wl,--hash-style=both

# -L$(LIB_DIR) -llvgl_examples -llvgl  -llvgl_thorvg -llvgl_demos
LVGL_LIBS = -lSDL2 # Add LVGL library paths and names

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.c.o: $(SOURCES_ALL)
	$(CC) $(CFLAGS) -c $< -o $@

#$(EXE): $(OBJS)
#	$(CC) -o $@ $^ $(CFLAGS) $(LVGL_LIBS) $(LDFLAGS)

TARGET = liblvglWidgets.a

$(TARGET): $(OBJS)
	ar rcs $(TARGET) $^

.PHONY: copy_lib

copy_lib: $(TARGET)
	cp $(TARGET) $(PARENT_DIR)
	

#all: $(EXE)
.PHONY: all

all: $(TARGET) copy_lib

cl:
	rm -f $(OBJS)
	rm -f $(TARGET)

reb:
	rm -f $(OBJS)
	rm -f $(TARGET)
	make
