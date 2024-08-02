CC	= g++ -std=c++11
CPPFLAGS= -Wno-deprecated -O3 -c
LDFLAGS	= -O3 
SOURCES	= Main.cpp Basic.cpp Graph.cpp BasicMethod.cpp TopoLevel.cpp Index.cpp genIndexMethods.cpp queryMethods.cpp geneQuery.cpp transf.cpp
OBJECTS	= $(SOURCES:.cpp=.o)
EXECUTABLE=BL

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o : 
	$(CC) $(CPPFLAGS) $< -o $@

.PHONY : clean

clean:
	rm *.o $(EXECUTABLE)
	


