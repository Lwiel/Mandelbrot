#include <GL/glut.h>
#include <X11/Xlib.h>
#include <iostream>
#include <complex>
#include <vector>
#include <thread> 
#include <ctime>
#include <chrono>
#include <mutex>

//g++ mandelbrot.cpp -lglut -lGL -pthread -lX11
//./a.out -2.0 1.0 -1.0 1.0 100 2

double xmin;
double xmax;
double ymin;
double ymax;
int n;
int nthreads;

struct Point{
	int x;
	int y;
	double brightness;
	
	Point(int xval, int yval, double bval){
		x = xval;
		y = yval;
		brightness = bval;
	}
};

std::vector<Point> points;
std::mutex mymutex;

double check(std::complex<double> p){
	std::complex<double> z(0.0, 0.0);

	for(int i = 0;i<=n;i++){
		if(abs(z) >= 2.0){
			if(i>n*0.2) return (double) i/n;
			else return 0;
		}
		z = z*z+p;
	}
	return 1;
}

void doJob(int tid){
	const int width = glutGet(GLUT_WINDOW_WIDTH);
    const int height = glutGet(GLUT_WINDOW_HEIGHT);
	double re, im, brightness;
	std::vector<Point> subpoints;
	for(int y = tid;y<height;y+=nthreads){
		for(int x = 0;x<width;x++){
			re = (double) x*(xmax-xmin)/(width-1) + xmin;
			im = (double) y*(ymax-ymin)/(height-1) + ymin;
			std::complex<double> p(re, im);
			brightness = check(p);
			subpoints.push_back(Point(x, y, brightness));
		}
	}
	mymutex.lock();
    points.insert(points.end(), subpoints.begin(), subpoints.end());
    mymutex.unlock();
}

void calculatePoints(){
	const int width = glutGet(GLUT_WINDOW_WIDTH);
    const int height = glutGet(GLUT_WINDOW_HEIGHT);
	double brightness, re, im;
	std::thread threads[nthreads];

	auto start = std::chrono::system_clock::now();
	for(int i = 0;i<nthreads;i++){
        threads[i] = std::thread(doJob, i);
    }
	for(int i = 0;i<nthreads;i++){
        threads[i].join();
    }
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsedTime = end-start;
	std::cout<<"Number of threads: "<<nthreads<<", Execution time: "<<elapsedTime.count()<<"s"<<std::endl;
}

void MyKeyboardFunc(unsigned char Key, int x, int y){
	double width = xmax-xmin;
	double height = ymax-ymin;
	switch(Key){
		case 'a':
			xmin-=width*0.1;
			xmax-=width*0.1;
		break;
		case 'd':
			xmin+=width*0.1;
			xmax+=width*0.1;
		break;
		case 'w':
			ymax+=height*0.1;
			ymin+=height*0.1;
		break;
		case 's':
			ymax-=height*0.1;
			ymin-=height*0.1;
		break;
		case 'q':
			ymin*=0.8;
			ymax*=0.8;
			xmin*=0.8;
			xmax*=0.8;
		break;
		case 'e':
			ymin*=1.25;
			ymax*=1.25;
			xmin*=1.25;
			xmax*=1.25;
		break;
	}
	glutPostRedisplay();
}

void display(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	const int width = glutGet(GLUT_WINDOW_WIDTH);
    const int height = glutGet(GLUT_WINDOW_HEIGHT);
	glOrtho(0, width, 0, height, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	calculatePoints();
	glBegin(GL_POINTS);
	for(int i = 0;i<points.size();i++){
		glColor3f(points[i].brightness, 0.0, 0.0);
		glVertex2i(points[i].x, points[i].y);
	}
	glEnd();
	glutSwapBuffers();
}

int main(int argc, char* argv[]){
	XInitThreads();
    glutInit(&argc, argv);
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	
	if(argc==1){
		xmin = -2.0;
		xmax = 1.0;
		ymin = -1.0;
		ymax = 1.0;
		n = 100;
		nthreads = 1;
	}
	else if(argc==7){
		xmin = atof(argv[1]);
		xmax = atof(argv[2]);
		ymin = atof(argv[3]);
		ymax = atof(argv[4]);
		n = atoi(argv[5]);
		nthreads = atoi(argv[6]);
	}
	else{
		std::cout<<"Incorrect number of arguments"<<std::endl;
		std::cout<<"Syntax: xmin xmax ymin ymax n nthreads (or no arguments)"<<std::endl;
		return 0;
	}

	glutInitWindowPosition(80, 80);
  	glutInitWindowSize(640, 480);
  	glutCreateWindow("Mandelbrot");

    glutDisplayFunc(display);
	glutKeyboardFunc(MyKeyboardFunc);
	glutMainLoop();

	return 0;
}