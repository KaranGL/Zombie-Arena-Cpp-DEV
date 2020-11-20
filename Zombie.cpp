#include "Zombie.h"
#include "TextureHolder.h"
#include <cstdlib>
#include <ctime>

using namespace std;

void Zombie::spawn(float startX, float startY, int type, int seed) {
	switch (type) {
	case 0:
		//Bloater
		m_sprite = Sprite(TextureHolder::getTexture("graphics/bloater.png"));
		m_speed = 40;
		m_health = 5;
		break;
	case 1:
		//Chaser
		m_sprite = Sprite(TextureHolder::getTexture("graphics/chaser.png"));
		m_speed = 70;
		m_health = 1;
		break;
	case 2:
		//Crawler
		m_sprite = Sprite(TextureHolder::getTexture("graphics/crawler.png"));
		m_speed = 20;
		m_health = 3;
		break;
	}

	//Modify the speed to make the zombie unique
	//Every zombie is unique. Create a speed modifier
	srand((int)time(0) * seed);
	
	//Somewhere between 80 an 100
	float modifier = (rand() % MAX_VARIANCE) + OFFSET;
	
	//Express as a fraction of 1
	modifier /= 100; //Now equals between .7 and 1
	m_speed *= modifier;

	m_position.x = startX;
	m_position.y = startY;

	m_sprite.setOrigin(25, 25);
	m_sprite.setPosition(m_position);
}

bool Zombie::hit() {
	m_health--;
	if (m_health < 0) {
		//dead
		m_alive = false;
		m_sprite.setTexture(TextureHolder::getTexture("graphics/blood.png"));

		return true;
	}

	//not dead, only injured
	return false;
}

bool Zombie::isAlive() {
	return m_alive;
}

FloatRect Zombie::getPosition() {
	return m_sprite.getGlobalBounds();
}

Sprite Zombie::getSprite() {
	return m_sprite;
}

void Zombie::update(float elapsedTime, Vector2f playerLocation) {
	float playerX = playerLocation.x;
	float playerY = playerLocation.y;

	//update the zombie position variables
	if (playerX > m_position.x) {
		m_position.x = m_position.x + m_speed * elapsedTime;
	}

	if (playerY > m_position.y)
	{
		m_position.y = m_position.y + m_speed * elapsedTime;
	}

	if (playerX < m_position.x)
	{
		m_position.x = m_position.x - m_speed * elapsedTime;
	}

	if (playerY < m_position.y)
	{
		m_position.y = m_position.y - m_speed * elapsedTime;
	}

	// Move the sprite
	m_sprite.setPosition(m_position);

	// Face the sprite in the correct direction
	float angle = (atan2(playerY - m_position.y, playerX - m_position.x) * 180) / 3.141;

	m_sprite.setRotation(angle);
}