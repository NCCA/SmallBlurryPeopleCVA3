TARGET = game

OBJECTS_DIR = obj

INCLUDEPATH +=./include \
              ./include/imgui

SOURCES+=$$PWD/src/main.cpp \
         $$PWD/src/Scene.cpp \
         $$PWD/src/Camera.cpp \
         $$PWD/src/Grid.cpp \
         $$PWD/src/AI.cpp \
         $$PWD/src/Character.cpp \
         $$PWD/src/Inventory.cpp \
         $$PWD/src/Node.cpp \
         $$PWD/src/GridTile.cpp \
         $$PWD/src/NodeNetwork.cpp \
         $$PWD/src/AssetStore.cpp \
         $$PWD/src/Framebuffer.cpp \
         $$PWD/src/Gui.cpp \
         $$PWD/src/Button.cpp \
         $$PWD/src/Baddie.cpp \
         $$PWD/src/Utility.cpp \
         $$PWD/src/Commands.cpp \
         $$PWD/src/Prefs.cpp \
         $$PWD/src/PrefsParser.cpp \
         $$PWD/src/IVal.cpp \
         $$PWD/src/Light.cpp \
         $$PWD/src/MapList.cpp \
         $$PWD/src/ParticleSystem.cpp \
         $$PWD/src/TerrainHeightTracer.cpp

HEADERS+=$$PWD/include/Scene.hpp \
         $$PWD/include/Camera.hpp \
         $$PWD/include/Grid.hpp \
         $$PWD/include/AI.hpp \
         $$PWD/include/Character.hpp \
         $$PWD/include/Inventory.hpp \
         $$PWD/include/Node.hpp \
         $$PWD/include/GridTile.hpp \
         $$PWD/include/NodeNetwork.hpp \
         $$PWD/include/AssetStore.hpp \
         $$PWD/include/Framebuffer.hpp \
         $$PWD/include/Gui.hpp \
         $$PWD/include/Button.hpp \
         $$PWD/include/Baddie.hpp \
         $$PWD/include/Utility.hpp \
         $$PWD/include/Commands.hpp \
         $$PWD/include/Prefs.hpp \
         $$PWD/include/PrefsParser.hpp \
         $$PWD/include/IVal.hpp \
         $$PWD/include/Light.hpp \
         $$PWD/include/MapList.hpp \
         $$PWD/include/ParticleSystem.hpp \
         $$PWD/include/TerrainHeightTracer.hpp

QMAKE_CXXFLAGS+=$$system(sdl2-config --cflags)
QMAKE_CXXFLAGS+=$$system(python2.7-config --cflags)

CONFIG += c++14

LIBS+=$$system(sdl2-config --libs)
LIBS+= -lSDL2_image
LIBS+=$$system(python2.7-config --libs)

include($(HOME)/NGL/UseNGL.pri)

OTHER_FILES += README.md \
               $$PWD/shaders/*.glsl \
               $$PWD/python/*.py \
               $$PWD/python/gameUtils/*.py \
               $$PWD/python/noise/*.py \
               $$PWD/names/*.txt \
               $$PWD/preferences.conf


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

DISTFILES += \
    shaders/fragCharDiffuse.glsl
