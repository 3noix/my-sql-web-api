CONFIG(debug, debug|release) {
	MODE = debug
}
CONFIG(release, debug|release) {
	MODE = release
}


TEMPLATE = app
TARGET = App
CONFIG += c++17
DESTDIR = ../bin
OBJECTS_DIR = $$MODE/objects
MOC_DIR = $$MODE/moc
QT += widgets websockets


HEADERS +=  CODE/MainWindow.h \
			CODE/EntryDialog.h \
			CODE/EntryTableModel.h


SOURCES +=  CODE/main.cpp \
			CODE/MainWindow.cpp \
			CODE/EntryDialog.cpp \
			CODE/EntryTableModel.cpp \
			CODE/otherFunctions.cpp


RESOURCES += resources.qrc

