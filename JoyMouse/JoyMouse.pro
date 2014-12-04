HEADERS += \
    joypad.h \
    mainwindow.h \
    joymouse.h \
    mapdialog.h \
    sensivitydialog.h \
    mapaxisdialog.h \
    joypaderror.h

SOURCES += \
    joypad.cpp \
    main.cpp \
    mainwindow.cpp \
    joymouse.cpp \
    mapdialog.cpp \
    sensivitydialog.cpp \
    mapaxisdialog.cpp \
    joypaderror.cpp

FORMS += \
    mainwindow.ui \
    mapdialog.ui \
    sensivitydialog.ui \
    mapaxisdialog.ui

RESOURCES += \
    resources.qrc

LIBS += \
    -lX11 \
    -lXtst \
    -lXi \
    -lexplain

QMAKE_CXXFLAGS += -std=c++11
