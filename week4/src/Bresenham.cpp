#include "Bresenham.h"

Bresenham::Bresenham()
{

}


Bresenham::~Bresenham()
{
}

void Bresenham::triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	generateLine(x1, y1, x2, y2);
	generateLine(x1, y1, x3, y3);
	generateLine(x2, y2, x3, y3);
}

void Bresenham::generateLine(int x1, int y1, int x2, int y2)
{
	unsigned int VAO, VBO;
	int delta_X = abs(x1 - x2);
	int delta_Y = abs(y1 - y2);

	// 斜率大于1调整x, y
	bool flag = delta_Y > delta_X;
	if (flag) {
		int temp = delta_X;
		delta_X = delta_Y;
		delta_Y = temp;
		temp = x1;
		x1 = y1;
		y1 = temp;
		temp = x2;
		x2 = y2;
		y2 = temp;
	}

	int x = x1 <= x2 ? x1 : x2;
	int y = x1 <= x2 ? y1 : y2;

	bool positive = (x1 - x2) * (y1 - y2) >= 0;

	float vertices[1000];
	// 初始点
	int steps = delta_X + 1;
	int p = 2 * delta_Y - delta_X;
	for (int i = 0; i < steps; i++) {

		vertices[3 * i] = (flag ? (float)y : (float)(x + i)) / precision;
		vertices[3 * i + 1] = (flag ? (float)(x + i) : (float)y) / precision;
		vertices[3 * i + 2] = 0.0f;

		if (p > 0) {
			y = positive ? y + 1 : y - 1;
			p = p + 2 * delta_Y - 2 * delta_X;
		}
		else {
			p = p + 2 * delta_Y;
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, steps);

	// 清除
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Bresenham::padding(int x1, int y1, int x2, int y2, int x3, int y3)
{
	int max_x = getMax(x1, x2, x3);
	int max_y = getMax(y1, y2, y3);
	int min_x = getMin(x1, x2, x3);
	int min_y = getMin(y1, y2, y3);


	float vertices[100000];
	int pointNum = 0;
	// 直线方程
	int a1 = y2 - y1, b1 = -(x2 - x1), c1 = -b1 * y1 - a1 * x1;
	int a2 = y3 - y2, b2 = -(x3 - x2), c2 = -b2 * y2 - a2 * x2;
	int a3 = y3 - y1, b3 = -(x3 - x1), c3 = -b3 * y1 - a3 * x1;

	for (int i = min_x; i <= max_x; i++) {
		for (int j = min_y; j <= max_y; j++) {
			if (checkSameSide(a1, b1, c1, i, j, x3, y3) &&
				checkSameSide(a2, b2, c2, i, j, x1, y1) &&
				checkSameSide(a3, b3, c3, i, j, x2, y2)) {

				vertices[3 * pointNum] = (float)i / precision;
				vertices[3 * pointNum + 1] = (float)j / precision;
				vertices[3 * pointNum + 2] = 0.0f;
				pointNum++;
			}
		}
	}

	unsigned int VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, pointNum);

	// 清除
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

}

int Bresenham::getMax(int n1, int n2, int n3)
{
	int max = n1 > n2 ? n1 : n2;
	max = max > n3 ? max : n3;
	return max;
}

int Bresenham::getMin(int n1, int n2, int n3)
{
	int min = n1 < n2 ? n1 : n2;
	min = min < n3 ? min : n3;
	return min;
}

bool Bresenham::checkSameSide(int a, int b, int c, int x, int y, int p1, int p2)
{
	return a*x + b*y + c >= 0 && a * p1 + b * p2 + c >= 0 ||
		a * x + b * y + c <= 0 && a * p1 + b * p2 + c <= 0;
}

void Bresenham::circle(int center_X, int center_Y, int radius)
{
	// 起点
	int x = 0;
	int y = radius;

	int p = 3 - 2 * radius;
	float vertices[3000];
	steps = 0;
	for (; x <= y; x++) {

		// 八分之一圆
		addPoint(x, y, vertices);
		// 对称性求其余圆
		addPoint(-x, y, vertices);
		addPoint(x, -y, vertices);
		addPoint(-x, -y, vertices);
		addPoint(y, x, vertices);
		addPoint(-y, x, vertices);
		addPoint(y, -x, vertices);
		addPoint(-y, -x, vertices);

		if (p < 0) {
			p += 4 * x  + 6;
		} else {
			p += 4 * (x- y) + 10;
			y--;
		}
	}
	unsigned int VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, steps);

	// 清除
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Bresenham::addPoint(int x, int y, float* vertices)
{
	vertices[3 * steps] = (float)x / precision;
	vertices[3 * steps + 1] = (float)y / precision;
	vertices[3 * steps + 2] = 0.0f;
	steps++;
}
