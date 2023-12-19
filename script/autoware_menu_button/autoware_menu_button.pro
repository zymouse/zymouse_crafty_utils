TARGET = autoware_menu_button
TEMPLATE = app
INCLUDEPATH += .

# Add the necessary Qt modules here
QT += widgets network core svg core gui

SOURCES += autoware_menu_button.cpp
RESOURCES += resources.qrc

# qmake ../autoware_menu_button.pro
# make
# ./autoware_menu_button