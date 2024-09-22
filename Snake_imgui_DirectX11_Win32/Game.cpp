#include "Game.h"


// 构造函数
Game::Game() :snake(), food(snake), gameOver(false), score(0)
{

}

// 游戏开始
void Game::startGame()
{
	while (!gameOver) {
		// ... (游戏主循环逻辑)

	}
	endGame();
}

// 游戏结束
void Game::endGame()
{

}

void Game::drawCell(int cell_size, int x, int y, const ImVec4 color, const bool filled)
{
	// 获取窗口的屏幕坐标
	ImVec2 windowPos = ImGui::GetWindowPos();

	ImVec2 top_left = ImVec2(windowPos.x + x * cell_size, windowPos.y + y * cell_size);
	ImVec2 bottom_right = ImVec2(windowPos.x + (x + 1) * cell_size, windowPos.y + (y + 1) * cell_size);

	const ImU32 color_int = ImColor(color);

	ImDrawList* draw = ImGui::GetWindowDrawList();
	if (filled)
		draw->AddRectFilled(top_left, bottom_right, color_int);
	else
		draw->AddRect(top_left, bottom_right, color_int);
}