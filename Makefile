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

#CXX = g++
#CXX = clang++

EXE = loadTester
IMGUI_DIR = imgui
TP_DIR = thread_pool
LVGL_LIB_PATH = lvgl
ENDLESS_TH_M_DIR = endlessThMngr

SOURCES = main.cpp K3Buffer.cpp K3Proc.cpp K3Key.cpp K3System.cpp imgui_thread.cpp imgui_standard_demo.cpp
SOURCES += $(TP_DIR)/thread_pool.cpp
SOURCES += $(ENDLESS_TH_M_DIR)/endless_th_manager.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl2.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -std=c++14 -pthread -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(IMGUI_DIR)/include -I$(TP_DIR) -I$(ENDLESS_TH_M_DIR) -I$(LVGL_LIB_PATH)/include
CXXFLAGS += -g -Wall -Wformat
LIBS = -L$(LVGL_LIB_PATH) -llvglWidgets `pkg-config --static --libs glfw3` -lSDL2 -lGL -Wl,-Bstatic -Wl,-Bdynamic --verbose
#LIBS += -Bstatic -lGL -lglfw --verbose -LLVGL_LIB_PATH

CXXFLAGS += `pkg-config --cflags glfw3`
LDFLAGS = -Wl,--hash-style=both
CFLAGS = $(CXXFLAGS)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(TP_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(ENDLESS_TH_M_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE) 

$(EXE): $(OBJS) liblvglWidgets.a
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) $(LDFLAGS)

.PHONY: liblvglWidgets.a

liblvglWidgets.a:
	$(call build_lvgl)

.PHONY: build_lvgl

build_lvgl:
	@$(MAKE) -C $(LVGL_LIB_PATH) all -B -v

#liblvglWidgets.a:
#	cd $(LVGL_LIB_PATH)
#	$(MAKE) all -B -v

cl:
	rm -f $(OBJS)
	rm -f $(EXE)
	rm -f liblvglWidgets.a

reb:
	rm -f $(OBJS)
	rm -f $(EXE)
	rm -f liblvglWidgets.a
	make
