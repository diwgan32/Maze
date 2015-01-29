#ifndef MAZE_H
#define MAZE_H

#include "stdafx.h"

void ChangeSize(int, int);
void SetupRC(void/*HINSTANCE hInstance*/);
void RenderScene(void);

void ProcessSceneInfo(void);
void DownKeys(unsigned char, int, int);
void UpKeys(unsigned char, int, int);

#endif