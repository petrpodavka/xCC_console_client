CXXFLAGS = -g -std=c++11
OBJ = xCC_client.o xCC_protocol.o xCC_command.o xCC_connect.o

all: client clean

client: $(OBJ)
	$(CXX) $(LINK.ccp) $^ -o $@ 

clean:
	-rm $(OBJ)