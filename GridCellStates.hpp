#ifndef GRIDVAL_H
#define GRIDVAL_H

#include <SFML/Graphics.hpp>

enum class GridValue
{
	INVALID = -1,
	UNOCCUPIED,
	OCCUPIED,
	SELECTED,
	DESTINATION,
	START
};

enum class GridStateColor : unsigned long
{
	INVALID_COLOR,
	OCCUPIED_COLOR = 0xFFFFFFFF, // white
	UNOCCUPED_COLOR = 0x00000000, // transparent
	SELECTED_COLOR = 0xFF000064, // slightly transparent red
	DEST_COLOR = 0x0097FFFF, // light blue
	START_COLOR = 0xFFFB00FF // bright yellow
};

GridStateColor valToColor(GridValue val)
{
	switch (val)
	{
		case GridValue::OCCUPIED:
			return GridStateColor::OCCUPIED_COLOR;
		case GridValue::UNOCCUPIED:
			return GridStateColor::UNOCCUPED_COLOR;
		case GridValue::SELECTED:
			return GridStateColor::SELECTED_COLOR;
		case GridValue::DESTINATION:
			return GridStateColor::DEST_COLOR;
		case GridValue::START:
			return GridStateColor::START_COLOR;
		default:
			return GridStateColor::INVALID_COLOR;
	}
}

#endif 

