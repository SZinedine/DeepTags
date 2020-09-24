TEMPLATE = app
TARGET = tests
INCLUDEPATH += .

HEADERS += catch.hpp ../src/baseelement.h  ../src/element.h
SOURCES += ../src/baseelement.cpp ../src/element.cpp
SOURCES += baseelement_test.cpp element_test.cpp test-main.cpp
