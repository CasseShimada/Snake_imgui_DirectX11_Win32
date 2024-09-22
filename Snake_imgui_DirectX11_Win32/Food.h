#pragma once
#include "Snake.h"

class Snake;

class Food
{
public:

	std::pair<int, int> GetFoodPosition() const; // 获取食物位置
	void SetFoodPosition(std::pair<int, int>); // 设置食物位置

private:

	std::pair<int, int> pos; // 食物位置

};
