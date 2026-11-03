CXX ?= g++
PYTHON ?= python3
CXXFLAGS ?= -std=c++17 -g3 -O0 -fno-omit-frame-pointer -Wall -Wextra -Wpedantic

.PHONY: all check handout clean

all: converter

converter: src/main.cpp src/converter.cpp src/converter.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/main.cpp src/converter.cpp $(LDFLAGS) $(LDLIBS) -o $@

check: converter
	$(PYTHON) instructor/check.py --binary ./converter --cxx "$(CXX)"

handout: converter
	mkdir -p dist
	tar -czf dist/lab1.tar.gz --transform='s,^,lab1/,' README.md converter src examples

clean:
	$(RM) converter dist/lab1.tar.gz
