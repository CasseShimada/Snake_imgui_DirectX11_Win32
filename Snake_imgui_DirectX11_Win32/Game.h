#pragma once
#include "Timer.h"

#include "Food.h"
#include "Snake.h"

#include "imgui.h"

class Food;
class Snake;

class Game
{

public:

	Game(); // 构造函数
	void startGame(); // 游戏开始
	void endGame(); // 游戏结束
	void drawCell(int, int, int, const ImVec4, const bool); // 绘制格子

private:

	Timer timer; // 计时器
	Food food; // 食物
	Snake snake; // 蛇
	bool gameOver; // 游戏结束的标志
	int score; // 得分
};
