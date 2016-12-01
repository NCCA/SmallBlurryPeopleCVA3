TARGET = game

OBJECTS_DIR = obj

INCLUDEPATH +=./include \
                           ./include/imgui

SOURCES+=$$PWD/src/main.cpp \
         $$PWD/src/Scene.cpp \
         $$PWD/src/Camera.cpp \
         $$PWD/src/Grid.cpp \
         $$PWD/src/Character.cpp \
         $$PWD/src/Prop.cpp \
         $$PWD/src/Node.cpp \
         $$PWD/src/imgui/*.cpp \
         $$PWD/src/AssetStore.cpp

HEADERS+=$$PWD/include/Scene.hpp \
         $$PWD/include/Camera.hpp \
         $$PWD/include/Grid.hpp \
         $$PWD/include/Character.hpp \
         $$PWD/include/Prop.hpp \
         $$PWD/include/Node.hpp \
         $$PWD/include/imgui/*.h \
         $$PWD/include/AssetStore.hpp

QMAKE_CXXFLAGS+=$$system(sdl2-config --cflags)
QMAKE_CXXFLAGS+=$$system(python2.7-config --cflags)

CONFIG += c++11

LIBS+=$$system(sdl2-config --libs)
LIBS+=$$system(python2.7-config --libs)

include($(HOME)/NGL/UseNGL.pri)

OTHER_FILES += README.md \
               $$PWD/shaders/*.glsl\
               $$PWD/python/*.py

#This means I can build this on windows.
win32 {
    QT += core gui opengl

    INCLUDEPATH += C:\SDL2\i686-w64-mingw32\include\SDL2
    INCLUDEPATH += C:\SDL2\i686-w64-mingw32\include
    LIBS += -L"C:/SDL2/lib/x64/" -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer
    LIBS += -L"C:/NGL/lib/NGLDebug.lib" -lNGLDebug

    PRE_TARGETDEPS += C:/NGL/lib/NGL.lib
    INCLUDEPATH += -I c:/boost
    INCLUDEPATH += C:/NGL/include/
    DEFINES += GL42
    DEFINES += WIN32
    DEFINES += _WIN32
    DEFINES += _USE_MATH_DEFINES
    LIBS += -LC:/NGL/lib/ -lNGL
    DEFINES += NO_DLL
}
