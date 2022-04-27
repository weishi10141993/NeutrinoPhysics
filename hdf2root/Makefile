BASEDIR  = $PWD

TARGET   = hdf2root

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)

HDF5CFLAGS    = -I$(HDF5)/include
HDF5LIBS      = -L$(HDF5)/lib -lhdf5_cpp

OMPICFLAGS    = $(shell mpif77 --showme:compile)
OMPILIBS      = $(shell mpif77 --showme:link)

# c++-compiler
CXX           = g++
CXXFLAGS      = -g -O2 -Wall $(ROOTCFLAGS) $(HDF5CFLAGS) $(OMPICFLAGS)

LIBS          = $(ROOTLIBS) $(HDF5LIBS) $(OMPILIBS)

all: bin

bin: $(TARGET)

$(TARGET): $(TARGET).o
	@echo "Linking $@"
	@$(CXX) $(LDFLAGS) -o $@ $(TARGET).o $(LIBS) 

%.o: %.cpp
	@echo "Compiling $<"
	@$(CXX) -c $(CXXFLAGS) $<

clean:
	@rm -f $(TARGET)
	@rm -f *.o
	@rm -f *~ *.d

