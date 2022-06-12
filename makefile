TARGET	:= theoLISP

MAIN	:= main.cpp
HEADERS	:= $(wildcard *.hpp)

MAKEFILE := makefile

CXX			:= g++
CXXFLAGS	:= -Wall -Wextra -Wpedantic -Werror -std=c++1z -O3 -march=native

$(TARGET): $(MAIN) $(HEADERS) $(MAKEFILE)
	$(CXX) $(CXXFLAGS) $(MAIN) -o $(TARGET)

.PHONY: clean
clean:
	$(RM) -rf $(TARGET) $(TARGET).exe