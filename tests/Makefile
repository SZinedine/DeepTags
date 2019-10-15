.SILENT:       
CXX	= g++
CXXFLAGS= -std=c++17 -lstdc++fs -Wall -Wextra -pedantic
TARGET	= test
OBJS	= element_test.o element.o baseelement.o test-main.o

.PHONY: all
all: $(TARGET)
	./$(TARGET)
	$(MAKE) shallowclean


$(TARGET): element_test.o test-main.o element.o baseelement.o 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

element.o: ../src/element/element.cpp
	$(CXX) $(CXXFLAGS) -c ../src/element/element.cpp

baseelement.o: ../src/element/baseelement.cpp
	$(CXX) $(CXXFLAGS) -c ../src/element/baseelement.cpp

test-main.o: test-main.cpp
	$(CXX) $(CXXFLAGS) -c test-main.cpp

element_test.o: element_test.cpp
	$(CXX) $(CXXFLAGS) -c element_test.cpp

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) "_new md file_.md"

.PHONY: shallowclean
shallowclean: 
	rm -f $(TARGET) "_new md file_.md"

