build:
	mkdir -p bin
	clang++ src/*.cpp -o bin/out.o -std=c++20

run: build
	./bin/out.o

build_debug: clean
	mkdir -p bin/debug
	clang++ --debug src/*.cpp -o bin/debug/out.o -std=c++20

run_debug: build_debug
	./bin/debug/out.o

clean:
	rm -rf bin
