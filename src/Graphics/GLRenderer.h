#pragma once

extern float screenWidth;
extern float screenHeight;

void init(float width, float height);

void drawPoint(float x, float y, float pointSize = 1);
void drawVertex(float x, float y);
void drawLine(float x0, float y0, float x1, float y1, float lineThickness = 1);
void drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2);
void drawRect(float x, float y, float x2, float y2, float borderWidth = 0);

void setDrawColor(int r, int g, int b);
void setPointSize(float s);

void startDrawingPoints();
void stopDrawing();