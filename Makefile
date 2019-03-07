CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		ntripcpp.o

LIBS =

TARGET =	ntripcpp

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
