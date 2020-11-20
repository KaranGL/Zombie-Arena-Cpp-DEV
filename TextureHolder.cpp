#include <SFML/Graphics.hpp>
#include "TextureHolder.h"
#include <assert.h>

TextureHolder* TextureHolder::msInstance = nullptr;

TextureHolder::TextureHolder() {
	assert(msInstance == nullptr);
	msInstance = this;
}

sf::Texture& TextureHolder::getTexture(std::string const& filename) {
	//get a reference to mTexture using msInstance
	auto& m = msInstance->mTexture;				  //auto is equivalent to map<string, texture>
	
	//Iterator to hold key value pair aka KVP, and search for the required kvp using the passed in file name
	auto keyValuePair = m.find(filename);		 //auto is equivalent to map<string, Texture>::iterator

	//finding a match
	if (keyValuePair != m.end()) {
		//return the texture, the second part of kvp is the texture
		return keyValuePair->second;
	}
	else { //File name not found in the map
		//Create a new key value pair using the filename
		auto& texture = m[filename];

		//Load the texture from file in the usual way
		texture.loadFromFile(filename);
		
		return texture;
	}
}