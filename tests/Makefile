.SILENT:       
CPP	 = g++
CPPFLAGS = -std=c++17 -lstdc++fs -Wall -Wextra -pedantic
TARGET	 = test
ELEM_DIR = ../src/element
OBJS	 = test-main.o baseelement_test.o element_test.o element.o baseelement.o 



.PHONY: all
all: $(TARGET) exec 

$(TARGET): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(TARGET)

exec: 
	./$(TARGET)



.PHONY: element
element: test-main.o element_test.o element.o baseelement.o 
	$(CPP) $(CPPFLAGS) test-main.o element_test.o element.o baseelement.o  -o $(TARGET)

.PHONY: baseelement 
baseelement: test-main.o baseelement_test.o baseelement.o 
	$(CPP) $(CPPFLAGS) test-main.o baseelement_test.o baseelement.o -o $(TARGET)



test-main.o: test-main.cpp
	$(CPP) -c $(CPPFLAGS) test-main.cpp

baseelement.o: $(ELEM_DIR)/baseelement.cpp
	$(CPP) -c $(CPPFLAGS) $(ELEM_DIR)/baseelement.cpp

element.o: $(ELEM_DIR)/element.cpp baseelement.o
	$(CPP) -c $(CPPFLAGS) $(ELEM_DIR)/element.cpp

element_test.o: element_test.cpp element.o test-main.o
	$(CPP) -c $(CPPFLAGS) element_test.cpp

baseelement_test.o: baseelement_test.cpp baseelement.o test-main.o
	$(CPP) -c $(CPPFLAGS) baseelement_test.cpp



.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) "_new md file_.md"

