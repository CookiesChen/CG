#ifndef BRESENHAM_TRIANGLE
#define BRESENHAM_TRIANGLE

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Shader.h"
#include <math.h>

using namespace std;

class Bresenham
{
public:
	Bresenham();
	~Bresenham();

	// 三角形
	void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
	void generateLine(int x1, int y1, int x2, int y2);

	// 填充三角形
	void padding(int x1, int y1, int x2, int y2, int x3, int y3);
	int getMax(int n1, int n2, int n3);
	int getMin(int n1, int n2, int n3);
	bool checkSameSide(int a, int b, int c, int x, int y, int p1, int p2);

	// 圆形
	void circle(int center_X, int center_Y, int radius);
	void addPoint(int x, int y, float*);

private:
	int precision = 100;
	int steps = 0;
};

#endif // !BRESENHAM_TRIANGLE

