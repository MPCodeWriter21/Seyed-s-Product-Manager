build:
	mkdir -p build
	clang++ src/*.cpp -o build/out.o

clean:
	rm -rf build

run: build
	./build/out.o
