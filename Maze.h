#ifndef MAZE_H
#define MAZE_H

#include "stdafx.h"

void changeSize(int, int);
void setupRC(void/*HINSTANCE hInstance*/);
void renderScene(void);

void countFPS(void);
void renderText(char *, int, int);
void processSceneInfo(void);
void downKeys(unsigned char, int, int);
void upKeys(unsigned char, int, int);

#endif