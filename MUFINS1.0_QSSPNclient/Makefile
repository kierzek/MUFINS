CC := gcc
CXX := g++

# -MMD flag turns on dependency files generation.
# Those .d files are used by make to automatically
# rebuild on header file changes.
CFLAGS := -MMD -O2
CXXFLAGS := -MMD -std=c++0x -O2 -Isrc/asio/include -Isrc/asio
LD_FLAGS := -lm -pthread


CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
GLPK_C_FILES := $(wildcard src/glpk/*.c)
GLPK_OBJ_FILES := $(addprefix obj/glpk/,$(notdir $(GLPK_C_FILES:.c=.o)))

all: directories install

.PHONY: directories
directories:
	mkdir -p obj/glpk

install: bin/qsspn

obj/glpk/%.o: src/glpk/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

bin/qsspn: $(OBJ_FILES) $(GLPK_OBJ_FILES)
	$(CXX) -o $@ $^ $(LD_FLAGS)

clean:
	rm -rf obj/*.o obj/*.d bin/qsspn

veryclean:
	rm -rf obj/*.o obj/*.d obj/glpk/* bin/qsspn

-include $(OBJ_FILES:.o=.d)
