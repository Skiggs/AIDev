#include"PathFinder.h"
#include"MathFunc.h"
#include<iostream>
#include<chrono> 

std::vector<Wall*> PathFinder::mapWalls;
std::vector<Door*> PathFinder::mapDoors;
std::vector<Window*> PathFinder::mapWindows;
std::vector<Door*> PathFinder::CollidingDoors;
std::vector<Window*> PathFinder::CollidingWindows;
std::vector<Node*> PathFinder::AllNodes;
std::vector<Node*> PathFinder::_iterativeNode;
std::vector<Node*> PathFinder::_pathPool;
Node* PathFinder:: _grid[col][row];
std::multimap<double, Node*> PathFinder::_iterativeNodesMap;


void PathFinder::Init(std::vector<Wall*> walls, std::vector<Door*> doors, std::vector<Window*> windows)
{

	mapWalls = walls;
	mapDoors = doors;
	mapWindows = windows;

	for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < row; j++)
		{
			if (i == 0 && j == 0)
			{
				_grid[i][j]->SetNeighbour(_grid[i + 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i][j + 1]);
			}

			else if (i == col - 1 && j == row - 1)
			{
				_grid[i][j]->SetNeighbour(_grid[i - 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i][j - 1]);
			}

			else if (i == 0 && j == row - 1)
			{
				_grid[i][j]->SetNeighbour(_grid[i + 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i][j - 1]);
			}

			else if (i == col - 1 && j == 0)
			{
				_grid[i][j]->SetNeighbour(_grid[i - 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i][j + 1]);
			}

			else if (i == 0 && j > 0 && j < row - 1)
			{
				_grid[i][j]->SetNeighbour(_grid[i + 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i][j + 1]);
				_grid[i][j]->SetNeighbour(_grid[i][j - 1]);
			}

			else if (i == col - 1 && j > 0 && j < row - 1)
			{
				_grid[i][j]->SetNeighbour(_grid[i - 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i][j + 1]);
				_grid[i][j]->SetNeighbour(_grid[i][j - 1]);
			}

			else if (i > 0 && i < col - 1 && j == 0)
			{
				_grid[i][j]->SetNeighbour(_grid[i - 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i + 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i][j + 1]);
			}

			else if (i > 0 && i < col - 1 && j == row - 1)
			{
				_grid[i][j]->SetNeighbour(_grid[i - 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i + 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i][j - 1]);
			}

			else
			{
				_grid[i][j]->SetNeighbour(_grid[i - 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i + 1][j]);
				_grid[i][j]->SetNeighbour(_grid[i][j + 1]);
				_grid[i][j]->SetNeighbour(_grid[i][j - 1]);
			}
		}
	}

	for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < row; j++)
		{
			AllNodes.push_back(_grid[i][j]);
		}
	}

	for (int i = 0; i < AllNodes.size(); i++)
	{
		for (int j = 0; j < mapWalls.size(); j++)
		{
			float dist = CheckDistFromPoint(AllNodes[i]->GetPosition(), mapWalls[j]->p1, mapWalls[j]->p2);

			if (AllNodes[i]->distToclosestWall == -1) AllNodes[i]->distToclosestWall = dist;

			if (dist < AllNodes[i]->distToclosestWall)
			{
				AllNodes[i]->distToclosestWall = dist;
			}

		}

		for (int j = 0; j < mapWindows.size(); j++)
		{
			float dist = CheckDistFromPoint(AllNodes[i]->GetPosition(), mapWindows[j]->p1, mapWindows[j]->p2);

			if (AllNodes[i]->distToclosestWall == -1) AllNodes[i]->distToclosestWall = dist;

			if (dist < AllNodes[i]->distToclosestWall)
			{
				AllNodes[i]->distToclosestWall = dist;
			}
		}
	}

}

void PathFinder::Draw()
{
	for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < row; j++)
		{
			_grid[i][j]->Draw();
		}
	}
}

void PathFinder::ConnectLinesToNeighbours()
{
	std::vector<Node*> _gridNodes;

	for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < row; j++)
		{
			_gridNodes.push_back(_grid[i][j]);
		}
	}

	for (int i = 0; i < _gridNodes.size(); i++)
	{
		for (int k = 0; k < _gridNodes[i]->GetNeighbours().size(); k++)
		{
			_gridNodes[i]->_connectingArray[k] = { _gridNodes[i]->GetPosition(),_gridNodes[i]->GetNeighbours().at(k)->GetPosition() };
			for (int l = 0; l < mapWalls.size(); l++)
			{
				if (areLinesColliding(_gridNodes[i]->GetPosition(), _gridNodes[i]->GetNeighbours().at(k)->GetPosition(), mapWalls[l]->p1, mapWalls[l]->p2))
				{
					_gridNodes[i]->_connectingArray[k] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
				}
			}

			for (int l = 0; l < mapDoors.size(); l++)
			{
				if (areLinesColliding(_gridNodes[i]->GetPosition(), _gridNodes[i]->GetNeighbours().at(k)->GetPosition(), mapDoors[l]->p1, mapDoors[l]->p2))
				{
					_gridNodes[i]->_connectingArray[k] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
					_gridNodes[i]->DoorAtwhatIndex = { k,mapDoors[l] };
				}
			}



			for (int l = 0; l < mapWindows.size(); l++)
			{
				if (areLinesColliding(_gridNodes[i]->GetPosition(), _gridNodes[i]->GetNeighbours().at(k)->GetPosition(), mapWindows[l]->p1, mapWindows[l]->p2))
				{
					_gridNodes[i]->_connectingArray[k] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
				}
			}
		}
	}
}


void PathFinder::UpdateNodesInRectangle(float nodesInRect[4])
{

	std::vector<Node*> NodesContainer;

	//for (int i = 0; i < col; i++)
	//{
	//	for (int j = 0; j < row; j++)
	//	{
	//		if (_grid[i][j]->GetPosition().x > nodesInRect[0] && _grid[i][j]->GetPosition().x < nodesInRect[1] && _grid[i][j]->GetPosition().y > nodesInRect[2] && _grid[i][j]->GetPosition().y < nodesInRect[3])
	//		{
	//			NodesContainer.push_back(_grid[i][j]);
	//			_grid[i][j]->Reset();
	//		}
	//	}
	//}

	for (int i = 0; i < AllNodes.size(); i++)
	{
		if (AllNodes[i]->GetPosition().x > nodesInRect[0] && AllNodes[i]->GetPosition().x < nodesInRect[1] && AllNodes[i]->GetPosition().y > nodesInRect[2] && AllNodes[i]->GetPosition().y < nodesInRect[3])
		{
			NodesContainer.push_back(AllNodes[i]);
			AllNodes[i]->Reset();
		}
	}

	for (int i = 0; i < NodesContainer.size(); i++)
	{
		for (int j = 0; j < mapDoors.size(); j++)
		{
			if (mapDoors[j]->isOpen)
			{
				float dist = CheckDistFromPoint(NodesContainer[i]->GetPosition(), mapDoors[j]->p1, mapDoors[j]->p2);

				if (NodesContainer[i]->distToclosestWall == -1) NodesContainer[i]->distToclosestWall = dist;

				if (dist < NodesContainer[i]->distToclosestWall)
				{
					NodesContainer[i]->distToclosestWall = dist;
				}
			}
		}
	}

	for (int i = 0; i < NodesContainer.size(); i++)
	{

		for (int k = 0; k < NodesContainer[i]->GetNeighbours().size(); k++)
		{
			NodesContainer[i]->_connectingArray[k] = { NodesContainer[i]->GetPosition(),NodesContainer[i]->GetNeighbours().at(k)->GetPosition() };
			for (int l = 0; l < mapWalls.size(); l++)
			{
				if (areLinesColliding(NodesContainer[i]->GetPosition(), NodesContainer[i]->GetNeighbours().at(k)->GetPosition(), mapWalls[l]->p1, mapWalls[l]->p2))
				{
					NodesContainer[i]->_connectingArray[k] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
				}
			}

			for (int l = 0; l < mapDoors.size(); l++)
			{
				if (areLinesColliding(NodesContainer[i]->GetPosition(), NodesContainer[i]->GetNeighbours().at(k)->GetPosition(), mapDoors[l]->p1, mapDoors[l]->p2))
				{
					NodesContainer[i]->_connectingArray[k] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
					NodesContainer[i]->DoorAtwhatIndex = { k,mapDoors[l] };
				}
			}

			for (int l = 0; l < mapWindows.size(); l++)
			{
				if (areLinesColliding(NodesContainer[i]->GetPosition(), NodesContainer[i]->GetNeighbours().at(k)->GetPosition(), mapWindows[l]->p1, mapWindows[l]->p2))
				{
					NodesContainer[i]->_connectingArray[k] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
				}
			}
		}
	}
}


sf::Vector2f PathFinder::FindUnaffectedPointByGrenade(sf::Vector2f grenadePos, sf::Vector2f enemyPos)
{
	Node* closetNodeToEscape = nullptr;
	float closesNodeDistance = 200.f;
	for (int i = 0; i < AllNodes.size(); ++i)
	{
		for (int l = 0; l < mapWalls.size(); ++l)
		{
			if (Distance(enemyPos, AllNodes[i]->GetPosition()) < closesNodeDistance && (AllNodes[i]->distToclosestWall > 25.f && AllNodes[i]->distToclosestWall < 40.f))
			{
				if (areLinesColliding(grenadePos, AllNodes[i]->GetPosition(), mapWalls[l]->p1, mapWalls[l]->p2))
				{
					sf::Vector2f lineVector = mapWalls[l]->p2 - mapWalls[l]->p1;                                                           // (a) finds the vector for the line from pt1 to pt2  (b) so later I can find out its dot product to check how far the circle is (c) gives the vector in the direction of pos2 
					sf::Vector2f circle2lineVector = AllNodes[i]->GetPosition() - mapWalls[l]->p1;                                       // (a) finds the vector for the pt1 of line to circle's center (b) to check the magnitude of circle's position from pt1 of the line (c) gives the vector in the direction of circle's center 
					float distance = Distance(mapWalls[l]->p1, mapWalls[l]->p2);
					sf::Vector2f unitLineVector = sf::Vector2f(lineVector.x / distance, lineVector.y / distance);   // (a) finds the unit vector for lineVector (b) so we can figure out how far is the circle from pt1 (c) gives the unit vector of line Vector
					float pro = Clamp(DotProduct(circle2lineVector, unitLineVector), 0.f, distance);
					if (pro > 25.f && pro < distance)
					{
						if (CheckIspossiblePath(enemyPos, AllNodes[i]))
						{
							closetNodeToEscape = AllNodes[i];
							closesNodeDistance = Distance(enemyPos, closetNodeToEscape->GetPosition());
						}
					}
				}
			}
		}
	}

	Node* closestNodeOfNoCover = nullptr;
	float closestEscapePtFromEnemy = 1000.f;
	for (int i = 0; i < AllNodes.size(); ++i)
	{
		float distFromEnemy = Distance(enemyPos, AllNodes[i]->GetPosition());
		float dist = Distance(grenadePos, AllNodes[i]->GetPosition());
		int isColliding = -1;
		if (distFromEnemy < closestEscapePtFromEnemy)
		{
			if ((dist > 250.0f && dist < 290.0f) && AllNodes[i]->distToclosestWall > 25.f)
			{
				isColliding = 1;
				for (int l = 0; l < mapWalls.size(); ++l)
				{
					if (areLinesColliding(AllNodes[i]->GetPosition(), grenadePos, mapWalls[l]->p1, mapWalls[l]->p2))
					{
						isColliding = 0;
						break;
					}
				}
			}
		}
		if (isColliding == 1)
		{
			closestNodeOfNoCover = AllNodes[i];
			closestEscapePtFromEnemy = distFromEnemy;
		}
	}


	if (closetNodeToEscape != nullptr && closestNodeOfNoCover != nullptr)
	{
		return (closesNodeDistance < Distance(enemyPos, closestNodeOfNoCover->GetPosition()) ? closetNodeToEscape->GetPosition() : closestNodeOfNoCover->GetPosition());
	}

	else if (closetNodeToEscape != nullptr && closestNodeOfNoCover == nullptr)
	{
		return closetNodeToEscape->GetPosition();
	}

	else if (closetNodeToEscape == nullptr && closestNodeOfNoCover != nullptr)
	{
		return closestNodeOfNoCover->GetPosition();
	}

	else return sf::Vector2f(NULL, NULL);
}

std::pair<sf::Vector2f, Wall*> PathFinder::GetPositionBehindCover(sf::Vector2f playerPos, sf::Vector2f enemyPos)
{
	std::pair<sf::Vector2f, Wall*> coverWithWall;
	Node* closetNodeToEscape = nullptr;
	float closesNodeDistance = 250.f;

	for (int l = 0; l < mapWalls.size(); ++l)
	{
	for (int i = 0; i < AllNodes.size(); ++i)
	{
			if ((Distance(enemyPos, AllNodes[i]->GetPosition()) < closesNodeDistance && Distance(playerPos, AllNodes[i]->GetPosition()) < 250.f) && (AllNodes[i]->distToclosestWall > 25.f && AllNodes[i]->distToclosestWall < 40.f))
			{
				if (areLinesColliding(playerPos, AllNodes[i]->GetPosition(), mapWalls[l]->p1, mapWalls[l]->p2))
				{
					sf::Vector2f lineVector = mapWalls[l]->p2 - mapWalls[l]->p1;                                                           // (a) finds the vector for the line from pt1 to pt2  (b) so later I can find out its dot product to check how far the circle is (c) gives the vector in the direction of pos2 
					sf::Vector2f circle2lineVector = AllNodes[i]->GetPosition() - mapWalls[l]->p1;                                       // (a) finds the vector for the pt1 of line to circle's center (b) to check the magnitude of circle's position from pt1 of the line (c) gives the vector in the direction of circle's center 
					float distance = Distance(mapWalls[l]->p1, mapWalls[l]->p2);
					sf::Vector2f unitLineVector = sf::Vector2f(lineVector.x / distance, lineVector.y / distance);   // (a) finds the unit vector for lineVector (b) so we can figure out how far is the circle from pt1 (c) gives the unit vector of line Vector
					float pro = Clamp(DotProduct(circle2lineVector, unitLineVector), 0.f, distance);
					if (pro > 10.f || pro < distance)
					{
						if (CheckIspossiblePath(enemyPos, AllNodes[i]))
						{
							closetNodeToEscape = AllNodes[i];
							closesNodeDistance = Distance(enemyPos, closetNodeToEscape->GetPosition());
							coverWithWall = { closetNodeToEscape->GetPosition(),mapWalls[l] };
						}
					}
				}
			}
	}
	}


	if (closetNodeToEscape != nullptr) return coverWithWall;
	else return std::make_pair(sf::Vector2f(NULL, NULL), nullptr);
}

sf::Vector2f PathFinder::FindClosestOutOfCoverPosition(sf::Vector2f coverPos,sf::Vector2f playerPos, Wall* coverWall)
{
	Node* closetNodeToEscape = nullptr;
	float closesNodeDistance = 250.f;
	for (int i = 0; i < AllNodes.size(); ++i)
	{
		if (Distance(coverPos, AllNodes[i]->GetPosition()) < closesNodeDistance && (AllNodes[i]->distToclosestWall > 35.f) && Distance(playerPos,AllNodes[i]->GetPosition()) < 250.f && AllNodes[i]->GetPosition() != coverPos)
		{
			if (!areLinesColliding(coverPos, AllNodes[i]->GetPosition(), coverWall->p1, coverWall->p2))
			{
				bool isCollidingwithwall = false;
				for (int wI = 0; wI < mapWalls.size(); wI++)
				{
					if (areLinesColliding(playerPos, AllNodes[i]->GetPosition(), mapWalls[wI]->p1, mapWalls[wI]->p2))
					{
						isCollidingwithwall = true;
					}
				}

				if (!isCollidingwithwall)
				{
					sf::Vector2f lineVector = coverWall->p2 - coverWall->p1;                                                           // (a) finds the vector for the line from pt1 to pt2  (b) so later I can find out its dot product to check how far the circle is (c) gives the vector in the direction of pos2 
					sf::Vector2f circle2lineVector = AllNodes[i]->GetPosition() - coverWall->p1;                                       // (a) finds the vector for the pt1 of line to circle's center (b) to check the magnitude of circle's position from pt1 of the line (c) gives the vector in the direction of circle's center 
					float distance = Distance(coverWall->p1, coverWall->p2);
					sf::Vector2f unitLineVector = sf::Vector2f(lineVector.x / distance, lineVector.y / distance);   // (a) finds the unit vector for lineVector (b) so we can figure out how far is the circle from pt1 (c) gives the unit vector of line Vector
					float pro = DotProduct(circle2lineVector, unitLineVector);
					if (pro < 0.f || pro > distance)
					{
						if (CheckIspossiblePath(coverPos, AllNodes[i]))
						{
							closetNodeToEscape = AllNodes[i];
							closesNodeDistance = Distance(coverPos, closetNodeToEscape->GetPosition());
						}
					}
				}
			}
		}
	}

	if(closetNodeToEscape!=nullptr) return closetNodeToEscape->GetPosition();
	else return sf::Vector2f(NULL, NULL);
}


std::array<std::array<sf::Vector2f, 2>, 4> PathFinder::CheckConnectionToNeighbours(Node *grid[32][32], std::vector<Wall*> walls)
{
	_connectingArray[0] = { grid[10][17]->GetPosition(), grid[10][18]->GetPosition() };
	_connectingArray[1] = { grid[10][17]->GetPosition(), grid[10][16]->GetPosition() };
	_connectingArray[2] = { grid[10][17]->GetPosition(), grid[9][17]->GetPosition() };
	_connectingArray[3] = { grid[10][17]->GetPosition(), grid[11][17]->GetPosition() };

	for (int i = 0; i < walls.size(); i++)
	{
		if (areLinesColliding(grid[10][17]->GetPosition(), grid[10][18]->GetPosition(), walls[i]->p1, walls[i]->p2))
		{
			_connectingArray[0] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
		}

		if (areLinesColliding(grid[10][17]->GetPosition(), grid[10][16]->GetPosition(), walls[i]->p1, walls[i]->p2))
		{
			_connectingArray[1] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
		}

		if (areLinesColliding(grid[10][17]->GetPosition(), grid[9][17]->GetPosition(), walls[i]->p1, walls[i]->p2))
		{
			_connectingArray[2] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
		}

		if (areLinesColliding(grid[10][17]->GetPosition(), grid[11][17]->GetPosition(), walls[i]->p1, walls[i]->p2))
		{
			_connectingArray[3] = { sf::Vector2f(NULL,NULL),sf::Vector2f(NULL,NULL) };
		}
	}

	return _connectingArray;
}

std::vector<sf::Vector2f> PathFinder::CheckPath(sf::Vector2f start, sf::Vector2f end)
{
	std::vector<Node*> closeNnodesToStart, closeNnodesToEnd;

	/*for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < row; j++)
		{
			_grid[i][j]->Reset();
		}
	}*/

	for (int i = 0; i < AllNodes.size(); i++)
	{
		AllNodes[i]->Reset();
	}

	for (int i = 0; i < AllNodes.size(); i++)
	{
		if (Distance(start, AllNodes[i]->GetPosition()) < 40.0f && AllNodes[i]->distToclosestWall > 25.f)
		{
			closeNnodesToStart.push_back(AllNodes[i]);
		}
		if (Distance(end, AllNodes[i]->GetPosition()) < 40.0f  && AllNodes[i]->distToclosestWall > 25.f)
		{
			closeNnodesToEnd.push_back(AllNodes[i]);
		}
	}

	/*for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < row; j++)
		{
			if (Distance(start, _grid[i][j]->GetPosition()) < 40.0f && _grid[i][j]->distToclosestWall > 25.f)
			{
				closeNnodesToStart.push_back(_grid[i][j]);
			}

			if (Distance(end, _grid[i][j]->GetPosition()) < 40.0f  && _grid[i][j]->distToclosestWall > 25.f)
			{
				closeNnodesToEnd.push_back(_grid[i][j]);
			}
		}
	}*/

	for (int i = 0; i < closeNnodesToStart.size(); i)
	{
		for (int l = 0; l < mapWalls.size(); l++)
		{
			if (areLinesColliding(closeNnodesToStart[i]->GetPosition(), start, mapWalls[l]->p1, mapWalls[l]->p2))
			{
				closeNnodesToStart.erase(closeNnodesToStart.begin() + i);
				break;
			}
		}
		i++;
	}

	for (int i = 0; i < closeNnodesToEnd.size(); i++)
	{
		for (int l = 0; l < mapWalls.size(); l++)
		{
			if (areLinesColliding(closeNnodesToEnd[i]->GetPosition(), end, mapWalls[l]->p1, mapWalls[l]->p2))
			{
				closeNnodesToEnd.erase(closeNnodesToEnd.begin() + i);
				break;
			}
		}
		i++;
	}
	
	if (closeNnodesToStart.size() > 0)
	{
		float shortestDistance = Distance(start, closeNnodesToStart[0]->GetPosition());

		for (int i = 1; i < closeNnodesToStart.size(); i++)
		{
			float curDist = Distance(start, closeNnodesToStart[i]->GetPosition());
			if (curDist < shortestDistance)
			{
				std::swap(closeNnodesToStart[0], closeNnodesToStart[i]);
				shortestDistance = curDist;
			}
		}

	}

	if (closeNnodesToEnd.size() > 0)
	{
		float shortestDistance = Distance(start, closeNnodesToEnd[0]->GetPosition());

		for (int i = 1; i < closeNnodesToEnd.size(); i++)
		{
			float curDist = Distance(start, closeNnodesToEnd[i]->GetPosition());
			if (curDist < shortestDistance)
			{
				std::swap(closeNnodesToEnd[0], closeNnodesToEnd[i]);
				shortestDistance = curDist;
			}
		}

	}

	return TraverseNodesByDistance(closeNnodesToStart[0], closeNnodesToEnd[0]);
}



bool PathFinder::CheckIspossiblePath(sf::Vector2f start, Node* end)
{
	std::vector<Node*> closeNnodesToStart;


	for (int i = 0; i < AllNodes.size(); i++)
	{
		AllNodes[i]->Reset();
	}

	for (int i = 0; i < AllNodes.size(); i++)
	{
		if (Distance(start, AllNodes[i]->GetPosition()) < 50.0f && AllNodes[i]->distToclosestWall > 25.f)
		{
			closeNnodesToStart.push_back(AllNodes[i]);
		}
	}


	for (int i = 0; i < closeNnodesToStart.size(); i)
	{
		for (int l = 0; l < mapWalls.size(); l++)
		{
			if (areLinesColliding(closeNnodesToStart[i]->GetPosition(), start, mapWalls[l]->p1, mapWalls[l]->p2))
			{
				closeNnodesToStart.erase(closeNnodesToStart.begin() + i);
				break;
			}
		}
		i++;
	}

	if (closeNnodesToStart.size() > 0)
	{
		float shortestDistance = Distance(start, closeNnodesToStart[0]->GetPosition());

		for (int i = 1; i < closeNnodesToStart.size(); i++)
		{
			float curDist = Distance(start, closeNnodesToStart[i]->GetPosition());
			if (curDist < shortestDistance)
			{
				std::swap(closeNnodesToStart[0], closeNnodesToStart[i]);
				shortestDistance = curDist;
			}
		}

	}

	return TraverseNodesByIteration(closeNnodesToStart[0], end);
}

int PathFinder::TraverseNodes(Node* startNode, Node* EndNode)
{
	startNode->isChecked = true;

	if (startNode == EndNode)
	{
		std::cout << "ReachedDest";
		startNode->SetColor();
		return 1;
	}

	else
	{
		for (int i = 0; i < startNode->GetNeighbours().size(); i++)
		{
			if (!startNode->GetNeighbours().at(i)->isChecked && (startNode->_connectingArray[i][0] != sf::Vector2f(NULL, NULL) || (startNode->DoorAtwhatIndex.first == i)))
			{
				b = TraverseNodes(startNode->GetNeighbours().at(i), EndNode);
				if (b == 1)
				{
					startNode->SetColor();
					return b;
				}
			}
		}
		return 0;
	}
}

sf::Vector2f PathFinder::Findpointnearby(sf::Vector2f curPos)
{
	int k = rand();
	if (k % 2 == 0)
	{
		for (int i = 0; i < AllNodes.size(); ++i)
		{
			if (Distance(curPos, AllNodes[i]->GetPosition()) < 200.f && Distance(curPos, AllNodes[i]->GetPosition()) > 30.f && AllNodes[i]->distToclosestWall > 25.f)
			{
				bool ispointColliding = false;
				for (int l = 0; l < mapWalls.size(); ++l)
				{
					if (areLinesColliding(curPos, AllNodes[i]->GetPosition(), mapWalls[l]->p1, mapWalls[l]->p2))
					{
						ispointColliding = true;

					}
				}

				for (int l = 0; l < mapDoors.size(); ++l)
				{
					if (areLinesColliding(curPos, AllNodes[i]->GetPosition(), mapDoors[l]->p1, mapDoors[l]->p2))
					{
						ispointColliding = true;

					}
				}

				for (int l = 0; l < mapWindows.size(); ++l)
				{
					if (areLinesColliding(curPos, AllNodes[i]->GetPosition(), mapWindows[l]->p1, mapWindows[l]->p2))
					{
						ispointColliding = true;

					}
				}

				if (!ispointColliding)
				{
					return AllNodes[i]->GetPosition();
				}
			}
		}
	}

	else
	{
		for (int i = AllNodes.size() - 1; i > 0; --i)
		{
			if (Distance(curPos, AllNodes[i]->GetPosition()) < 200.f && Distance(curPos, AllNodes[i]->GetPosition()) > 30.f && AllNodes[i]->distToclosestWall > 25.f)
			{
				bool ispointColliding = false;
				for (int l = 0; l < mapWalls.size(); ++l)
				{
					if (areLinesColliding(curPos, AllNodes[i]->GetPosition(), mapWalls[l]->p1, mapWalls[l]->p2))
					{
						ispointColliding = true;

					}
				}

				for (int l = 0; l < mapDoors.size(); ++l)
				{
					if (areLinesColliding(curPos, AllNodes[i]->GetPosition(), mapDoors[l]->p1, mapDoors[l]->p2))
					{
						ispointColliding = true;

					}
				}

				for (int l = 0; l < mapWindows.size(); ++l)
				{
					if (areLinesColliding(curPos, AllNodes[i]->GetPosition(), mapWindows[l]->p1, mapWindows[l]->p2))
					{
						ispointColliding = true;

					}
				}

				if (!ispointColliding)
				{
					return AllNodes[i]->GetPosition();
				}
			}
		}
	}

	return sf::Vector2f(0, 0);
}


int PathFinder::TraverseNodesByIteration(Node* startNode, Node* EndNode)
{
	startNode->distFromStart = 0;
	_iterativeNodesMap.insert({ Distance(startNode->GetPosition(),EndNode->GetPosition()),startNode });
	std::multimap<double, Node*>::iterator it = _iterativeNodesMap.begin();

	while (!_iterativeNodesMap.empty())
	{
		Node* curNode = _iterativeNodesMap.begin()->second;
		_iterativeNodesMap.erase(_iterativeNodesMap.begin());
		curNode->isChecked = true;
		if (curNode == EndNode)
		{
			
			return true;
		}

		else
		{
			for (int i = 0; i < curNode->GetNeighbours().size(); i++)
			{
				if (!curNode->GetNeighbours().at(i)->isChecked && (curNode->GetNeighbours().at(i)->distToclosestWall > 20.f) && (curNode->_connectingArray[i][0] != sf::Vector2f(NULL, NULL) || ((curNode->DoorAtwhatIndex.first == i) && (curNode->DoorAtwhatIndex.second->isOpen == false))) && !curNode->GetNeighbours().at(i)->isAdded)
				{
					curNode->GetNeighbours().at(i)->pointingNode = curNode;
					curNode->GetNeighbours().at(i)->isAdded = true;
					curNode->GetNeighbours().at(i)->distFromStart = curNode->GetNeighbours().at(i)->pointingNode->distFromStart + distanceBWNodes;
					if (curNode->GetNeighbours().at(i)->distFromStart < 200.f)_iterativeNodesMap.insert({ curNode->GetNeighbours().at(i)->distFromStart,curNode->GetNeighbours().at(i) });
				}
			}
		}
	}

	return false;
}


std::vector<sf::Vector2f> PathFinder::TraverseNodesByDistance(Node* startNode, Node* EndNode)
{
	_pathPool.clear();
	startNode->distFromStart = 0;
	_iterativeNodesMap.insert({ Distance(startNode->GetPosition(),EndNode->GetPosition()),startNode });
	std::multimap<double, Node*>::iterator it = _iterativeNodesMap.begin();
	it->second->distanceFromEnd = Distance(startNode->GetPosition(), EndNode->GetPosition());

	while (!_iterativeNodesMap.empty())
	{
		Node* curNode = _iterativeNodesMap.begin()->second;
		_iterativeNodesMap.erase(_iterativeNodesMap.begin());
		curNode->isChecked = true;
		if (curNode == EndNode)
		{
			std::cout << "ReachedDest";
			curNode->SetColor();
			while (curNode->pointingNode != nullptr)
			{
				curNode->SetColor();
				curNode->pathNodePos = curNode->pointingNode->GetPosition();
				_pathPool.push_back(curNode);
				curNode = curNode->pointingNode;
			}

		}

		else
		{
			for (int i = 0; i < curNode->GetNeighbours().size(); i++)
			{
				if (!curNode->GetNeighbours().at(i)->isChecked && (curNode->GetNeighbours().at(i)->distToclosestWall > 20.f) && (curNode->_connectingArray[i][0] != sf::Vector2f(NULL, NULL) || ((curNode->DoorAtwhatIndex.first == i) && (curNode->DoorAtwhatIndex.second->isOpen == false))) && !curNode->GetNeighbours().at(i)->isAdded)
				{
					curNode->GetNeighbours().at(i)->pointingNode = curNode;
					curNode->GetNeighbours().at(i)->isAdded = true;
					curNode->GetNeighbours().at(i)->distFromStart = curNode->GetNeighbours().at(i)->pointingNode->distFromStart + distanceBWNodes;
					curNode->GetNeighbours().at(i)->distanceFromEnd = Distance(curNode->GetPosition(), EndNode->GetPosition());
					curNode->GetNeighbours().at(i)->totalDistance = curNode->GetNeighbours().at(i)->distFromStart + curNode->GetNeighbours().at(i)->distanceFromEnd;
					_iterativeNodesMap.insert({ curNode->GetNeighbours().at(i)->totalDistance,curNode->GetNeighbours().at(i) });
				}
			}
		}
	}

	std::vector<sf::Vector2f> _pathPosVector;

	for (int i = 0; i < _pathPool.size(); i++)
	{
		_pathPosVector.push_back(_pathPool[i]->GetPosition());
	}

	return _pathPosVector;
}

bool PathFinder::CheckIfLineBetweenColliding(sf::Vector2f posIni, sf::Vector2f posFina)
{
	for (int wI = 0; wI < mapWalls.size(); wI++)
	{
		if (areLinesColliding(posIni, posFina, mapWalls[wI]->p1, mapWalls[wI]->p2))
		{
			return true;
		}
	}

	for (int wI = 0; wI < mapDoors.size(); wI++)
	{
		if (areLinesColliding(posIni, posFina, mapDoors[wI]->p1, mapDoors[wI]->p2))
		{
			return true;
		}
	}

	return false;
}




/*auto sortNodeWithDist = [EndNode](Node* s1, Node *s2) -> bool
{
	return Distance(s1->GetPosition(), EndNode->GetPosition()) < Distance(s2->GetPosition(), EndNode->GetPosition());
};

std::sort(shortestDistNodes.begin(), shortestDistNodes.end(), sortNodeWithDist);

for (int i = 0; i < shortestDistNodes.size(); i++)
{
	_iterativeNode.push_back(shortestDistNodes[i]);
}*/

//std::sort(_iterativeNode.begin(), _iterativeNode.end(), sortNodeWithDist);