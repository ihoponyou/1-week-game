#pragma once

#include <raylib.h>

bool approximatelyEqual(float a, float b);

void drawTile(float x, float y, Color color);

void drawHorizontalLineAtTile(int x, int y, Color color, float thickness);

void drawVerticalLineAtTile(int x, int y, Color color, float thickness);
