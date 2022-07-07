## Compilation
```
/g++ mandelbrot.cpp -lglut -lGL -pthread -lX11
```
## Usage
```
./a.out xmin xmax ymin ymax max_iterations number_of_threads
```
or
```
./a.out
```
to run with default parameters
```
xmin = -2.0;
xmax = 1.0;
ymin = -1.0;
ymax = 1.0;
max_iterations = 100;
number_of_threads = 1;
```