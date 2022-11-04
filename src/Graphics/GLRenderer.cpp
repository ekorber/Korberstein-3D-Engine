#include "GLRenderer.h"
#include <GLFW/glfw3.h>

float screenWidth = 0;
float screenHeight = 0;

//Used to avoid recurring division operations
float inverseScreenWidth = 0;
float inverseScreenHeight = 0;

/* Initialize inverse values. Call at start, and any time screen is resized. */
void init(float width, float height) {
	screenWidth = width;
	screenHeight = height;
	inverseScreenWidth = 1 / screenWidth;
	inverseScreenHeight = 1 / screenHeight;
}

void drawPoint(float x, float y, float pointSize) {
	glPointSize(pointSize);
	glBegin(GL_POINTS);
	drawVertex(x, y);
	glEnd();
}

/* A function to draw vertices in pixel screen space, given inputted x, y coordinates. */
void drawVertex(float x, float y) {
	x = 2 * x * inverseScreenWidth - 1;
	y = 2 * (1 - (y * inverseScreenHeight)) - 1;

	glVertex2f(x, y);
}

void drawLine(float x0, float y0, float x1, float y1, float lineThickness) {
	glLineWidth(lineThickness);
	glBegin(GL_LINES);
	drawVertex(x0, y0);
	drawVertex(x1, y1);
	glEnd();
}

void drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
	drawVertex(x0, y0);
	drawVertex(x1, y1);
	drawVertex(x2, y2);
}

void drawRect(float x0, float y0, float x1, float y1, float borderWidth) {

	if (borderWidth > 0) {
		x0 += borderWidth;
		y0 += borderWidth;
		x1 -= borderWidth;
		y1 -= borderWidth;
	}

	//Draw rect by drawing 2 triangles
	glBegin(GL_TRIANGLES);
	drawTriangle(x0, y0, x0, y1, x1, y0);
	drawTriangle(x0, y1, x1, y0, x1, y1);
	glEnd();

}

void setDrawColor(int r, int g, int b) {
	glColor3ub(r, g, b);
}

void setPointSize(float s) {
	glPointSize(s);
}

void startDrawingPoints() {
	glBegin(GL_POINTS);
}

void stopDrawing() {
	glEnd();
}