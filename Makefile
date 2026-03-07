CXX = g++ # compiler
CXXFLAGS = -O2 -std=c++20 # -O2 enables safe performance optimisations that do not modify program behaviour

TARGET = build # make will produce an executable called build
SRC = $(wildcard src/*.cpp) # c++ source files
OBJ = $(SRC:.cpp=.o) # list of object files produced by source files. syntax just takes files under SRC and swaps file extensions

# all = default target. it depends on $(TARGET) which depends on src files
all: $(TARGET)

# from updated object files, link + build final executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

# first regenerate only the object files necessary (reduces build times)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# running make clean will remove previous builds + object files
clean:
	rm -f $(TARGET) $(OBJ)
