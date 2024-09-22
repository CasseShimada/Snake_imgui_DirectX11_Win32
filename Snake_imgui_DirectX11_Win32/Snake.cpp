#include "Snake.h"

// 构造函数，初始化蛇的属性
Snake::Snake() :length(3)
{
	Generate();
}

// 创建新的蛇（长度3）
void Snake::Generate()
{
	// 创建蛇头节点
	head = new SnakeNode{ 10, 10, nullptr };

	// 创建蛇身其他节点
	SnakeNode* second = new SnakeNode{ 9, 10, nullptr };
	SnakeNode* third = new SnakeNode{ 8, 10, nullptr };

	// 连接节点
	head->next = second;
	second->next = third;
}

// 删除旧的蛇
void Snake::Delete()
{
	SnakeNode* current = head;
	head = NULL;
	while (current != NULL) {
		SnakeNode* temp = current;
		current = current->next;
		delete temp;
	}
}

// 重置蛇状态
void Snake::Reset()
{
	Delete();
	Generate();

	length = 3;
}

// 编辑蛇长
void Snake::ModifyLength(int modifier)
{
	length += modifier;
}

// 获取蛇头坐标
std::pair<int, int> Snake::GetHeadPos() const
{
	return std::pair<int, int>(head->x, head->y);
}

// 获取蛇头节点
SnakeNode* Snake::GetHeadNode() const
{
	return head;
}

void Snake::SetHeadNode(SnakeNode* new_head)
{
	head = new_head;
}

// 获取蛇长
int Snake::GetLength() const
{
	return length;
}

// 删除蛇的最后一个节点
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
