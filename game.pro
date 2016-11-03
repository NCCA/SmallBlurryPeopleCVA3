TARGET = game

OBJECTS_DIR = obj

INCLUDEPATH+=./include

SOURCES+=$$PWD/src/main.cpp \
         $$PWD/src/Scene.cpp

HEADERS+=$$PWD/include/Scene.hpp

QMAKE_CXXFLAGS+=$$system(sdl2-config --cflags)

CONFIG += c++11

LIBS+=$$system(sdl2-config --libs)

include($(HOME)/NGL/UseNGL.pri)

OTHER_FILES += README.md \
               $$PWD/shaders/*.glsl
