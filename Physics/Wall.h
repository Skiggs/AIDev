#pragma once

#include"Game.h"
#include<SFML/Graphics.hpp>

class Wall
{
public:
	Wall(GameDataRef data) : _data(data) {}
	~Wall();
	sf::Vector2f p1, p2;
	void Draw() {
		sf::Vertex line[] =
		{
			sf::Vertex(p1),
			sf::Vertex(p2)
		}; 
		_data->window.draw(line, 2, sf::Lines); 
	}
	void SetLinePoints(sf::Vector2f pos1, sf::Vector2f pos2) { p1 = pos1; p2 = pos2; }
	void Update(float dt) {};
private:
	GameDataRef _data;
};

class Window
{
public:
	Window(GameDataRef data) : _data(data) {}
	~Window();
	sf::Vector2f p1, p2;
	void Draw() {
		sf::Vertex line[] =
		{
			sf::Vertex(p1),
			sf::Vertex(p2)
		};
		line->color = sf::Color::Red;
		_data->window.draw(line, 2, sf::Lines);
	}
	void SetLinePoints(sf::Vector2f pos1, sf::Vector2f pos2) { p1 = pos1; p2 = pos2; }
	void Update(float dt) {};
private:
	GameDataRef _data;
};

class Door
{
public:
	Door(GameDataRef data) : _data(data) {}
	~Door() {};
	bool isOpen = false;
	sf::Vector2f p1, p2;
	void Draw() {
		sf::Vertex line[] =
		{
			sf::Vertex(p1),
			sf::Vertex(p2)
		};
		line->color = sf::Color::Green;
		_data->window.draw(line, 2, sf::Lines);
	}
	float minX = 0, maxX = 0, minY = 0, maxY = 0;
	void SetLinePoints(sf::Vector2f pos1, sf::Vector2f pos2) { p1 = pos1; p2 = pos2; }
	void Update(float dt) {};
private:
	GameDataRef _data;
};


class RectWall
{
public:
	RectWall(GameDataRef data) : _data(data) 
	{
		_rect.setOrigin(sf::Vector2f(175, 25));
		_rect.setSize(sf::Vector2f(350, 50));
		_rect.setFillColor(sf::Color::Blue);
	}
	~RectWall() {};
	sf::Vector2f p1, p2;
	void Draw() 
	{
		_data->window.draw(_rect);
	}

	void SetPosition(sf::Vector2f pos) { _rect.setPosition(pos);}
	void Update(float dt) {};
private:
	GameDataRef _data;
	sf::RectangleShape _rect;
};