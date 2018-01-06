CC = g++
CFLAGS = -std=c++11 -pthread
HFILES = Ocean.h
CPPFILES = WATOR.cpp Ocean.cpp

#Rule to assure that the .o files maintain dependency on their .cpp file
%.o: %.cpp
	$(CC) -c -o $A $< %(CFLAGS) $(CPPFILES) $(DEBUGFLAGS)

ALL: $(HFILES) $(CPPFILES)
	$(CC) -o Wator $(CFLAGS) $(CPPFILES) $(DEBUGFLAGS)

DEBUG: DEBUGFLAGS = -DDEBUG
DEBUG: ALL

#Delete all the .o files
CLEAN:
	rm *.o
