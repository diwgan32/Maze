#ifndef MAZE_H
#define MAZE_H

#include "stdafx.h"

#define SAFE_DELETE(ptr) delete ptr; ptr = NULL
#define SLEEP(ms) boost::this_thread::sleep(boost::posix_time::milliseconds(ms))

namespace Maze {

	void changeSize(int, int);
	void setupRC(void/*HINSTANCE hInstance*/);
	void renderScene(void);
	void destroy(void);

	void countFPS(void);
	void renderText(char *, int, int);
	void processSceneInfo(void);
	void downKeys(unsigned char, int, int);
	void upKeys(unsigned char, int, int);
	void mouseFunction(int, int);
	void shutdown(void);

	void procThread(void);

	bool keyIn(const char *);
}

#endif