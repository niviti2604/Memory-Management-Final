CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRCS = main.cpp memory.cpp stats.cpp first_fit.cpp best_fit.cpp worst_fit.cpp buddy.cpp cache.cpp 

OBJS = $(SRCS:.cpp=.o)

TARGET = simulator.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo Cleaning project...
	del /f /q *.o $(TARGET) 2>nul || exit 0