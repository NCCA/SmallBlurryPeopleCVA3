#############################################################################
# Makefile for building: game
# Generated by qmake (3.0) (Qt 5.7.0)
# Project:  game.pro
# Template: app
# Command: /opt/Qt5.7.0/5.7/gcc_64/bin/qmake -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug -o Makefile game.pro
#############################################################################

MAKEFILE      = Makefile

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT5BUILD -DNGL_DEBUG -DQT_QML_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB
CFLAGS        = -pipe -g -Wall -W -D_REENTRANT -fPIC $(DEFINES)
CXXFLAGS      = -pipe -I/usr/local/include/SDL2 -D_REENTRANT -msse -msse2 -msse3 -march=native -march=native -g -std=gnu++11 -Wall -W -Wno-unused-parameter -D_REENTRANT -fPIC $(DEFINES)
INCPATH       = -I. -Iinclude -I../../NGL/include -I/opt/Qt5.7.0/5.7/gcc_64/include -I/opt/Qt5.7.0/5.7/gcc_64/include/QtGui -I/opt/Qt5.7.0/5.7/gcc_64/include/QtCore -I. -I/opt/Qt5.7.0/5.7/gcc_64/mkspecs/linux-g++
QMAKE         = /opt/Qt5.7.0/5.7/gcc_64/bin/qmake
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
DISTNAME      = game1.0.0
DISTDIR = /home/i7624405/CA2/SmallBlurryPeopleCVA3/obj/game1.0.0
LINK          = g++
LFLAGS        = -Wl,-z,origin -Wl,-rpath,\$$ORIGIN/L/home/i7624405/NGL/lib -Wl,-rpath,/home/i7624405/NGL/lib -Wl,-rpath,/opt/Qt5.7.0/5.7/gcc_64/lib
LIBS          = $(SUBLIBS) -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2 -L/home/i7624405/NGL/lib -l NGL -ltiff -L/opt/Qt5.7.0/5.7/gcc_64/lib -lQt5Gui -L/usr/lib64 -lQt5Core -lGL -lpthread 
AR            = ar cqs
RANLIB        = 
SED           = sed
STRIP         = strip

####### Output directory

OBJECTS_DIR   = obj/

####### Files

SOURCES       = src/main.cpp 
OBJECTS       = obj/main.o
DIST          = /opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/spec_pre.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/unix.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/linux.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/sanitize.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/gcc-base.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/gcc-base-unix.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/g++-base.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/g++-unix.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/qconfig.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dcore.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dcore_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dextras.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dextras_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dinput.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dinput_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dlogic.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dlogic_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquick.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquick_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickextras.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickextras_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickinput.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickinput_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickrender.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickrender_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3drender.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3drender_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_bluetooth.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_bluetooth_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_bootstrap_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_charts.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_charts_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_clucene_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_concurrent.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_concurrent_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_core.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_core_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_datavisualization.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_datavisualization_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_dbus.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_dbus_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_designer.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_designer_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_designercomponents_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_eglfs_device_lib_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gamepad.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gamepad_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gui.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gui_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_help.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_help_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_location.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_location_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimedia.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimedia_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimediawidgets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimediawidgets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_network.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_network_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_nfc.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_nfc_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_opengl.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_opengl_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_openglextensions.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_openglextensions_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_packetprotocol_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_platformsupport_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_positioning.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_positioning_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_printsupport.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_printsupport_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_purchasing.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_purchasing_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qml.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qml_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmldebug_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmldevtools_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmltest.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmltest_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qtmultimediaquicktools_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quick.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quick_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickcontrols2.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickcontrols2_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickparticles_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quicktemplates2_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickwidgets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickwidgets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_script.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_script_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scripttools.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scripttools_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scxml.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scxml_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sensors.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sensors_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialbus.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialbus_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialport.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialport_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sql.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sql_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_svg.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_svg_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_testlib.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_testlib_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_uiplugin.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_uitools.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_uitools_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webchannel.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webchannel_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webengine.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webengine_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginecore.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginecore_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginecoreheaders_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginewidgets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginewidgets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_websockets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_websockets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webview.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webview_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_widgets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_widgets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_x11extras.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_x11extras_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xcb_qpa_lib_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xml.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xml_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xmlpatterns.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xmlpatterns_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qt_functions.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qt_config.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/linux-g++/qmake.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/spec_post.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/exclusive_builds.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/default_pre.prf \
		../../NGL/UseNGL.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/resolve_config.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/default_post.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qml_debug.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/warn_on.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qt.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/resources.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/moc.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/unix/opengl.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/unix/thread.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/file_copies.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/testcase_targets.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/exceptions.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/yacc.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/lex.prf \
		game.pro  src/main.cpp
QMAKE_TARGET  = game
DESTDIR       = 
TARGET        = game


first: all
####### Build rules

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: game.pro .qmake.cache /opt/Qt5.7.0/5.7/gcc_64/mkspecs/linux-g++/qmake.conf /opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/spec_pre.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/unix.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/linux.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/sanitize.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/gcc-base.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/gcc-base-unix.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/g++-base.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/g++-unix.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/qconfig.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dcore.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dcore_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dextras.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dextras_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dinput.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dinput_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dlogic.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dlogic_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquick.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquick_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickextras.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickextras_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickinput.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickinput_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickrender.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickrender_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3drender.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3drender_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_bluetooth.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_bluetooth_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_bootstrap_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_charts.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_charts_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_clucene_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_concurrent.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_concurrent_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_core.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_core_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_datavisualization.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_datavisualization_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_dbus.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_dbus_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_designer.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_designer_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_designercomponents_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_eglfs_device_lib_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gamepad.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gamepad_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gui.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gui_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_help.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_help_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_location.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_location_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimedia.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimedia_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimediawidgets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimediawidgets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_network.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_network_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_nfc.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_nfc_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_opengl.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_opengl_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_openglextensions.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_openglextensions_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_packetprotocol_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_platformsupport_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_positioning.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_positioning_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_printsupport.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_printsupport_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_purchasing.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_purchasing_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qml.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qml_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmldebug_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmldevtools_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmltest.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmltest_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qtmultimediaquicktools_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quick.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quick_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickcontrols2.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickcontrols2_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickparticles_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quicktemplates2_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickwidgets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickwidgets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_script.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_script_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scripttools.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scripttools_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scxml.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scxml_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sensors.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sensors_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialbus.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialbus_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialport.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialport_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sql.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sql_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_svg.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_svg_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_testlib.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_testlib_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_uiplugin.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_uitools.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_uitools_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webchannel.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webchannel_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webengine.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webengine_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginecore.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginecore_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginecoreheaders_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginewidgets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginewidgets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_websockets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_websockets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webview.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webview_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_widgets.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_widgets_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_x11extras.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_x11extras_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xcb_qpa_lib_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xml.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xml_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xmlpatterns.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xmlpatterns_private.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qt_functions.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qt_config.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/linux-g++/qmake.conf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/spec_post.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/exclusive_builds.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/default_pre.prf \
		../../NGL/UseNGL.pri \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/resolve_config.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/default_post.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qml_debug.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/warn_on.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qt.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/resources.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/moc.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/unix/opengl.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/unix/thread.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/file_copies.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/testcase_targets.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/exceptions.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/yacc.prf \
		/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/lex.prf \
		game.pro \
		/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Gui.prl \
		/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Core.prl
	$(QMAKE) -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug -o Makefile game.pro
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/spec_pre.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/unix.conf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/linux.conf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/sanitize.conf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/gcc-base.conf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/gcc-base-unix.conf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/g++-base.conf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/common/g++-unix.conf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/qconfig.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dcore.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dcore_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dextras.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dextras_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dinput.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dinput_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dlogic.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dlogic_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquick.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquick_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickextras.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickextras_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickinput.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickinput_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickrender.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3dquickrender_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3drender.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_3drender_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_bluetooth.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_bluetooth_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_bootstrap_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_charts.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_charts_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_clucene_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_concurrent.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_concurrent_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_core.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_core_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_datavisualization.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_datavisualization_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_dbus.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_dbus_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_designer.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_designer_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_designercomponents_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_eglfs_device_lib_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gamepad.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gamepad_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gui.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_gui_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_help.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_help_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_location.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_location_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimedia.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimedia_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimediawidgets.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_multimediawidgets_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_network.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_network_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_nfc.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_nfc_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_opengl.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_opengl_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_openglextensions.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_openglextensions_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_packetprotocol_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_platformsupport_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_positioning.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_positioning_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_printsupport.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_printsupport_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_purchasing.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_purchasing_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qml.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qml_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmldebug_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmldevtools_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmltest.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qmltest_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_qtmultimediaquicktools_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quick.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quick_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickcontrols2.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickcontrols2_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickparticles_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quicktemplates2_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickwidgets.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_quickwidgets_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_script.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_script_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scripttools.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scripttools_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scxml.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_scxml_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sensors.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sensors_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialbus.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialbus_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialport.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_serialport_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sql.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_sql_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_svg.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_svg_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_testlib.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_testlib_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_uiplugin.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_uitools.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_uitools_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webchannel.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webchannel_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webengine.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webengine_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginecore.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginecore_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginecoreheaders_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginewidgets.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webenginewidgets_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_websockets.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_websockets_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webview.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_webview_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_widgets.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_widgets_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_x11extras.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_x11extras_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xcb_qpa_lib_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xml.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xml_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xmlpatterns.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/modules/qt_lib_xmlpatterns_private.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qt_functions.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qt_config.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/linux-g++/qmake.conf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/spec_post.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/exclusive_builds.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/default_pre.prf:
../../NGL/UseNGL.pri:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/resolve_config.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/default_post.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qml_debug.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/warn_on.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/qt.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/resources.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/moc.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/unix/opengl.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/unix/thread.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/file_copies.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/testcase_targets.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/exceptions.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/yacc.prf:
/opt/Qt5.7.0/5.7/gcc_64/mkspecs/features/lex.prf:
game.pro:
/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Gui.prl:
/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Core.prl:
qmake: FORCE
	@$(QMAKE) -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug -o Makefile game.pro

qmake_all: FORCE


all: Makefile $(TARGET)

dist: distdir FORCE
	(cd `dirname $(DISTDIR)` && $(TAR) $(DISTNAME).tar $(DISTNAME) && $(COMPRESS) $(DISTNAME).tar) && $(MOVE) `dirname $(DISTDIR)`/$(DISTNAME).tar.gz . && $(DEL_FILE) -r $(DISTDIR)

distdir: FORCE
	@test -d $(DISTDIR) || mkdir -p $(DISTDIR)
	$(COPY_FILE) --parents $(DIST) $(DISTDIR)/
	$(COPY_FILE) --parents src/main.cpp $(DISTDIR)/


clean: compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


distclean: clean 
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) .qmake.stash
	-$(DEL_FILE) Makefile


####### Sub-libraries

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

check: first

benchmark: first

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_moc_header_make_all:
compiler_moc_header_clean:
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

####### Compile

obj/main.o: src/main.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o obj/main.o src/main.cpp

####### Install

install:  FORCE

uninstall:  FORCE

FORCE:
