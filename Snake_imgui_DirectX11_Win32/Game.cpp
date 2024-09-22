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