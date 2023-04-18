#pragma once

#include<map>


#include<SFML/Graphics.hpp>


class FontManager
{
public:
	void LoadFont(std::string name, std::string fileName);
	sf::Font& GetFont(std::string name);

private:
	std::map<std::string, sf::Font> _fonts;
};