#pragma once
#include <utility>

struct SnakeNode
{

	int x;
	int y;
	SnakeNode* next;

	std::pair<int, int> GetPosition()
	{
		return std::pair<int, int>(x, y);
	}

};

class Snake
{
public:

	Snake(); // 构造函数，初始化蛇的属性

	std::pair<int, int> GetHeadPos() const; // 获取蛇头坐标
	SnakeNode* GetHeadNode() const; // 获取蛇头结点
	int GetLength() const; // 获取蛇的长度
	void SetHeadNode(SnakeNode*);

	void Generate();
	void Delete();
	void Reset();
	void ModifyLength(int);
	void RemoveTail(); // 去除尾部节点

private:

	SnakeNode* head; // 蛇身链表
	int length; // 蛇的长度

};