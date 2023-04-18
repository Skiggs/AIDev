#pragma once

#include<map>

#include<SFML/Graphics.hpp>

class AssetManager
{

public:
	AssetManager() {};
	~AssetManager() {};

	void LoadTexture(std::string name, std::string fileName);
	sf::Texture& GetTexture(std::string name);

private:
	std::map<std::string, sf::Texture> _textures;
};