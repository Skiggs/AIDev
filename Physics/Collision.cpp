#include"Collision.h"

Collision::Collision()
{

}

bool Collision::CheckCollision(sf::FloatRect tcir, sf::FloatRect tfloor)
{
	sf::Rect<float> rect1 = tcir;
	sf::Rect<float> rect2 = tfloor;

	if (rect1.intersects(rect2))
	{
		return true;
	}

	else
	{
		return false;
	}
}