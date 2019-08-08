  .SILENT:       
CXX	= g++
CXXFLAGS= -std=c++17 -lstdc++fs -Wall -Wextra -pedantic
TARGET	= test_bin
FILES	= element_test.cpp \
	  ../src/element/element.cpp \
	  ../src/element/baseelement.cpp

all:
	$(CXX) $(CXXFLAGS) $(FILES) -o $(TARGET) && ./$(TARGET) 
	rm -f $(TARGET)
element:
	$(CXX) $(CXXFLAGS) $(FILES) -o $(TARGET) && ./$(TARGET) "[Element]"
baseelement:
	$(CXX) $(CXXFLAGS) $(FILES) -o $(TARGET) && ./$(TARGET) "[baseelement]"
clean:
	rm -f $(TARGET) "_new md file_.md"
