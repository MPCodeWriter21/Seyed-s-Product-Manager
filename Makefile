build:
	mkdir -p bin
	clang src/sqlite/sqlite3.c -o bin/sqlite3.obj -c
	clang++ src/*.cpp bin/sqlite3.obj -o bin/out.o -std=c++20

run: build
	./bin/out.o

build_debug: clean
	mkdir -p bin/debug
	clang --debug src/sqlite/sqlite3.c -o bin/debug/sqlite3.obj -c
	clang++ --debug src/*.cpp bin/debug/sqlite3.obj -o bin/debug/out.o -std=c++20

run_debug: build_debug
	./bin/debug/out.o

clean:
	rm -rf bin
