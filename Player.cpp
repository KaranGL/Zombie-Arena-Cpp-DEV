#include <SFML/Graphics.hpp>
#include "Player.h"
#include "TextureHolder.h"

Player::Player() {
	pSpeed = STARTING_SPEED;
	pHealth = STARTING_HEALTH;
	pMaxHealth = STARTING_HEALTH;

	pSprite = Sprite(TextureHolder::getTexture("graphics/player.png"));

	//Setting origin sprite to center for smooth rotation
	pSprite.setOrigin(25,25);
}

void Player::spawn(IntRect arena, Vector2f resolution, int tileSize) {
	// Place the player in the middle of the arena, regardless the size of the arena
	pPosition.x = arena.width / 2;
	pPosition.y = arena.height / 2;

	// Copy the details of the arena to the player's m_Arena
	pArena.left = arena.left;
	pArena.width = arena.width;
	pArena.top = arena.top;
	pArena.height = arena.height;

	// Remember how big the tiles are in this arena
	pTileSize = tileSize;

	// Strore the resolution for future use
	pResolution.x = resolution.x;
	pResolution.y = resolution.y;
}

void Player::resetPlayerStats()
{
	pSpeed = STARTING_SPEED;
	pHealth = STARTING_HEALTH;
	pMaxHealth = STARTING_HEALTH;
}

Time Player::getLastHitTime() {
	return pLastHit;
}

bool Player::hit(Time timeHit) {
	if (timeHit.asMilliseconds() - pLastHit.asMilliseconds() > 200)// 2 tenths of second
	{
		pLastHit = timeHit;
		pHealth -= 10;
		return true;
	}
	else
	{
		return false;
	}
}

FloatRect Player::getPosition() {
	return pSprite.getGlobalBounds();
}

Vector2f Player::getCenter() {
	return pPosition;
}

float Player::getRotation() {
	return pSprite.getRotation();
}

Sprite Player::getSprite() {
	return pSprite;
}

int Player::getHealth() {
	return pHealth;
}

void Player::moveLeft() {
	pLEFT = true;
}

void Player::moveRight() {
	pRIGHT = true;
}

void Player::moveUp() {
	pUP = true;
}

void Player::moveDown() {
	pDOWN = true;
}

void Player::stopLeft() {
	pLEFT = false;
}

void Player::stopRight() {
	pRIGHT = false;
}

void Player::stopUp() {
	pUP = false;
}

void Player::stopDown() {
	pDOWN = false;
}

void Player::update(float elapsedTime, Vector2i mousePosition) {
	if (pUP) {
		pPosition.y -= pSpeed * elapsedTime;
	}

	if (pDOWN) {
		pPosition.y += pSpeed * elapsedTime;
	}

	if (pRIGHT) {
		pPosition.x += pSpeed * elapsedTime;
	}

	if (pLEFT) {
		pPosition.x -= pSpeed * elapsedTime;
	}

	pSprite.setPosition(pPosition);

	// Keep the player in the arena || checking if the player graphic is being displayed beyond the arena size
	if (pPosition.x > pArena.width - pTileSize) {
		pPosition.x = pArena.width - pTileSize;
	}

	if (pPosition.x < pArena.left +pTileSize) {
		pPosition.x = pArena.left + pTileSize;
	}

	if (pPosition.y > pArena.height - pTileSize) {
		pPosition.y = pArena.height - pTileSize;
	}

	if (pPosition.y < pArena.top + pTileSize) {
		pPosition.y = pArena.top + pTileSize;
	}

	// Calculate the angle the player is facing
	float angle = (atan2(mousePosition.y - pResolution.y / 2, mousePosition.x - pResolution.x / 2) * 180) / 3.141;

	pSprite.setRotation(angle);
}

void Player::upgradeSpeed()
{
	// 20% speed upgrade
	pSpeed += (STARTING_SPEED * .2);
}

void Player::upgradeHealth()
{
	// 20% max health upgrade
	pMaxHealth += (STARTING_HEALTH * .2);

}

void Player::incHealthLevel(int amount)
{
	pHealth += amount;

	// But not beyond the maximum
	if (pHealth > pMaxHealth)
	{
		pHealth = pMaxHealth;
	}
}