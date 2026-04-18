# AeroSync Makefile

CXX = g++
CXXFLAGS = -std=c++17 -I ./include

all: atc_server aircraft_client

atc_server: src/server.cpp
	$(CXX) $(CXXFLAGS) src/server.cpp -o atc_server

aircraft_client: src/client.cpp
	$(CXX) $(CXXFLAGS) src/client.cpp -o aircraft_client

# Unit Tests
unit_test: tests/unit_tests.cpp
	$(CXX) $(CXXFLAGS) tests/unit_tests.cpp -o tests/run_units
	./tests/run_units

# System Test (builds and runs full system)
system_test: all
	chmod +x tests/system_test.sh
	./tests/system_test.sh

# Integration Test (requires logs from a previous run)
integration_test:
	chmod +x tests/integration_test.sh
	./tests/integration_test.sh

# Usability Test
usability_test: all
	chmod +x tests/usability_test.sh
	./tests/usability_test.sh

# Run all tests
test: unit_test system_test integration_test usability_test

# Code coverage
coverage:
	@mkdir -p tests/tool_outputs
	$(CXX) $(CXXFLAGS) --coverage tests/unit_tests.cpp -o tests/run_units_cov
	./tests/run_units_cov
	cd tests && xcrun llvm-cov gcov -b run_units_cov-unit_tests.gcda 2>&1 | grep -A1 "^File 'tests/" | tee tool_outputs/coverage_report.txt
	@rm -f tests/run_units_cov tests/*.gcov tests/*.gcda tests/*.gcno

# Static analysis (DO-178C compliance evidence)
static_analysis:
	@mkdir -p tests/tool_outputs
	cppcheck --enable=all --std=c++17 -I ./include --suppress=missingIncludeSystem src/ include/ 2>&1 | tee tests/tool_outputs/cppcheck_report.txt

clean:
	rm -f atc_server aircraft_client
	rm -f tests/run_units
	rm -f telemetry_data.bin
