#ifndef GRID_H
#define GRID_H

#include <SFML/Graphics.hpp>
#include "GridCellStates.hpp"
#include "math.h"
#include <stdio.h>
#include <vector>

using namespace std;
using namespace sf;

template <typename T>
class Grid
{
private:
	T ***gridArr; // the grid
	int gridWidth; // the width of the grid
	int gridHeight; // the height of the grid
	int cellSize; // in pixels

public:
	Grid(int width, int height, int cellSize);

	T *getValueAt(int x, int y);

	Vector2f gridToScreen(int x, int y);

	Vector2i screenToGrid(Vector2i pos);

	Vector2f centerScreenCoord(Vector2i pos);

	bool setValAt(int x, int y, T *val);

	bool setValAt(Vector2i pos, T *val);

	int getCellSize();

	int getGridWidth();

	int getGridHeight();

	bool validCoords(int x, int y);

	vector<T*> *getNeighbours(int x, int y, bool includeDiagonals);
	
	~Grid();
};

/// <summary>
/// Get a list of all of the neighbouring cells around the given
/// grid coordinates
/// </summary>
/// <param name="x">the x coordinate of a cell in the grid</param>
/// <param name="y">the x coordinate of a cellin the grid</param>
/// <returns>a list of all of the neighbouring cells around the given
/// grid coordinates if the coordinates are valid and null otherwise</returns>
template <typename T>
vector<T*> *Grid<T>::getNeighbours(int x, int y, bool includeDiagonals)
{
	// check for invalid coords
	if (!validCoords(x, y))
	{
		return NULL;
	}

	// create a list to store the neighbouring cells
	vector<T*>* neighbours = new vector<T*>();

	// find all neighbours around given coordinates
	for (int relativeX = -1; relativeX < 2; relativeX++)
	{
		for (int relativeY = -1; relativeY < 2; relativeY++)
		{
			// skip the center cell
			if (relativeX == 0 && relativeY == 0)
			{
				continue;
			}

			// skip diagonals if needed
			if (!includeDiagonals)
			{
				if ((relativeX == -1 && relativeY == -1)
					|| (relativeX == -1 && relativeY == 1)
					|| (relativeX == 1 && relativeY == -1)
					|| (relativeX == 1 && relativeY == 1))
				{
					continue;
				}
			}

			int currX = x + relativeX;
			int currY = y + relativeY;
			// add neighbouring cell if it exists in the grid
			if (validCoords(currX, currY))
			{
				neighbours->push_back(getValueAt(currX, currY));
			}
		}
	}

	return neighbours;
}

/// <summary>
/// Get the grid's height
/// </summary>
/// <returns>the grid's height</returns>
template <typename T>
int Grid<T>::getGridHeight()
{
	return gridHeight;
}

/// <summary>
/// Get the grid's width
/// </summary>
/// <returns>the grid's width</returns>
template <typename T>
int Grid<T>::getGridWidth()
{
	return gridWidth;
}

/// <summary>
/// Get the cell size
/// </summary>
/// <returns>the cell size in pixels</returns>
template <typename T>
int Grid<T>::getCellSize()
{
	return cellSize;
}

/// <summary>
/// Return the center screen position of the cell closest to the givest 
/// screen position
/// </summary>
/// <param name="pos">A position on the screen</param>
/// <returns>the center screen position of the cell closest to the givest 
/// screen position</returns>
template <typename T>
Vector2f Grid<T>::centerScreenCoord(Vector2i pos)
{
	int posX = (int)floor(pos.x / cellSize) * cellSize;
	int posY = (int)floor(pos.y / cellSize) * cellSize;
	return Vector2f(posX, posY);
}

/// <summary>
/// Create an instance of a grid
/// </summary>
/// <param name="width">the width of the grid</param>
/// <param name="height">the height of the grid</param>
/// <param name="cellSize">the size of each grid square in pixels</param>
template <typename T>
Grid<T>::Grid(int width, int height, int cellSize)
{
	this->cellSize = cellSize;
	// initialize grid
	gridArr = new T **[width];
	for (int x = 0; x < width; x++)
	{
		gridArr[x] = new T *[height];
	}

	// initialize instance variables
	gridWidth = width;
	gridHeight = height;
	this->cellSize = cellSize;
}

/// <summary>
/// Get the value at the given grid coordinates
/// </summary>
/// <param name="relativeX">the row cooridnate</param>
/// <param name="relativeY">the column coordinate</param>
/// <returns>if the coordinates are valid, returns the value at the given grid 
/// coordinates, and returns false otherwise</returns>
template <typename T>
T *Grid<T>::getValueAt(int x, int y)
{
	if (x < gridWidth && y < gridHeight)
	{
		return gridArr[x][y];
	}

	return NULL;
}

/// <summary>
/// Convert the grid position to a screen position
/// </summary>
/// <param name="relativeX">the relativeX coordinate of the desired grid space</param>
/// <param name="relativeY">the relativeY coordinate of the dedired grid space</param>
/// <returns>a screen position in pixels as a Vector2f of the given cell
/// in the grid if the given coordinates are valid, otherwise returns NULL</returns>
template <typename T>
Vector2f Grid<T>::gridToScreen(int x, int y)
{
	float screenX = x * cellSize;
	float screenY = y * cellSize;

	return Vector2f(screenX, screenY);
}


/// <summary>
/// Convert the given screen position to a grid position
/// </summary>
/// <param name="pos">The screen position to be converted</param>
/// <returns>The screen position as a grid position</returns>
template <typename T>
Vector2i Grid<T>::screenToGrid(Vector2i pos)
{
	int xPos = (int)floor(pos.x / cellSize);
	int yPos = (int)floor(pos.y / cellSize);
	return Vector2i(xPos, yPos);
}

/// <summary>
/// Check to see if the given grid coordinates are valid
/// </summary>
/// <param name="relativeX">the relativeX coordinate of the desired cell</param>
/// <param name="relativeY"the relativeY coordinate of the desired cell></param>
/// <returns>true if the cell is valid and false otherwise</returns>
template <typename T>
bool Grid<T>::validCoords(int x, int y)
{
	return (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight);
}

/// <summary>
/// Set the value of the cell at the given coordinates
/// </summary>
/// <param name="relativeX">the relativeX coordinate of the cell</param>
/// <param name="relativeY">the relativeY coordinate of the cell</param>
/// <param name="val">the new value of the given cell</param>
/// <returns>true if the cell is valid and false otherwise</returns>
template <typename T>
bool Grid<T>::setValAt(int x, int y, T *val)
{
	if (validCoords(x, y))
	{
		// set the value at the cell
		gridArr[x][y] = val;

		return true;
	}

	return false;

}

/// <summary>
/// Set the value of the cell at the given screen position
/// </summary>
/// <param name="pos">the screen position of the cell</param>
/// <param name="val">the new value of the cell</param>
/// <returns>true if the cell is valid and false otherwise</returns>
template <typename T>
bool Grid<T>::setValAt(Vector2i pos, T *val)
{
	Vector2i gridCoords(screenToGrid(pos));
	return setValAt(gridCoords.x, gridCoords.y, val);
}

/// <summary>
/// Destructor for a grid object
/// </summary>
template <typename T>
Grid<T>::~Grid()
{
	// free the grid array
	for (int x = 0; x < gridWidth; x++)
	{
		delete gridArr[x];
	}

	delete gridArr;
}

#endif

