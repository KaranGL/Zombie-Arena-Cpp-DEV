#pragma once
#ifndef TEXTURE_HOLDER_H
#define TEXTURE_HOLDER_H

#include <SFML/Graphics.hpp>
#include <map>

using namespace sf;
using namespace std;

class TextureHolder {
	private:
		//a map container which holds pairs of string and texture values
		std::map<std::string, Texture> mTexture;

		//pointer of same type as the class itself, the one and only instance
		static TextureHolder* msInstance;

	public:
		TextureHolder();
		static Texture& getTexture(std::string const& filename);
};
#endif