#pragma once
#include<SFML/Graphics.hpp>

using namespace sf;

class Zombie {
	private:
		//Speed for types of Zombies
		const float BLOATER_SPEED = 40;
		const float CHASER_SPEED = 80;
		const float CRAWLER_SPEEED = 20;

		//ZOMBIE health
		const float BLOATER_HEALTH = 5;
		const float CHASER_HEALTH = 1;
		const float CRAWLER_HEALTH = 3;

		//changing each zombie's speed slightly
		const int MAX_VARIANCE = 30;
		const int OFFSET = 101 - MAX_VARIANCE;

		//Zombie position
		Vector2f m_position;

		//A sprite for the zombie
		Sprite m_sprite;

		//zombie speed
		float m_speed;

		//zombie health
		float m_health;

		//Alive or dead
		bool m_alive;

	public:
		//bullet hits zombie
		bool hit();

		//if the zombie is alive or dead
		bool isAlive();

		//spawn a new zombie
		void spawn(float startX, float startY, int type, int seed);

		//return a rectangle that is the position in the world
		FloatRect getPosition();

		//get a copy of the sprite to draw
		Sprite getSprite();

		//update the zombie each frame
		void update(float elapsedTime, Vector2f playerLocation);
};