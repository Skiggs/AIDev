
#include"AssetManager.h"

void AssetManager::LoadTexture(std::string name, std::string fileName) 
{
	sf::Texture tex;

	if (tex.loadFromFile(fileName))
	{
		this->_textures[name] = tex;
	}
}

sf::Texture& AssetManager::GetTexture(std::string name)
{
	return this->_textures.at(name);
}


