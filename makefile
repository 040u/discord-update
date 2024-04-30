CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = discord-update
INSTALL_DIR = /usr/bin
SRCS = main.cpp

all:$(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) -lcurl

install: $(TARGET)
	cp $(TARGET) $(INSTALL_DIR)

clean:
	rm -f $(TARGET)