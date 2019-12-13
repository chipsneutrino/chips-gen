CXX = g++
CXXFLAGS += -I$(CRYDIR)/src -L$(CRYDIR)/lib

default: cosmicGen

cosmicGen: cosmicGen.cc # Rule to make cosmicGen executable
	$(CXX) $(CXXFLAGS) -o cosmicGen cosmicGen.cc -lCRY

clean:
	rm -f cosmicGen