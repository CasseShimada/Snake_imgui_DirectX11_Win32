#include "Food.h"

std::pair<int, int> Food::GetFoodPosition() const
{
	return pos;
}

void Food::SetFoodPosition(std::pair<int, int> new_pos)
{
	pos = new_pos;
}
