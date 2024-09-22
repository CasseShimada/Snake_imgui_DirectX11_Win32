#pragma once
#include "Timer.h"

#include "Food.h"
#include "Snake.h"

#include "imgui.h"

class Food;
class Snake;

struct GameSettings
{

	int speed = 0; // 速度
	std::pair<int, int> grid_size; // 舞台尺寸
	int cell_size = 0; // 格子大小
	ImVec4 grid_color; // 格子颜色
	ImVec4 snake_color;

};

class Game
{

public:

	Game();

	// 游戏流程
	void Start(); // 开始游戏
	void Reset(); // 重置游戏
	void Update();
	void End(); // 游戏结束

	// 渲染
	void DrawGrid(int, int, int, const ImU32, const bool); // 绘制格子
	void DrawSnake();
	void DrawEndScreen();

	// 工具
	void MoveSnake(); // 蛇移动
	void GrowSnake(); // 蛇生长
	void RefreshFood();
	std::pair<int, int> GetSnakeHeadNextPos() const; // 获取下一次移动蛇头位置
	bool CheckSnakeEat() const; // 检测蛇吃食物
	bool CheckCollision() const; // 检测蛇碰撞

	// 同步设置
	void SyncGameSettings(int, std::pair<int, int>, int, ImVec4, ImVec4);

private:

	// 游戏设置
	GameSettings game_settings;

	float snake_move_interval; // 蛇速度
	ImU32 grid_color; // 格子颜色
	ImU32 snake_color;

	// 游戏变量
	Timer timer; // 计时器
	Food food; // 食物
	Snake snake; // 蛇
	int score; // 得分
	bool gameOver; // 游戏结束的标志
	int max_length;

	// 玩家控制
	enum Direction { UP, DOWN, LEFT, RIGHT } direction; // 蛇的移动方向
};
