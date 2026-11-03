CXX ?= g++
PYTHON ?= python3
CXXFLAGS ?= -std=c++20 -O2 -g0 -Wall -Wextra -Wpedantic -fno-tree-vectorize -fno-unroll-loops -fno-omit-frame-pointer -fno-lto -fno-pie
LDFLAGS ?= -no-pie

.PHONY: all check handout clean

all: bin/telemetry

bin/telemetry: src/main.cpp src/engine.cpp src/engine.hpp Makefile
	mkdir -p bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/main.cpp src/engine.cpp $(LDFLAGS) $(LDLIBS) -o $@

check: bin/telemetry
	$(PYTHON) instructor/check.py --binary ./bin/telemetry --cxx "$(CXX)"

handout: bin/telemetry
	mkdir -p dist
	tar -czf dist/lab5.tar.gz --transform='s,^,lab5/,' README.md Makefile src examples bin/telemetry

clean:
	$(RM) bin/telemetry dist/lab5.tar.gz
