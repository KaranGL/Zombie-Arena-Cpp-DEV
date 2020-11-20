#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

class Player {
	private:
		const float STARTING_SPEED = 200;
		const float STARTING_HEALTH = 100;

		//Player position
		Vector2f pPosition;

		Sprite pSprite;

		Texture pPlayer;

		//Screen resolution
		Vector2f pResolution;

		//size of current arena
		IntRect pArena;

		//tile size in the arena
		int pTileSize;

		//Available player directions
		bool pUP, pDOWN, pLEFT, pRIGHT;

		//Player's health
		int pHealth;

		//player's max health
		int pMaxHealth;

		//When was the player last hit
		Time pLastHit;

		//Speed in pixels per second
		float pSpeed;

	public:
		Player();

		void spawn(IntRect arena, Vector2f resolution, int tileSize);

		//call this at the end of the game every game
		void resetPlayerStats();

		//Player getting hit by a zombie
		bool hit(Time timeHit);

		//Player last hit by a zombie
		Time getLastHitTime();

		//player position
		FloatRect getPosition();

		//player position origin
		Vector2f getCenter();

		//Player facing angle
		float getRotation();

		//send a copy of the sprite to main function
		Sprite getSprite();

		//Player's moving functionality
		void moveLeft();
		void moveRight();
		void moveUp();
		void moveDown();

		//stop the player moving in a specific direction
		void stopLeft();
		void stopRight();
		void stopUp();
		void stopDown();
		
		//call this function once every frame
		void update(float elapsedTime, Vector2i mousePosition);

		//Speed boost
		void upgradeSpeed();

		//health boost
		void upgradeHealth();

		//Increase the max health of player
		void incHealthLevel(int amount);

		//player's current health
		int getHealth();
};