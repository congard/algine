g++ src/main.cpp -o out/main.o `pkg-config --libs glfw3 glew` -pthread;
./out/main.o;