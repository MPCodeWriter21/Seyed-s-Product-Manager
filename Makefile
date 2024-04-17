build: clean
	mkdir -p build
	clang++ src/*.cpp -o build/out.o -std=c++20

clean:
	rm -rf build

run: build
	./build/out.o
