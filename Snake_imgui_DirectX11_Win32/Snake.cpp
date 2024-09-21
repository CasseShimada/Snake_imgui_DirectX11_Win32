#include "Snake.h"

// 构造函数，初始化蛇的属性
Snake::Snake()
{
	length = 3; // 初始长度为 3
	direction = RIGHT; // 初始方向向右
	speed = 1.0f; // 初始速度为 1.0格/秒
	status = ALIVE; // 初始蛇活着

	// 创建蛇头节点
	head = new SnakeNode{ 10, 10, nullptr };

	// 创建蛇身其他节点
	SnakeNode* second = new SnakeNode{ 9, 10, nullptr };
	SnakeNode* third = new SnakeNode{ 8, 10, nullptr };

	// 连接节点
	head->next = second;
	second->next = third;
}

// 碰撞检测（撞墙、撞蛇身）
bool Snake::CheckCollision()
{
	ImVec2 headPos = GetHeadPos();

	// 1. 蛇头与墙壁的碰撞
	if (headPos.x < 0 || headPos.x >= GRID_WIDTH ||
		headPos.y < 0 || headPos.y >= GRID_HEIGHT) {
		return true; // 撞墙
	}

	// 2. 蛇头与自身的碰撞
	SnakeNode* current = head->next; // 从第二个节点开始遍历
	while (current != nullptr) {
		if (current->x == headPos.x && current->y == headPos.y) {
			return true; // 撞到自己
		}
		current = current->next;
	}

	return false; // 未发生碰撞
}

// 蛇移动
void Snake::Move(ImVec2 nextHeadPos)
{
	SnakeNode* newHead = new SnakeNode{ static_cast<int>(nextHeadPos.x),
				static_cast<int>(nextHeadPos.y), nullptr };

	newHead->next = head; // 将新头节点连接到当前头节点之前
	head = newHead; // 更新头节点为新头节点

	RemoveTail(); // 删除尾节点
}

//// 移动蛇
//void Snake::Move(Food& food)
//{
//	ImVec2 nextHeadPos = GetHeadNextPos();
//
//	if (CheckCollision()) { // 判断蛇是否撞死
//		// 移动
//		SnakeNode* newHead = new SnakeNode{ static_cast<int>(nextHeadPos.x),
//				static_cast<int>(nextHeadPos.y), nullptr };
//
//		newHead->next = head; // 将新头节点连接到当前头节点之前
//		head = newHead; // 更新头节点为新头节点
//
//		RemoveTail(); // 删除尾节点
//
//		// 撞死
//		status = DEAD;
//	}
//	else {
//		// 判断下一步是否会碰到食物
//		if (nextHeadPos.x == food.GetFoodPosition().x &&
//			nextHeadPos.y == food.GetFoodPosition().y) {
//
//			// 会碰到食物，增长
//			Grow(nextHeadPos); // 在蛇头添加新节点
//			food.RefreshFood(*this); // 生成新的食物
//
//		}
//		else {
//			// 不会碰到食物，移动
//			SnakeNode* newHead = new SnakeNode{ static_cast<int>(nextHeadPos.x),
//				static_cast<int>(nextHeadPos.y), nullptr };
//
//			newHead->next = head;
//			head = newHead;
//
//			RemoveTail();
//		}
//	}
//}

// 蛇生长
void Snake::Grow(ImVec2& next)
{
	SnakeNode* newHead = new SnakeNode{ static_cast<int>(next.x),
		static_cast<int>(next.y), nullptr };

	newHead->next = head; // 将新头节点连接到当前头节点之前
	head = newHead; // 更新头节点为新头节点

	length++;
}

// 获取蛇头坐标
ImVec2 Snake::GetHeadPos() const
{
	return ImVec2(head->x, head->y);
}

// 获取蛇头结点
SnakeNode* Snake::GetHeadNode() const
{
	return head;
}

// 获取蛇的长度
int Snake::GetLength() const
{
	return length;
}

// 获取下一次移动蛇头位置
ImVec2 Snake::GetHeadNextPos() const
{
	ImVec2 nextHeadPos = GetHeadPos(); // 获取当前蛇头坐标

	// 计算下一步蛇头坐标
	switch (direction) {
	case UP:
		nextHeadPos.x = head->x;
		nextHeadPos.y = head->y - 1; // 向上移动，y 坐标减 1
		break;
	case DOWN:
		nextHeadPos.x = head->x;
		nextHeadPos.y = head->y + 1; // 向下移动，y 坐标加 1
		break;
	case LEFT:
		nextHeadPos.x = head->x - 1; // 向左移动，x 坐标减 1
		nextHeadPos.y = head->y;
		break;
	case RIGHT:
		nextHeadPos.x = head->x + 1; // 向右移动，x 坐标加 1
		nextHeadPos.y = head->y;
		break;
	}

	return nextHeadPos;
}

void Snake::RemoveTail()
{
	//if (head == nullptr || head->next == nullptr) { 
	//	// 空链表或只有一个节点
	//	return;
	//}

	SnakeNode* current = head;
	while (current->next->next != nullptr) {
		// 遍历到倒数第二个节点
		current = current->next;
	}

	delete current->next; // 删除最后一个节点
	current->next = nullptr; // 断开连接
}