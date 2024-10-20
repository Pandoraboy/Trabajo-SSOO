CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = trabajo1
OBJ = trabajo1.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)
