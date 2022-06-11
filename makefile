MAIN	:= main.cpp
TARGET	:= theoLISP

CXX			:= g++
CXXFLAGS	:= -Wall -Wextra -Wpedantic -Werror -std=c++1z -O3 -march=native

$(TARGET): $(MAIN)
	$(CXX) $(CXXFLAGS) $(MAIN) -o $(TARGET)

.PHONY: clean
clean:
	$(RM) -rf $(TARGET) $(TARGET).exe