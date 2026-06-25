CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = crypto
SOURCES = main.cpp vigenere.cpp atbash.cpp xor_cipher.cpp
HEADERS = vigenere.h atbash.h xor_cipher.h
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run