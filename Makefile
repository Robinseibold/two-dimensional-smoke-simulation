CC = g++
CPPFLAGS = -Wall -std=c++11 -g -c -o $@
LDFLAGS = -framework Cocoa -framework openGL -framework SDL2 -o $@

DEPENDENCIES = src/Main.o src/Fluid.o

main: $(DEPENDENCIES)
	$(CC) $(LDFLAGS) $(DEPENDENCIES)

main.o: src/Main.cc 
	$(CC) $(CPPFLAGS) src/Main.cc

Fluid.o: src/Fluid.h src/Fluid.cc
	$(CC) $(CPPFLAGS) src/Fluid.cc

clean:
	-rm main $(DEPENDENCIES)

run:
	./main
