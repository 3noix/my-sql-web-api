CONFIG(debug, debug|release) {
	MODE = debug
}
CONFIG(release, debug|release) {
	MODE = release
}


TEMPLATE = app
TARGET = WebApi
CONFIG += c++17 console
DESTDIR = ../bin
OBJECTS_DIR = $$MODE/objects
MOC_DIR = $$MODE/moc
QT += sql websockets


HEADERS +=  CODE/ApiWebSocketsServer.h \


SOURCES +=  CODE/main.cpp \
			CODE/SqlConnection.cpp \
			CODE/ApiWebSocketsServer.cpp \
			CODE/SqlInterface.cpp \
			CODE/SqlTransaction.cpp

