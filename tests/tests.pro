TEMPLATE = app
TARGET = tests
INCLUDEPATH += .

HEADERS += catch.hpp ../src/element/baseelement.h  ../src/element/element.h
SOURCES += ../src/element/baseelement.cpp ../src/element/element.cpp
SOURCES += baseelement_test.cpp element_test.cpp test-main.cpp
