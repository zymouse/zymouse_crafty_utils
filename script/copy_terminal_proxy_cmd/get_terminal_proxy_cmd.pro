TEMPLATE = app
TARGET = get_terminal_proxy_cmd
INCLUDEPATH += .

# Add the necessary Qt modules here
QT += widgets network core svg

SOURCES += get_terminal_proxy_cmd.cpp
RESOURCES += resources.qrc

# qmake ../get_terminal_proxy_cmd.pro
# make
# ./get_terminal_proxy_cmd

