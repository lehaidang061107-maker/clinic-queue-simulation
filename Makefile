CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
CPPFLAGS = -Iinclude
LEADER = src/Patient.cpp src/SimulationConfig.cpp src/Simulation.cpp
DEPENDENCIES = src/PatientStore.cpp src/LinkedQueue.cpp src/PriorityHeap.cpp src/ClinicRoom.cpp src/Statistics.cpp
HEADERS = $(wildcard include/*.h)

.PHONY: all test-c test-leader test-leader-isolated check-leader
all: build/clinic_simulator

build:
	mkdir -p build

build/clinic_simulator: $(wildcard src/*.cpp) $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(wildcard src/*.cpp) -o $@

# Isolated C tests discard the unreferenced run-results writer, which depends on B.
test-c: build/test_c
	./build/test_c

build/test_c: tests/test_c.cpp src/PatientStore.cpp src/FileIO.cpp src/DataGenerator.cpp src/Patient.cpp src/SimulationConfig.cpp $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -ffunction-sections -fdata-sections tests/test_c.cpp src/PatientStore.cpp src/FileIO.cpp src/DataGenerator.cpp src/Patient.cpp src/SimulationConfig.cpp -Wl,--gc-sections -o $@

# Requires the real A/B/C modules. This target never uses test doubles.
test-leader: build/test_leader
	./build/test_leader

build/test_leader: tests/test_leader.cpp $(LEADER) $(DEPENDENCIES) $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_leader.cpp $(LEADER) $(DEPENDENCIES) -o $@

# Explicit isolated verification while A/B/C are not implemented.
test-leader-isolated: build/test_leader_isolated
	./build/test_leader_isolated

build/test_leader_isolated: tests/test_leader.cpp tests/support/leader_dependencies.cpp $(LEADER) $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DLEADER_ISOLATED_TEST tests/test_leader.cpp tests/support/leader_dependencies.cpp $(LEADER) -o $@

check-leader:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fsyntax-only $(LEADER) src/main.cpp
