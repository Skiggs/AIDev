#pragma once

#include<SFML/Graphics.hpp>
#include "Game.h"
#include<array>
#include"Wall.h"
#include<utility>

class Node
{
public:
	Node(){}
	Node(GameDataRef data) : _data(data) 
	{
		_square.setSize(sf::Vector2f(1, 1));
		_square.setOrigin(sf::Vector2f(0.5, 0.5));
		_square.setFillColor(sf::Color::Magenta);
	}
	~Node() {}
	int id = 0;
	bool isChecked = false;
	bool isAdded = false;
	void Update(float dt);
	void SetPosition(sf::Vector2f pos) { _square.setPosition(pos); }
	void SetColor() { _square.setFillColor(sf::Color::Yellow); }
	void SetColorMag() { _square.setFillColor(sf::Color::Magenta); }
	sf::Vector2f GetPosition() { return _square.getPosition(); }
	void SetNeighbour(Node* node) { _neighbours.push_back(node); }
	std::vector<Node*> GetNeighbours() { return _neighbours; }
	double distFromStart = 0, distanceFromEnd = 0,totalDistance=0;
	float distToclosestWall = -1.f;
	void Draw()
	{
		_data->window.draw(_square);
		for (int i = 0; i < _connectingArray.size(); i++)
		{
			sf::Vertex line[] =
			{
				sf::Vertex(_connectingArray[i][0]),
				sf::Vertex(_connectingArray[i][1])
			};
			line->color = sf::Color::Magenta;
			//_data->window.draw(line, 2, sf::Lines);
		}
		if (pathNodePos != sf::Vector2f(NULL,NULL))
		{
			sf::Vertex line2[] =
			{
				sf::Vertex(pathNodePos),
				sf::Vertex(_square.getPosition())
			};
			line2->color = sf::Color::Yellow;
			_data->window.draw(line2, 2, sf::Lines);
		}
	}

	void Reset()
	{
		isChecked = false;
		isAdded = false;
		totalDistance = 0;
		distFromStart = 0;
		distanceFromEnd = 0;
		pointingNode = nullptr;
		pathNodePos = sf::Vector2f(NULL, NULL);
        SetColorMag();
	}

	std::array<std::array<sf::Vector2f, 2>, 4> _connectingArray;
	sf::Vector2f pathNodePos = sf::Vector2f(NULL,NULL);
	std::pair<int, Door*> DoorAtwhatIndex = {-1,nullptr};
	Node* pointingNode = nullptr;
private:
	GameDataRef _data;
	std::vector<Node*> _neighbours;

	sf::RectangleShape _square;
};