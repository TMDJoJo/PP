include(qxtglobalshortcut/qxtglobalshortcut.pri)
SOURCES += \
    Main.cpp \
    Tray/TrayIconMenu.cpp \
    Tray/Tray.cpp \
    Setting.cpp \
    Configure.cpp \
    Transfer.cpp \
    About.cpp \
    RecC/Predecessor.cpp \
    RecC/ev_buffer.cpp \
    RecC/RedC.cpp \
    Socket/SimpleSocket.cpp \
    Socket/PassiveSocket.cpp \
    Socket/ActiveSocket.cpp

HEADERS += \
    Tray/TrayIconMenu.h \
    Tray/Tray.h \
    Setting.h \
    Configure.h \
    Transfer.h \
    About.h \
    RecC/Predecessor.h \
    RecC/ev_buffer.h \
    RecC/RedC.h \
    Socket/StatTimer.h \
    Socket/SimpleSocket.h \
    Socket/PassiveSocket.h \
    Socket/Host.h \
    Socket/ActiveSocket.h

FORMS += \
    Setting.ui \
    Transfer.ui \
    About.ui

QT += \
    network
RC_FILE = PP.rc
