#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

#CXX = g++
#CXX = clang++

APP = raven
EXE = $(APP)
SOURCES = main_glfw.cpp app.cpp audio.cpp widgets.cpp
SOURCES += imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp
SOURCES += imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp
SOURCES += imgui/imguihelper.cpp imgui/imgui_plot.cpp imgui/imguifilesystem.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -I. -I./imgui
CXXFLAGS += -g -Wall -Wformat
LIBS =

CXXFLAGS += -I./libs/opentimelineio/src/deps
CXXFLAGS += -I./libs/opentimelineio/src/deps/optional-lite/include
CXXFLAGS += -I./libs/opentimelineio/src/deps/Imath/src/Imath
CXXFLAGS += -I./libs/opentimelineio/build/temp.macosx-10.9-x86_64-3.9/src/deps/Imath/config
CXXFLAGS += -I./libs/opentimelineio/src
LIBS += ./libs/opentimelineio/build/temp.macosx-10.9-x86_64-3.9/src/opentime/libopentime.a
LIBS += ./libs/opentimelineio/build/temp.macosx-10.9-x86_64-3.9/src/deps/Imath/src/Imath/libImath-3_2.a
LIBS += ./libs/opentimelineio/build/temp.macosx-10.9-x86_64-3.9/src/opentimelineio/libopentimelineio.a

##---------------------------------------------------------------------
## OPENGL LOADER
##---------------------------------------------------------------------

## Using OpenGL loader: gl3w [default]
SOURCES += libs/gl3w/GL/gl3w.c
CXXFLAGS += -Ilibs/gl3w -DIMGUI_IMPL_OPENGL_LOADER_GL3W

## Using OpenGL loader: glew
## (This assumes a system-wide installation)
# CXXFLAGS += -lGLEW -DIMGUI_IMPL_OPENGL_LOADER_GLEW

## Using OpenGL loader: glad
# SOURCES += libs/glad/src/glad.c
# CXXFLAGS += -Ilibs/glad/include -DIMGUI_IMPL_OPENGL_LOADER_GLAD

## Using OpenGL loader: glbinding
## This assumes a system-wide installation
## of either version 3.0.0 (or newer)
# CXXFLAGS += -lglbinding -DIMGUI_IMPL_OPENGL_LOADER_GLBINDING3
## or the older version 2.x
# CXXFLAGS += -lglbinding -DIMGUI_IMPL_OPENGL_LOADER_GLBINDING2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL `pkg-config --static --libs glfw3` -lasound

	CXXFLAGS += `pkg-config --cflags glfw3` -pthread -DHAVE_STDINT_H
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreAudio -framework AudioToolbox
	LIBS += -L/opt/local/lib -L/opt/homebrew/lib -L/usr/local/lib
	#LIBS += -lglfw3
	LIBS += -lglfw

	CXXFLAGS += -I/opt/local/include -I/opt/homebrew/include -I/usr/local/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(XCOMPILE), Windows)
	ECHO_MESSAGE = "Windows cross-compile"
	CXX = x86_64-w64-mingw32-g++
	EXE = $(APP).exe
	CXXFLAGS += -I./libs/glfw/include
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

CXXONLYFLAGS = -std=c++11

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(CXXONLYFLAGS) -c -o $@ $<

%.o:imgui/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXONLYFLAGS) -c -o $@ $<

%.o:libs/gl3w/GL/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o:libs/glad/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)
