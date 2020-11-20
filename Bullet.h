#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Bullet
{
	private:
		//bullet postion
		Vector2f m_Position;

		//bullet shape
		RectangleShape m_BulletShape;

		//bullet flying?
		bool m_InFlight = false;

		//bullet speed
		float m_BulletSpeed = 1000;

		//What fraction of 1 pixel does the bullet travel, 
		//Horizontally and vertically each frame?
		//These values will be derived from m_BulletSpeed
		float m_BulletDistanceX;
		float m_BulletDistanceY;

		//bullet target
		float m_XTarget;
		float m_YTarget;

		//boundaries to stop the bullet from flying forever
		float m_MaxX;
		float m_MinX;
		float m_MaxY;
		float m_MinY;

	public:
		Bullet();

		//Stop the bullet
		void stop();

		//Returns the value of m_InFlight
		bool isInFlight();

		//Launch a new bullet or spawn a new bullet
		void shoot(float startX, float startY,float xTarget, float yTarget);

		//bullet position
		FloatRect getPosition();

		//Returns the actual shape (for drawing)
		RectangleShape getShape();

		//Update the bullet each frame
		void update(float elapsedTime);
};