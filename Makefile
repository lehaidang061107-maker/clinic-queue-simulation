CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
CPPFLAGS = -Iinclude
LEADER = src/Patient.cpp src/SimulationConfig.cpp src/Simulation.cpp
DEPENDENCIES = src/PatientStore.cpp src/LinkedQueue.cpp src/PriorityHeap.cpp src/ClinicRoom.cpp src/Statistics.cpp
HEADERS = $(wildcard include/*.h)

# macOS ld64 dung -dead_strip; GNU ld/MinGW dung --gc-sections.
ifeq ($(shell uname -s 2>/dev/null),Darwin)
GC_SECTIONS = -Wl,-dead_strip
else
GC_SECTIONS = -Wl,--gc-sections
endif

.PHONY: all test-a test-a-queue test-b test-c test-leader test-leader-isolated check-leader
all: build/clinic_simulator

build:
	mkdir -p build

build/clinic_simulator: $(wildcard src/*.cpp) $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(wildcard src/*.cpp) -o $@

# A: full test needs real PriorityHeap (B). test-a-queue only checks LinkedQueue.
test-a: build/test_a
	./build/test_a

build/test_a: tests/test_a.cpp src/LinkedQueue.cpp src/ClinicRoom.cpp src/PriorityHeap.cpp src/Patient.cpp $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_a.cpp src/LinkedQueue.cpp src/ClinicRoom.cpp src/PriorityHeap.cpp src/Patient.cpp -o $@

test-a-queue: build/test_a_queue
	./build/test_a_queue

build/test_a_queue: tests/test_a.cpp src/LinkedQueue.cpp src/Patient.cpp $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DTEST_A_QUEUE_ONLY tests/test_a.cpp src/LinkedQueue.cpp src/Patient.cpp -o $@

# B: PriorityHeap, Statistics and Merge Sort report (needs real PatientStore of C).
test-b: build/test_b
	./build/test_b

build/test_b: tests/test_b.cpp src/PriorityHeap.cpp src/Statistics.cpp src/PatientStore.cpp src/Patient.cpp $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_b.cpp src/PriorityHeap.cpp src/Statistics.cpp src/PatientStore.cpp src/Patient.cpp -o $@

# Isolated C tests discard the unreferenced run-results writer, which depends on B.
test-c: build/test_c
	./build/test_c

build/test_c: tests/test_c.cpp src/PatientStore.cpp src/FileIO.cpp src/DataGenerator.cpp src/Patient.cpp src/SimulationConfig.cpp $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -ffunction-sections -fdata-sections tests/test_c.cpp src/PatientStore.cpp src/FileIO.cpp src/DataGenerator.cpp src/Patient.cpp src/SimulationConfig.cpp $(GC_SECTIONS) -o $@

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
