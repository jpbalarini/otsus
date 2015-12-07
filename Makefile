# Makefile created with Create Makefile.sh 24/12/2011

COPT      = -O3
CFLAGS    = $(COPT)
CSTRICT   = -Wall -Wextra -ansi
CXXOPT    = -O3
CXXFLAGS  = $(CXXOPT)
CXXSTRICT = -Wall -Wextra -ansi
LDFLAGS   = -lm -lpng
EXEC      = otsus

default: $(EXEC)
all: $(EXEC)

# ------- C files -------
# ------- C++ files -------
./framework/CFramework.o: ./framework/CFramework.cpp ./framework/CFramework.h
	$(CXX) $(CXXFLAGS) -c ./framework/CFramework.cpp -o ./framework/CFramework.o

./framework/libparser.o: ./framework/libparser.cpp ./framework/libparser.h
	$(CXX) $(CXXFLAGS) -c ./framework/libparser.cpp -o ./framework/libparser.o

./framework/CHistogram.o: ./framework/CHistogram.cpp
	$(CXX) $(CXXFLAGS) -c ./framework/CHistogram.cpp -o ./framework/CHistogram.o

./framework/CImage.o: ./framework/CImage.cpp ./framework/CImage.h
	$(CXX) $(CXXFLAGS) -c ./framework/CImage.cpp -o ./framework/CImage.o

./framework/CInspector.o: ./framework/CInspector.cpp ./framework/CInspector.h
	$(CXX) $(CXXFLAGS) -c ./framework/CInspector.cpp -o ./framework/CInspector.o

./framework/framework.o: ./framework/framework.cpp
	$(CXX) $(CXXFLAGS) -c ./framework/framework.cpp -o ./framework/framework.o

./framework/operations.o: ./framework/operations.cpp
	$(CXX) $(CXXFLAGS) -c ./framework/operations.cpp -o ./framework/operations.o

./algo.o: ./algo.cpp ./algo.h
	$(CXX) $(CXXFLAGS) -c ./algo.cpp -o ./algo.o

# ------- Main -------
$(EXEC):  ./framework/CFramework.o ./framework/libparser.o ./framework/CHistogram.o ./framework/CImage.o ./framework/CInspector.o ./framework/framework.o ./framework/operations.o ./algo.o
	$(CXX)  ./framework/CFramework.o ./framework/libparser.o ./framework/CHistogram.o ./framework/CImage.o ./framework/CInspector.o ./framework/framework.o ./framework/operations.o ./algo.o $(LDFLAGS) -o $(EXEC)

lint:
	$(MAKE) CFLAGS="$(CFLAGS) $(CSTRICT)" CXXFLAGS="$(CXXFLAGS) $(CXXSTRICT)"

clean:
	rm -f *.o
	rm -f ./framework/*.o
	rm ./otsus

distclean: clean
	rm -f $(EXEC)

