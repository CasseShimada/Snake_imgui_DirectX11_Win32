#include "Game.h"
#include <random>

// 构造函数
Game::Game() :max_length(0), snake(), food(), gameOver(false), score(0), snake_move_interval(0), grid_color(0), snake_color(0), direction(RIGHT), pause(false)
{

}

void Game::Start()
{
	gameOver = false;
	pause = false;
	timer.Start();
	RefreshFood();
}

void Game::Reset()
{

	gameOver = true;
	pause = false;
	score = 0;
	snake_move_interval = 1.0f / game_settings.speed;
	max_length = game_settings.grid_size.first * game_settings.grid_size.second;
	grid_color = ImColor(game_settings.grid_color);
	snake_color = ImColor(game_settings.snake_color);

	timer.Reset();
	snake.Reset();

}

void Game::Update()
{
	if (gameOver == false)
	{
		if (ImGui::IsKeyDown(ImGuiKey_Escape))
		{
			pause = !pause;
		}
		if (!pause)
		{
			if (direction != DOWN && ImGui::IsKeyDown(ImGuiKey_GamepadDpadUp))
			{
				direction = UP;
			}
			else if (direction != UP && ImGui::IsKeyDown(ImGuiKey_GamepadDpadDown))
			{
				direction = DOWN;
			}
			else if (direction != RIGHT && ImGui::IsKeyDown(ImGuiKey_GamepadDpadLeft))
			{
				direction = LEFT;
			}
			else if (direction != LEFT && ImGui::IsKeyDown(ImGuiKey_GamepadDpadRight))
			{
				direction = RIGHT;
			}
			if (timer.GetElapsedSeconds() >= snake_move_interval) // 蛇移动
			{
				timer.Start();
				// 移动蛇
				if (CheckSnakeEat()) // 蛇吃到东西
				{
					GrowSnake(); // 生长

					// 判断蛇是否充满了整个画面
					if (snake.GetLength() == max_length) // 蛇充满了整个画面
					{
						gameOver = true; // 游戏结束
					}
					else
					{
						RefreshFood();
					}
				}
				else
					MoveSnake(); // 蛇移动一次

				// 判断蛇是否撞死
				if (CheckCollision()) // 蛇撞死了
				{
					gameOver = true; // 游戏结束
				}
			}
		}

		// 绘制格子
		for (int i = 0; i < game_settings.grid_size.first; i++)
		{
			for (int j = 0; j < game_settings.grid_size.second; j++)
			{
				DrawGrid(game_settings.cell_size, i, j, grid_color, false);
			}
		}

		DrawFood();

		// 绘制蛇
		DrawSnake();

	}
	if (gameOver)
	{
		DrawEndScreen();
	}
}

void Game::End()
{
	// 显示结算画面

}

void Game::DrawGrid(int cell_size, int x, int y, const ImU32 color,
	const bool filled)
{
	ImVec2 windowPos = ImGui::GetWindowPos(); // 获取窗口的屏幕坐标

	ImVec2 top_left = ImVec2(windowPos.x + x * cell_size,
		windowPos.y + y * cell_size);
	ImVec2 bottom_right = ImVec2(windowPos.x + (x + 1) * cell_size,
		windowPos.y + (y + 1) * cell_size);

	const ImU32 color_int = ImColor(color);

	ImDrawList* draw = ImGui::GetWindowDrawList();
	if (filled)
		draw->AddRectFilled(top_left, bottom_right, color_int);
	else
		draw->AddRect(top_left, bottom_right, color_int);
}

void Game::DrawSnake()
{
	SnakeNode* current = snake.GetHeadNode(); // 遍历蛇
	while (current != nullptr) {

		std::pair<int, int> node_pos = current->GetPosition();

		DrawGrid(game_settings.cell_size, node_pos.first, node_pos.second, snake_color, true);

		current = current->next; // 移动到下一个节点
	}
}

void Game::DrawFood()
{
	DrawGrid(game_settings.cell_size, food.GetFoodPosition().first, food.GetFoodPosition().second, grid_color, true);
}

void Game::DrawEndScreen()
{

	ImVec2 windowPos = ImGui::GetWindowPos(); // 获取窗口的屏幕坐标


}

// 移动蛇
void Game::MoveSnake()
{
	SnakeNode* newHead = new SnakeNode{ static_cast<int>(GetSnakeHeadNextPos().first),
					static_cast<int>(GetSnakeHeadNextPos().second), nullptr };

	newHead->next = snake.GetHeadNode(); // 将新头节点连接到当前头节点之前
	snake.SetHeadNode(newHead); // 更新头节点为新头节点

	snake.RemoveTail(); // 删除尾节点
}

// 蛇生长
void Game::GrowSnake()
{
	std::pair<int, int> nextHeadPos = GetSnakeHeadNextPos();
	SnakeNode* newHead = new SnakeNode{ static_cast<int>(nextHeadPos.first),
					static_cast<int>(nextHeadPos.second), nullptr };

	newHead->next = snake.GetHeadNode();
	snake.SetHeadNode(newHead);

	snake.ModifyLength(1);
}

// 刷新食物
void Game::RefreshFood()
{

	std::random_device rd;  // 获取随机数种子
	std::mt19937 gen(rd()); // 创建随机数生成器
	std::uniform_int_distribution<> dis_x(0, game_settings.grid_size.first - 1);
	std::uniform_int_distribution<> dis_y(0, game_settings.grid_size.second - 1);
	//std::uniform_int_distribution<> dis_x(0, 20);
	//std::uniform_int_distribution<> dis_y(0, 20);

	// 生成新的食物坐标
	std::pair<int, int> new_food_pos = std::pair<int, int>(dis_x(gen), dis_y(gen));

	bool overlap = true; // 假设初始情况下食物与蛇重叠
	while (overlap)
	{
		// 生成新的食物坐标
		std::pair<int, int> new_food_pos = std::pair<int, int>(dis_x(gen), dis_y(gen));

		// 检查新坐标是否与蛇身重叠
		overlap = false;
		SnakeNode* current = snake.GetHeadNode();
		while (current != nullptr)
		{
			std::pair<int, int> snake_node_pos = current->GetPosition();
			if (new_food_pos == snake_node_pos)
			{
				overlap = true;
				break; // 发现重叠，跳出循环
			}
			current = current->next;
		}
	}

	food.SetFoodPosition(new_food_pos);
}

// 获取下一次移动蛇头位置
std::pair<int, int> Game::GetSnakeHeadNextPos() const
{
	std::pair<int, int> nextHeadPos = snake.GetHeadPos(); // 获取当前蛇头坐标

	switch (direction) { // 计算下一步蛇头坐标
	case UP:
		nextHeadPos.second -= 1; // 向上移动，y 坐标减 1
		break;
	case DOWN:
		nextHeadPos.second += 1; // 向下移动，y 坐标加 1
		break;
	case LEFT:
		nextHeadPos.first -= 1; // 向左移动，x 坐标减 1
		break;
	case RIGHT:
		nextHeadPos.first += 1; // 向右移动，x 坐标加 1
		break;
	}

	return nextHeadPos;
}

bool Game::CheckSnakeEat() const
{
	if (GetSnakeHeadNextPos() == food.GetFoodPosition())
		return true;

	return false;
}


// 碰撞检测（撞墙、撞蛇身）
bool Game::CheckCollision() const
{
	std::pair<int, int> headPos = snake.GetHeadPos();

	// 1. 蛇头与墙壁的碰撞
	if (headPos.first < 0 || headPos.first >= game_settings.grid_size.first ||
		headPos.second < 0 || headPos.second >= game_settings.grid_size.second) {
		return true; // 撞墙
	}

	// 2. 蛇头与自身的碰撞
	SnakeNode* current = snake.GetHeadNode()->next; // 从第二个节点开始遍历
	while (current != nullptr) {
		if (current->x == headPos.first && current->y == headPos.second) {
			return true; // 撞到自己
		}
		current = current->next;
	}

	return false; // 未发生碰撞
}

void Game::SyncGameSettings(int speed, std::pair<int, int> grid_size, int cell_size, ImVec4 grid_color, ImVec4 snake_color)
{
	game_settings.speed = speed;
	game_settings.grid_size = grid_size;
	game_settings.cell_size = cell_size;
	game_settings.grid_color = grid_color;
	game_settings.snake_color = snake_color;
}

bool Game::CheckGameOver()
{
	return gameOver;
}

