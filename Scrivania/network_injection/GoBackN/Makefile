# Author : Enrico enrico.
# Date   : 10 November 2014.
# Note   :
# In order to run this Makefile the following environment variables must be set:
#     SCNSL_INC   - .../scnsl/include
#     SCNSL_LIB   - .../scnsl/lib
#     SYSTEMC_INC - .../systemc/include
#     SYSTEMC_LIB - .../systemc/lib

CXX        = g++
LD         = g++
RM         = rm -rf
EXECUTABLE = main

#SYSTEMC_INC = /usr/local/systemc231/include
#SCNSL_INC = /usr/local/scnsl/include
#SYSTEMC_LIB = /usr/local/systemc231/lib-linux64
#SCNSL_LIB = /usr/local/scnsl/lib

INCS = -I ${SYSTEMC_INC} -I ${SCNSL_INC}
LIBS = -L ${SYSTEMC_LIB} -L ${SCNSL_LIB} -lsystemc -lscnsl -ldl

CXXFLAGS   = -c $(INCS) -std=c++11 -Wall -w
LDFLAGS    =
SOURCES    = main.cc LocalTask.cc CentralTask.cc RemoteTask.cc
OBJECTS    = $(SOURCES:.cc=.o)

all: $(SOURCES) $(EXECUTABLE)
   
$(EXECUTABLE): $(OBJECTS) 
	@echo Linking $@
	@$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

clean:
	@echo "Cleaning..."
	@rm -rf $(EXECUTABLE)
	@rm -rf *.o

.PHONY: all clean
