CXX = g++
#CXXFLAGS = -Wall -std=c++11 -I.
LDFLAGS = -lcrypto

TARGET = main
SRCS = main.cpp AES.cpp pch.cpp RSA.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
