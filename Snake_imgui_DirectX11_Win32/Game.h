#pragma once
#include "Timer.h"

#include "Food.h"
#include "Snake.h"

#define GRID_WIDTH 20
#define GRID_HEIGHT 20

class Food;
class Snake;

class Game
{

public:

	Game(); // 构造函数
	void startGame(); // 游戏开始
	void endGame(); // 游戏结束

private:

	Timer timer; // 计时器
	Food food; // 食物
	Snake snake; // 蛇
	bool gameOver; // 游戏结束的标志
	int score; // 得分
};
