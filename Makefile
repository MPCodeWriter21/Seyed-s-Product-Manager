build: build/sqlite3.obj src/*.cpp src/*.h
	clang++ src/*.cpp build/sqlite3.obj -o build/out.o -std=c++20

run: build
	./build/debug/out.o

build_debug: build/debug/sqlite3.obj build/debug/out.o

run_debug: build_debug
	./build/debug/out.o

clean:
	rm -rf build

build/debug:
	mkdir -p build/debug

build/sqlite3.obj: src/sqlite/*.h src/sqlite/*.c
	mkdir -p build
	clang src/sqlite/*.c -o build/sqlite3.obj -c

build/debug/sqlite3.obj: src/sqlite/*.h src/sqlite/*.c
	mkdir -p build/debug
	clang --debug src/sqlite/*.c -o build/debug/sqlite3.obj -c
	
build/debug/out.o: build/debug build/debug/sqlite3.obj src/*.cpp src/*.h
	clang++ --debug src/*.cpp build/debug/sqlite3.obj -o build/debug/out.o -std=c++20
