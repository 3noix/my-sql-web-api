CONFIG(debug, debug|release) {
	MODE = debug
}
CONFIG(release, debug|release) {
	MODE = release
}


TEMPLATE = app
TARGET = app-qt
CONFIG += c++17
DESTDIR = ../bin
OBJECTS_DIR = $$MODE/objects
MOC_DIR = $$MODE/moc
QT += widgets websockets


HEADERS +=  src/MainWindow.h \
			src/EntryDialog.h \
			src/EntryTableModel.h


SOURCES +=  src/main.cpp \
			src/MainWindow.cpp \
			src/EntryDialog.cpp \
			src/EntryTableModel.cpp \
			src/otherFunctions.cpp


RESOURCES += resources.qrc

