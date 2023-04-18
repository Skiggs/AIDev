#pragma once
#include<string>
#include<SFML/Graphics.hpp>
#include"Game.h"
#include"ItemHandler.h"
#include<array>
#include<map>

#define ItemTraitEnum(X) X,
enum ItemTrait
{
    #include"ItemTraitsDefinitions.h"
	COUNT
};
#undef ItemTraitEnum

extern std::map<std::string, ItemTrait> EnumStringMap;

extern void MapItemEnumWithStrings();


struct Itemline
{
	sf::Vector2f p1, p2;
};

struct TargetLine
{
	sf::Vector2f p1, p2,p3;
};

class Item
{
public:
	Item(GameDataRef data) : _data(data) {

		for (int i = THROWABLE; i < COUNT; i++)
		{
			_traits[i] = false;
		}
		SetOrigin();
		itemhandler = new ItemHandler();
		itemhandler->parent = this;
	}

	~Item() {}

	void Draw() {
		_data->window.draw(m_sprite); 
		sf::Vertex line[] =
		{
			sf::Vertex(_Itemline.p1),
			sf::Vertex(_Itemline.p2)
		};
		//_data->window.draw(line, 2, sf::Lines);

		if (CheckTrait(ItemTrait::HOLDANDSHOOT) && IsPicked)
		{
			sf::Vertex targetRenderline[] =
			{
				sf::Vertex(_targetLine.p1),
				sf::Vertex(_targetLine.p3)
			};
			targetRenderline->color = sf::Color::Cyan;
			_data->window.draw(targetRenderline, 2, sf::Lines);
		}
	};


	Itemline _Itemline;
	TargetLine _targetLine;
	void Update(float dt);
	void SetOrigin() { m_sprite.setOrigin(sf::Vector2f(100.0, 130.0)); }
	void SetPosition(sf::Vector2f pos) { 
		m_sprite.setPosition(pos);
		curPos = pos;
	}
	sf::Vector2f GetPosition() { return m_sprite.getPosition(); }
	void SetName(std::string str) { m_name = str; }
	void SetTraits(std::array<bool, COUNT> trt) { _traits = trt; }
	void SetTexture(std::string texName) {
		m_sprite.setTexture(_data->asset.GetTexture(texName)); 
	}
	void SetDirection(sf::Vector2f dir) { acceleration = dir; }
	void SetID(int id) { m_id = id; }
	int GetID() const { return m_id; }
	sf::Vector2f GetDirection() { return acceleration; }
	void SetSize(sf::Vector2f size) { m_sprite.setScale(size);}
	bool CheckTrait(ItemTrait trait) { return _traits[trait]; }
	bool IsPicked = false;
	bool isThrownByEnemy = false;
	bool GonnaCapturedByotherEnemy = false;
	sf::Vector2f targetPos;
	bool isActivated = false;
	bool needToMove = false;
	bool doesntExist = false;
	ItemHandler *itemhandler;
	sf::Vector2f curPos;

private:
	GameDataRef _data;
	int m_id;
	sf::Sprite m_sprite;
	sf::Vector2f acceleration;
	std::string m_name;
	sf::Vector2f m_position;
	std::array<bool, COUNT> _traits;
};