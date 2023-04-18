#include"InputManager.h"

bool InputManager::IskeyPressed()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M) || sf::Keyboard::isKeyPressed(sf::Keyboard::N) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		return true;
	}

	return false;
}