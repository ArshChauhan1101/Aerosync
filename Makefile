# AeroSync Makefile

CXX = g++
CXXFLAGS = -std=c++17 -I ./include

all: atc_server aircraft_client

atc_server: src/server.cpp
	$(CXX) $(CXXFLAGS) src/server.cpp -o atc_server

aircraft_client: src/client.cpp
	$(CXX) $(CXXFLAGS) src/client.cpp -o aircraft_client

clean:
	rm -f atc_server aircraft_client