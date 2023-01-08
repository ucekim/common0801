#ifndef PATH_FINDER_H
#define PATH_FINDER_H

#include <stdio.h>
#include "SFML/Graphics.hpp"
#include "Grid.hpp"
#include "GridCellStates.hpp"
#include <vector>
#include <unordered_set>
#include <algorithm>

using namespace std;
using namespace sf;

/// <summary>
/// A class for finding the shortest path between two cells in a grid
/// </summary>
class PathFinder
{
public:
	// node for each grid position
	struct GridNode
	{
		GridValue val; // value of this node
		Vector2i gridPos; // position of this node in the grid
		GridNode *parentNode; // node that came before this node in the path
		int gCost; // distance from starting node
		int hCost; // distance from end node

		int fCost()
		{
			return gCost + hCost;
		}

		// override equals operator
		bool operator == (const GridNode& other) const
		{
			return gridPos == other.gridPos;
		}

		// override not equals operator
		bool operator != (const GridNode& other) const
		{
			return gridPos != other.gridPos;
		}
	};

	// custom hashing for GridNodes
	struct NodeHash
	{
		size_t operator() (const GridNode* node) const
		{
			return node->gCost + node->hCost + (int)node->val;
		}
	};

private:
	// private instance variables
	Grid<GridNode> *grid;
	int outlineThickness;

	Vector2i* startPos;
	Vector2i* endPos;

	int getDistance(GridNode* node1, GridNode* node2);

public:
	PathFinder(int width, int height, int cellSize);

	PathFinder(int width, int height, int cellSize, int outlineThickness);

	~PathFinder();

	Grid<GridNode> *getGrid();

	void drawGrid(RenderWindow *window);

	bool setValAt(int x, int y, GridValue val);

	bool setValAt(Vector2i pos, GridValue val);

	vector<GridNode *> *getShortestPath(bool includeDiagonals);

	bool drawShortestPath(RenderWindow* window, bool includeDiagonals);

private:
	void initializeNodes();

	vector<GridNode*> *retracePath(GridNode* startNode, GridNode* endNode);
};

/// <summary>
/// Initialize the nodes in the grid
/// </summary>
void PathFinder::initializeNodes()
{
	for (int x = 0; x < grid->getGridWidth(); x++)
	{
		for (int y = 0; y < grid->getGridHeight(); y++)
		{
			GridNode* newNode = new GridNode();
			newNode->gridPos = Vector2i(x, y);
			grid->setValAt(x, y, newNode);
		}
	}
}

/// <summary>
/// Constructor for a new PathFinder object
/// </summary>
/// <param name="width">the width of the grid</param>
/// <param name="height">the length of the grid</param>
/// <param name="cellSize">the size of each grid cell</param>
PathFinder::PathFinder(int width, int height, int cellSize)
{
	grid = new Grid<GridNode>(width, height, cellSize);
	outlineThickness = 1;

	startPos = NULL;
	endPos = NULL;

	initializeNodes();
}

/// <summary>
/// Constructor for a new PathFinder object
/// </summary>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="cellSize"></param>
/// <param name="outlineThickness">the thickness of the outline of each grid square</param>
PathFinder::PathFinder(int width, int height, int cellSize, int outlineThickness)
{
	grid = new Grid<GridNode>(width, height, cellSize);
	this->outlineThickness = outlineThickness;

	startPos = NULL;
	endPos = NULL;

	initializeNodes();
}

/// <summary>
/// A destructor for a PathFinder object
/// </summary>
PathFinder::~PathFinder()
{
	cout << "startPos == null: " << (startPos == NULL) << endl;
	cout << "destPos == null: " << (endPos == NULL) << endl;

	for (int x = 0; x < grid->getGridWidth(); x++)
	{
		for (int y = 0; y < grid->getGridHeight(); y++)
		{
			GridNode* currNode = grid->getValueAt(x, y);
			delete(currNode);
		}
	}

	delete(grid);
}

/// <summary>
/// Get a reference to the internal grid object used for pathfinding
/// </summary>
/// <returns>a reference to the internal grid object used for pathfinding</returns>
Grid<PathFinder::GridNode>* PathFinder::getGrid()
{
	return grid;
}

/// <summary>
/// Draws this grid in the given window
/// </summary>
/// <param name="window">window to draw into</param>
void PathFinder::drawGrid(RenderWindow *window)
{
	int gridWidth = grid->getGridWidth();
	int gridHeight = grid->getGridHeight();
	int cellSize = grid->getCellSize();

	for (int x = 0; x < gridWidth; x++)
	{
		for (int y = 0; y < gridHeight; y++)
		{
			PathFinder::GridNode *node = grid->getValueAt(x, y);
			if (node != NULL)
			{
				RectangleShape cell = RectangleShape(Vector2f(cellSize, cellSize));
				Vector2f pos = grid->gridToScreen(x, y);
				cell.setPosition(pos);
				cell.setOutlineThickness(outlineThickness);

				GridStateColor color = valToColor(node->val);
				cell.setFillColor(Color((unsigned long)color));
				cell.setOutlineColor(Color::White);

				window->draw(cell);
			}				
		}
	}
}

/// <summary>
/// Set the value at the given grid coordinates
/// </summary>
/// <param name="x">the x coordinate of a cell in the grid</param>
/// <param name="y">the y coordinate of a cell in the grid</param>
/// <param name="val">the new value of the cell at (x, y) in the grid</param>
/// <returns>true if the coordinates are valid and false otherwise</returns>
bool PathFinder::setValAt(int x, int y, GridValue val)
{
	if (!grid->validCoords(x, y))
	{
		return false;
	}

	// only allow one start and dest cell
	if (val == GridValue::START)
	{
		// set old start to unoccupied
		if (startPos == NULL)
		{
			startPos = new Vector2i(x, y);
		}
		else
		{
			// unoccupy old start pos
			GridNode *oldStart = grid->getValueAt(startPos->x, startPos->y);
			oldStart->val = GridValue::UNOCCUPIED;

			// set new start pos
			startPos->x = x;
			startPos->y = y;
		}
	}
	else if (val == GridValue::DESTINATION)
	{
		// set old dest to unoccupied
		if (endPos == NULL)
		{
			endPos = new Vector2i(x, y);
		}
		else
		{
			// unoccupy old end pos
			GridNode* oldEnd = grid->getValueAt(endPos->x, endPos->y);
			oldEnd->val = GridValue::UNOCCUPIED;

			// set new dest pos
			endPos->x = x;
			endPos->y = y;
		}
	}
	else
	{
		// make sure that dest/start are removed if the cell is overwritten 
		GridNode *currNode = grid->getValueAt(x, y);
		if (startPos != NULL && currNode->val == GridValue::START)
		{
			// remove start pos
			delete(startPos);
			startPos = NULL;
		}
		else if (endPos != NULL && currNode->val == GridValue::DESTINATION)
		{
			// remove destination pos
			delete(endPos);
			endPos = NULL;
		}
	}

	// set the value at the cell
	grid->getValueAt(x, y)->val = val;

	return true;
}

/// <summary>
/// Set the value at the given screen pos
/// </summary>
/// <param name="pos">The screen position of the desired grid</param>
/// <param name="val">the new value of the cell at the given screen position</param>
/// <returns>true if the position is valid and false otherwise</returns>
bool PathFinder::setValAt(Vector2i pos, GridValue val)
{
	Vector2i gridPos = grid->screenToGrid(pos);
	return setValAt(gridPos.x, gridPos.y, val);
}

/// <summary>
/// Find the distance between two nodes. Distance is given as a cost
/// </summary>
/// <param name="node1">a grid node</param>
/// <param name="node2">a grid onde</param>
/// <returns>the distance between two nodes given as a cost</returns>
int PathFinder::getDistance(GridNode* node1, GridNode* node2)
{
	// cost of moves
	const int DIAGONAL_COST = 14;
	const int NORMAL_COST = 10;

	// distance between the two nodes
	int xDist = abs(node1->gridPos.x - node2->gridPos.x);
	int yDist = abs(node1->gridPos.y - node2->gridPos.y);

	// find the number of diagonal and normal moves
	int numOfDiagonals = min(xDist, yDist);
	int numOfNormMoves = abs(xDist - yDist);

	return DIAGONAL_COST * numOfDiagonals + NORMAL_COST * numOfNormMoves;
}

/// <summary>
/// Retrace the path from the end node to the start node
/// </summary>
/// <param name="endNode">the end node in the grid</param>
/// <param name="startNode">the starting node in the grid</param>
/// <returns>the path from the end node to the start node as a vector of nodes
/// starting with the starting node</returns>
vector<PathFinder::GridNode*> *PathFinder::retracePath(GridNode *startNode, GridNode *endNode)
{
	vector<GridNode*>* path = new vector<GridNode*>();
	GridNode* currNode = endNode;
	// traverse the path backwards starting from the end node
	// and add every node in the path to the path vector
	while (currNode != startNode)
	{
		path->push_back(currNode);
		currNode = currNode->parentNode;
	}
	// reverse the path vector
	reverse(path->begin(), path->end());

	return path;
}

/// <summary>
/// Get the shortest path from the start and end positions
/// </summary>
/// <returns>the shortest path from the start and end positions if 
/// there are start and end positions, otherwise return NULL</returns>
vector<PathFinder::GridNode *> *PathFinder::getShortestPath(bool includeDiagonals)
{
	// only find shortest path if a start and end exist
	if (startPos == NULL || endPos == NULL)
	{
		return NULL;
	}
	// get the start and end nodes
	GridNode* startNode = grid->getValueAt(startPos->x, startPos->y);
	GridNode* endNode = grid->getValueAt(endPos->x, endPos->y);

	// list holds the nodes that CAN be part of the path
	vector<GridNode*> openList; 
	// set holds the nodes that HAVE been picked for a path
	unordered_set<GridNode*, NodeHash> closedSet;

	// openList starts with the start node
	openList.push_back(startNode);

	while (openList.size() > 0)
	{
		// find node with lowest fcost or lowest hcost if fcost are the same
		GridNode* lowestCostNode = openList[0];
		int pos = 0;
		for (int i = 1; i < openList.size(); i++)
		{
			GridNode* currOpenNode = openList[i];
			if (currOpenNode->fCost() < lowestCostNode->fCost()
					|| (currOpenNode->fCost() == lowestCostNode->fCost()
					&& currOpenNode->hCost <= lowestCostNode->hCost))
			{
				lowestCostNode = currOpenNode;
				pos = i;
			}
		}

		// remove lowest cost node from open list
		// because we are going to consider it as part of a path
		vector<GridNode*>::iterator it = openList.begin() + pos;
		openList.erase(it);
		// add lowest cost node to closed set
		closedSet.insert(lowestCostNode);

		// check to see if lowestCostNode is the end node
		if (lowestCostNode == endNode)
		{
			return retracePath(startNode, endNode);
		}

		// update all neighbour nodes
		Vector2i lowestCostPos = lowestCostNode->gridPos;
		vector<GridNode*>* neighbours = grid->getNeighbours(lowestCostPos.x, lowestCostPos.y, includeDiagonals);
		for (int i = 0; i < neighbours->size(); i++)
		{
			GridNode* currNeighbour = (*neighbours)[i];
			// if the neighbour is occupied (so can't be moved to) or it
			// is already considered as part of the path (is in closed set)
			// then ignore it and move onto the next neighbour
			if (currNeighbour->val == GridValue::OCCUPIED
				|| closedSet.find(currNeighbour) != closedSet.end())
			{
				continue;
			}

			int newMovementCostToNeighbour =
				lowestCostNode->gCost + getDistance(lowestCostNode, currNeighbour);
			bool isInOpenSet = find(openList.begin(), openList.end(), currNeighbour) 
					!= openList.end();
			// if the neighbour's current cost is greater than the new cost
			// (aka part of a longer path) or the neighbour has not been 
			// considered for a path yet, then update its values
			if (newMovementCostToNeighbour < currNeighbour->gCost 
					|| !isInOpenSet)
			{
				// set g and h costs of neighbour
				currNeighbour->gCost = newMovementCostToNeighbour;
				currNeighbour->hCost = getDistance(currNeighbour, endNode);
				// set parent of neighbour to the lowestCostNode
				currNeighbour->parentNode = lowestCostNode;
				// add neighbour to open list if it is not in it
				// (aka has not been considered for a path yet)
				if (!isInOpenSet)
				{
					openList.push_back(currNeighbour);
				}
			}
		}

	}
}

bool PathFinder::drawShortestPath(RenderWindow* window, bool includeDiagonals)
{
	int cellSize = grid->getCellSize();

	vector<GridNode*> *path = getShortestPath(includeDiagonals);
	if (path == NULL)
	{
		return false;
	}

	Color pathColor = Color::Green;

	// draw the path
	for (int i = 0; i < path->size(); i++)
	{
		GridNode *currNode = (*path)[i];
		Vector2i currPos = currNode->gridPos;

		RectangleShape square(Vector2f(cellSize, cellSize));
		Vector2f pos = grid->gridToScreen(currPos.x, currPos.y);
		square.setPosition(pos);
		square.setOutlineThickness(outlineThickness);
		square.setFillColor(pathColor);
		square.setOutlineColor(Color::White);

		window->draw(square);
	}

	delete(path);
	return true;
}


#endif // !PATH_FINDER_H

