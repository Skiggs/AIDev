
#include<SFML/Graphics.hpp>
#include"MathFunc.h"
#include<array>
#include"Node.h"
#include"Wall.h"
#include<map>



class PathFinder
{
private:
	static const int row = 50;
	static const int col = 50;
	GameDataRef _data;
	static const int distanceBWNodes = 22.0f;
	static Node* _grid[col][row];
	std::array<std::array<sf::Vector2f, 2>, 4> _connectingArray;
	static std::vector<Wall*> mapWalls;
	static std::vector<Door*> mapDoors;
	static std::vector<Window*> mapWindows;
	static std::vector<Door*> CollidingDoors;
	static std::vector<Window*> CollidingWindows;
	int num = 0;	
public:
	 PathFinder(GameDataRef data): _data(data)
	{
		for (int i = 0; i < col; i++)
		{
			for (int j = 0; j < row; j++)
			{
				Node* nde = new Node(_data);
				nde->id = num++;
				nde->SetPosition(sf::Vector2f(j * distanceBWNodes, i * distanceBWNodes));
				_grid[i][j] = nde;
			}
		}
	}

	static void Init(std::vector<Wall*>, std::vector<Door*>, std::vector<Window*>);
	static void Draw();
	static std::pair<sf::Vector2f, Wall*> GetPositionBehindCover(sf::Vector2f playerPos, sf::Vector2f enemyPos);
	static void UpdateNodesInRectangle(float rectPoint[4]);
	static sf::Vector2f FindUnaffectedPointByGrenade(sf::Vector2f grenadePos, sf::Vector2f enemyPos);
	static sf::Vector2f FindClosestOutOfCoverPosition(sf::Vector2f coverPos, sf::Vector2f, Wall*);
	static sf::Vector2f Findpointnearby(sf::Vector2f point);
	void Update(float dt);
	~PathFinder() {}


	std::array<std::array<sf::Vector2f, 2>, 4> CheckConnectionToNeighbours (Node *grid[32][32],std::vector<Wall*>);
	int b = 0;
	static void ConnectLinesToNeighbours();

	int TraverseNodes(Node* startNode, Node* EndNode);
	static int TraverseNodesByIteration(Node* startNode, Node* EndNode);
	//int TraverseNodesByIteration(Node* startNode, Node* EndNode);
	static std::vector<sf::Vector2f> TraverseNodesByDistance(Node* startNode, Node* EndNode);
	static std::vector<Node*> AllNodes;
	static std::vector<Node*> _iterativeNode;
	static std::vector<Node*> _pathPool;
	static std::multimap<double,Node*> _iterativeNodesMap;
	int _pathpoolIndex = 0;
	static std::vector<sf::Vector2f> CheckPath(sf::Vector2f,sf::Vector2f);
	static bool CheckIspossiblePath(sf::Vector2f, Node*);
    static bool CheckIfLineBetweenColliding(sf::Vector2f posIni, sf::Vector2f posFina);
};