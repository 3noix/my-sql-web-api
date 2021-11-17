CONFIG(debug, debug|release) {
	MODE = debug
}
CONFIG(release, debug|release) {
	MODE = release
}


TEMPLATE = app
TARGET = web-api
CONFIG += c++17 console
DESTDIR = ../bin
OBJECTS_DIR = $$MODE/objects
MOC_DIR = $$MODE/moc
QT += sql websockets


HEADERS +=  src/ApiWebSocketsServer.h


SOURCES +=  src/main.cpp \
			src/SqlConnection.cpp \
			src/ApiWebSocketsServer.cpp \
			src/SqlInterface.cpp \
			src/SqlTransaction.cpp

