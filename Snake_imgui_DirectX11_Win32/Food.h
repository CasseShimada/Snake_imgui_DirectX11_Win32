#pragma once
#include "Snake.h"

class Snake;

class Food
{
public:

	Food(Snake&);

	void RefreshFood(Snake&); // 刷新食物位置

	std::pair<int, int> GetFoodPosition() const; // 获取食物位置

private:

	std::pair<int, int> foodPosition; // 食物位置

};
