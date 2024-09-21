#pragma once
#include "imgui.h"
#include "Snake.h"

class Food
{
public:

	Food(Snake&);

	void RefreshFood(Snake&); // 刷新食物位置

	ImVec2 GetFoodPosition() const; // 获取食物位置

private:

	ImVec2 foodPosition; // 食物位置

};
