#include"FontManager.h"

void FontManager::LoadFont(std::string name, std::string fileName)
{
	sf::Font font;

	if (font.loadFromFile(fileName))
	{
		this->_fonts[name] = font;
	}

	else
	{
		printf("Could not load font");
	}
}
sf::Font& FontManager::GetFont(std::string name)
{
	return this->_fonts.at(name);
}