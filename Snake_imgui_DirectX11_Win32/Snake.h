#pragma once
#include <utility>

struct SnakeNode
{
	int x;
	int y;
	SnakeNode* next;
};

class Snake
{
public:

	Snake(); // 构造函数，初始化蛇的属性

	void Move(std::pair<int, int>); // 移动蛇
	//void Move(Food&);

	void Grow(std::pair<int, int>&); // 蛇生长

	bool CheckCollision(std::pair<int, int>); // 碰撞检测（撞墙、撞蛇身）

	std::pair<int, int> GetHeadPos() const; // 获取蛇头坐标

	SnakeNode* GetHeadNode() const; // 获取蛇头结点

	int GetLength() const; // 获取蛇的长度

	std::pair<int, int> GetHeadNextPos() const; // 获取下一次移动蛇头位置

	// ... 其他方法

private:

	SnakeNode* head; // 蛇身链表

	int length; // 蛇的长度

	enum Direction { UP, DOWN, LEFT, RIGHT } direction; // 蛇的移动方向

	float speed; // 蛇的移动速度

	//enum Status { DEAD, ALIVE } status; // 蛇是否活着

	void RemoveTail(); // 去除尾部节点

	// ... 其他属性
};