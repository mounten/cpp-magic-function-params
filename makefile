all: 
	$(CXX) -std=c++20 src/main.cpp -o cpp_magic_fn

clean:
	rm -f cpp_magic_fn
