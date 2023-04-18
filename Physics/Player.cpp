#include"Player.h"
#include<SFML/Window/Mouse.hpp>
#include"MathFunc.h"

void Player::Move(MoveDirection md)
{
	switch (md)
	{
	case MoveDirection::LEFTSIDE:
		circle.setPosition(circle.getPosition().x - 4, circle.getPosition().y);
		p1 = circle.getPosition();
		p2 = sf::Vector2f(p2.x - 4, p2.y);
		break;
	case MoveDirection::RIGHTSIDE:
		circle.setPosition(circle.getPosition().x + 4, circle.getPosition().y);
		p1 = circle.getPosition();
		p2 = sf::Vector2f(p2.x + 4, p2.y);
		break;
	case MoveDirection::UPSIDE:
		circle.setPosition(circle.getPosition().x, circle.getPosition().y - 4);
		p1 = circle.getPosition();
		p2 = sf::Vector2f(p2.x, p2.y - 4);
		break;
	case MoveDirection::DOWNSIDE:
		circle.setPosition(circle.getPosition().x, circle.getPosition().y + 4);
		p1 = circle.getPosition();
		p2 = sf::Vector2f(p2.x, p2.y + 4);
		break;
	default:
		break;
	}
}

Player::Player(GameDataRef data) :_data(data), p1(0, 0), p2(10, 10), nearByItem(nullptr) {
	circle.setRadius(20);
	circle.setOrigin(20, 20);
	circle.setFillColor(sf::Color::Green);
	SetDirection(sf::Vector2f(NULL, NULL));
	SeetHealthBarSize();
	healthrect.setOrigin(sf::Vector2f(healthrect.getSize().x / 2, 2.5f));
	healthrect.setFillColor(sf::Color::Red);
	playerHandler = new PlayerHandler();
	playerHandler->parent = this;
};

void Player::Rotate(float angle)
{
	circle.setRotation(angle);
}

void Player::Draw()
{
	_data->window.draw(circle);
	_data->window.draw(healthrect);
	sf::Vertex line[] =
	{
		sf::Vertex(p1),
		sf::Vertex(p2)
	};
	_data->window.draw(line, 2, sf::Lines);

	if (_items.size() > 0 && _items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
	{
		_items[0]->Draw();
	}

	if (_items.size() > 0 && _items[0]->CheckTrait(ItemTrait::EXPLODESONHIT))
	{
		sf::Vertex ItemTarget[] =
		{
			sf::Vertex(itemLauncher.p1),
			sf::Vertex(itemLauncher.p2)
		};
		ItemTarget->color = sf::Color::Magenta;
		_data->window.draw(ItemTarget, 2, sf::Lines);
	}
}

void Player::ReduceHealthOfPlayer()
{
 if (health > 0) health--;
 else Kill();
 SetHealthBarSize();
}

void Player::SetHealthBarSize()
{
	healthrect.setSize(sf::Vector2f(health*30.f, 8.f));
}

void Player::Update(float dt)
{
	mousPos = (sf::Vector2f)sf::Mouse::getPosition(_data->window);
	sf::Vector2f playertoMouse = UnitVector(mousPos - this->p1);
	sf::Vector2f p1Top2 = this->p2 - this->p1;
	this->p2 = Line2Mouse(this->p1, playertoMouse);
	if (_items.size() > 0 && _items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
	{
		_items[0]->SetPosition(GetPosition());
		_items[0]->_targetLine.p1 = p1;
		sf::Vector2f playerToTargetPt = UnitVector(p2 - _items[0]->_targetLine.p1);
		sf::Vector2f newPoint;
		newPoint = p1 + playerToTargetPt * 300.f;
		_items[0]->_targetLine.p2 = newPoint;
		_items[0]->_targetLine.p3 = newPoint;
		_items[0]->_targetLine.p1 = p1;
	}

	else if (_items.size() > 0 && _items[0]->CheckTrait(ItemTrait::EXPLODESONHIT))
	{
		_items[0]->SetPosition(GetPosition());
		itemLauncher.p1 = p1;
		sf::Vector2f playerToTargetPt = UnitVector(p2 - itemLauncher.p1);
		sf::Vector2f newPoint;
		newPoint = p1 + playerToTargetPt * targetLength;
		itemLauncher.p2 = newPoint;
		_items[0]->targetPos = itemLauncher.p2;
	}

}

void Player::Kill()
{
	circle.setFillColor(sf::Color::Magenta);
	isdead = true;
}