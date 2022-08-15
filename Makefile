

# image : ./main.html
# 	~/emsdk/upstream/emscripten/emrun ./main.html

# main.html : main.cpp
# 	~/emsdk/upstream/emscripten/em++ -std=c++2a -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 main.cpp -o main.html --emrun

image : ./main
	./main > image.ppm; display image.ppm

main: main.cpp
	g++-10 -O2 -std=c++2a main.cpp -o main

clean:
	rm *.o *.js *.html *.wasm main image out image.ppm
	