#include "Food.h"
#include "imgui.h"
#include <random>

Food::Food(Snake& snake)
{
	RefreshFood(snake);
}

void Food::RefreshFood(Snake& snake)
{
	bool overlap = true; // 假设初始情况下食物与蛇重叠
	while (overlap)
	{
		std::random_device rd;  // 获取随机数种子
		std::mt19937 gen(rd()); // 创建随机数生成器
		std::uniform_int_distribution<> dis(0, 19); // 定义随机数范围为 0 到 19
		// 生成新的食物坐标
		foodPosition.x = dis(gen);
		foodPosition.y = dis(gen);

		// 检查新坐标是否与蛇身重叠
		overlap = false;
		SnakeNode* current = snake.GetHeadNode();
		while (current != nullptr)
		{
			if (current->x == foodPosition.x && current->y == foodPosition.y)
			{
				overlap = true;
				break; // 发现重叠，跳出循环
			}
			current = current->next;
		}
	}
}

ImVec2 Food::GetFoodPosition() const
{
	return foodPosition;
}