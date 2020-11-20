#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "ZombieArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"
#include <sstream>
#include <fstream>

using namespace sf;

int main() {
	int score = 0;

	TextureHolder holder;

	//Four states of game. The game will be in one of these four states.
	enum class State {
		PAUSED, LEVELLING_UP, GAME_OVER, PLAYING
	};

	//Start with game over state
	State state = State::GAME_OVER;

	//Get the screen resolution and create a SFML Window.
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie ARENA", Style::Fullscreen);

	//SFMl View for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	//Clock for timing the whole game
	Clock clock;

	//PLAYING state active time.
	Time gameTimeTotal;

	//mouse position in relation to the arena coordinates
	Vector2f mouseWorldPosition;
	//mouse position in relation to the screen coordinates
	Vector2i mouseScreenPosition;

	//Create an instance of Player class.
	Player player;

	//The boundaries of the arena
	IntRect arena;

	//Create a background
	VertexArray background;

	//Load the texture for our background vertex array
	Texture texturebackground = TextureHolder::getTexture("graphics/background_sheet.png");

	int numZombies;
	int numZombiesAlive;
	Zombie* Zombie = nullptr;

	//100 bullets per game
	Bullet bullets[100];
	int currentBullet = 0;
	int bulletSpare = 28;
	int bulletsInClip = 7;
	int clipSize = 7;
	float fireRate = 1;
	//fire button last pressed time
	Time lastPressed;

	//creating a cross-hair
	window.setMouseCursorVisible(false);
	Sprite crosshairSprite;
	Texture crossHair = TextureHolder::getTexture("graphics/crosshair.png");
	crosshairSprite.setTexture(crossHair);
	crosshairSprite.setOrigin(25, 25);

	//create some pickups
	Pickup health(1);
	Pickup ammo(2);

	//home/game over screen
	Sprite KOSprite;
	Texture GameOver = TextureHolder::getTexture("graphics/background.png");
	KOSprite.setTexture(GameOver);
	KOSprite.setPosition(0, 0);

	//Create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	//Create a sprite for the ammo icon
	Sprite ammoSprite;
	Texture ammoIcon = TextureHolder::getTexture("graphics/ammo_icon.png");
	ammoSprite.setTexture(ammoIcon);
	ammoSprite.setPosition(20, 1000);

	//Load the font
	Font font;
	font.loadFromFile("fonts/zombiecontrol.ttf");

	//Paused
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setFillColor(Color::Red);
	pausedText.setPosition(500, 400);
	pausedText.setString("Press Enter \nto continue");

	//Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(250, 850);
	gameOverText.setString("Press Enter to play");

	//Levelling up
	Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(80);
	levelUpText.setFillColor(Color::White);
	levelUpText.setPosition(150, 250);
	std::stringstream levelUpStream;
	levelUpStream <<
		"1- Increased rate of fire" <<
		"\n2- Increased clip size(next reload)" <<
		"\n3- Increased max health" <<
		"\n4- Increased run speed" <<
		"\n5- More and better health pickups" <<
		"\n6- More and better ammo pickups";
	levelUpText.setString(levelUpStream.str());

	//Ammo
	Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(50);
	ammoText.setFillColor(Color::Red);
	ammoText.setPosition(150, 1000);

	//Score
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(50);
	scoreText.setFillColor(Color::Red);
	scoreText.setPosition(20, 0);

	//Zombies remaining
	Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(50);
	zombiesRemainingText.setFillColor(Color::Red);
	zombiesRemainingText.setPosition(1650, 1000);
	zombiesRemainingText.setString("Zombies: 0");

	//Wave number
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(50);
	waveNumberText.setFillColor(Color::Red);
	waveNumberText.setPosition(1400, 1000);
	waveNumberText.setString("Wave: 0");

	//Health bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Green);
	healthBar.setPosition(350, 1000);
	//Background for health bar
	RectangleShape back1;
	back1.setFillColor(sf::Color(0, 0, 0, 150));
	back1.setPosition(350, 1000);

	//last updation of HUD (Heads Up Display)
	int sinceLastHUDUpdate = 0;
	//How often is the HUD to be updated in counts of frames
	int frameInterval = 10;

	//hit sound
	SoundBuffer hitBuffer;
	hitBuffer.loadFromFile("sound/hit.wav");
	Sound hit;
	hit.setBuffer(hitBuffer);

	//splat sound
	SoundBuffer splatBuffer;
	splatBuffer.loadFromFile("sound/splat.wav");
	sf::Sound splat;
	splat.setBuffer(splatBuffer);

	//shoot sound
	SoundBuffer shootBuffer;
	shootBuffer.loadFromFile("sound/shoot.wav");
	Sound shoot;
	shoot.setBuffer(shootBuffer);

	//reload sound
	SoundBuffer reloadBuffer;
	reloadBuffer.loadFromFile("sound/reload.wav");
	Sound reload;
	reload.setBuffer(reloadBuffer);

	//failed sound
	SoundBuffer reloadFailedBuffer;
	reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
	Sound reloadFailed;
	reloadFailed.setBuffer(reloadFailedBuffer);

	//powerup sound
	SoundBuffer powerupBuffer;
	powerupBuffer.loadFromFile("sound/powerup.wav");
	Sound powerup;
	powerup.setBuffer(powerupBuffer);

	//pickup sound
	SoundBuffer pickupBuffer;
	pickupBuffer.loadFromFile("sound/pickup.wav");
	Sound pickup;
	pickup.setBuffer(pickupBuffer);

	//main game loop
	while (window.isOpen()) {
		//===================
		//	HANDLE INPUT
		//===================

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyPressed) {
				//Pause the game
				if (event.key.code == Keyboard::Return && state == State::PLAYING) {
					state = State::PAUSED;
				}
				else if (event.key.code == Keyboard::Return && state == State::PAUSED) {		//Restart while playing
					state = State::PLAYING;
					//Reset the clock so there isn't a frame jump
					clock.restart();
				}
				else if (event.key.code == Keyboard::Return && state == State::GAME_OVER) {
					state = State::LEVELLING_UP;
					wave = 0;
					score = 0;
					//prepare the gun and ammo for next game
					currentBullet = 0;
					bulletSpare = 24;
					bulletsInClip = 6;
					clipSize = 6;
					fireRate = 1;
					healthBar.setFillColor(Color::Green);

					//Reset the player's stats
					player.resetPlayerStats();
				}

				if (state == State::PLAYING) {
					//Reloading
					if (event.key.code == Keyboard::R) {
						if (bulletSpare > clipSize) { //when spare bullets are more than the clipsize
							if (bulletsInClip == 0) {
								bulletsInClip = clipSize;
								bulletSpare -= clipSize;
								reload.play();
							}
							else if (bulletsInClip > 0 && bulletsInClip < clipSize) {
								int bulletsTaken = clipSize - bulletsInClip;
								bulletsInClip = clipSize;
								bulletSpare -= bulletsTaken;
								reload.play();
							}
							else if (bulletsInClip == clipSize) {
								reloadFailed.play();
							}
						}
						else if (bulletSpare == clipSize) { //when spare bullets are equal to the clipSize
							if (bulletsInClip == 0) {
								bulletsInClip = clipSize;
								bulletSpare = 0;
								reload.play();
							}
							else if (bulletsInClip > 0 && bulletsInClip < clipSize) {
								int bulletsTaken = clipSize - bulletsInClip;
								bulletsInClip = clipSize;
								bulletSpare -= bulletsTaken;
								reload.play();
							}
							else if (bulletsInClip == clipSize) {
								reloadFailed.play();
							}
						}
						else {
							if (bulletsInClip == 0) {
								bulletsInClip = bulletSpare;
								bulletSpare = 0;
								reload.play();
							}
							else if (bulletsInClip > 0 && bulletsInClip < clipSize) {
								if (bulletSpare >= 4 && bulletSpare <= 6) {
									int bulletsTaken = clipSize - bulletsInClip;
									bulletsInClip = clipSize;
									bulletSpare -= bulletsTaken;
									reload.play();
								}
								else if (bulletSpare > 0 && bulletSpare < 4) {
									int bulletsTaken = clipSize - bulletsInClip;
									bulletsInClip += bulletsTaken;
									bulletSpare -= bulletsTaken;
									reload.play();
								}
								else if (bulletSpare == 0) {
									reloadFailed.play();
								}
							}
							else if (bulletsInClip == clipSize){
								reloadFailed.play();
							}
						}

						/*if (bulletSpare >= clipSize) {
							//Plenty of bullet, reload
							bulletsInClip = clipSize;
							bulletSpare -= clipSize;
							reload.play();
						}
						else if (bulletSpare > 0) {
							//only a few bullets left
							bulletsInClip = bulletSpare;
							bulletSpare = 0;
							reload.play();
						}
						else {
							reloadFailed.play();
						}*/
					}
				}
			}
		}//End of Poll Event
		
		//Handle the player quitting the game
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		//handle the controls while playing
		if (state == State::PLAYING) {
			//handle the pressing and releasing of the W S A D keys
			if (Keyboard::isKeyPressed(Keyboard::W)) {
				player.moveUp();
			}
			else {
				player.stopUp();
			}

			if (Keyboard::isKeyPressed(Keyboard::S)) {
				player.moveDown();
			}
			else {
				player.stopDown();
			}

			if (Keyboard::isKeyPressed(Keyboard::A)) {
				player.moveLeft();
			}
			else {
				player.stopLeft();
			}

			if (Keyboard::isKeyPressed(Keyboard::D)) {
				player.moveRight();
			}
			else {
				player.stopRight();
			}

			//Fire a bullet
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000/fireRate && bulletsInClip > 0) {
					//Passing the centre of the player and the centre of the of the crosshair to the shoot function
					bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);

					currentBullet++;
					if (currentBullet > 99) {
						currentBullet = 0;
					}
					lastPressed = gameTimeTotal;
					
					shoot.play();
					
					bulletsInClip--;
				}
			}//End of firing a bullet
		}//End of W A S D key handling

		//handle the levelling up state
		if (state == State::LEVELLING_UP) {
			//handle the player levelling up
			if (event.key.code == Keyboard::Num1) {
				//Increase fire rate
				fireRate++;
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num2) {
				//Increase the clip size
				clipSize += clipSize;
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num3) {
				//Increse the health
				player.upgradeHealth();
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num4) {
				//increase speed
				player.upgradeSpeed();
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num5) {
				//upgrade pickup
				health.upgrade();
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num6) {
				//upgrade pickup
				ammo.upgrade();
				state = State::PLAYING;
			}

			if (state == State::PLAYING) {
				//increase the wave number
				wave++;
				//Prepare the level
				//We will modify the next two liners later
				arena.width = 500 * wave;
				arena.height = 500 * wave;
				arena.left = 0;
				arena.top = 0;

				//passing the vertexArray by reference to the createBackground function
				int tileSize = createBackground(background, arena);

				//Spawn the player in the middle of the arena
				player.spawn(arena, resolution, tileSize);

				health.setArena(arena);
				ammo.setArena(arena);

				//create a horde of zombies
				numZombies = 5 * wave;

				//Delete the previously allocated memory, if it exists
				delete[] Zombie;
				Zombie = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;

				//play the powerup sound
				powerup.play();

				//Reset the clock to avoid frame jumps
				clock.restart();
			}
		}//End levelling up

		//================================
		//		Update the FRAME
		//================================

		if (state == State::PLAYING) {
			//Update the delta time
			Time time = clock.restart();

			//update the total game time
			gameTimeTotal += time;

			//make a decimal fraction of 1 from the time
			float timeAsSeconds = time.asSeconds();

			//mouse position
			mouseScreenPosition = Mouse::getPosition();

			//convert mouse position to world coordinates of mainview
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);
			
			//set the cross hair to mouse position
			crosshairSprite.setPosition(mouseWorldPosition);

			//update the player
			player.update(timeAsSeconds, Mouse::getPosition());
			
			//Make the note of the players new position
			Vector2f playerPosition(player.getCenter());

			//Make the view centre around the player
			mainView.setCenter(player.getCenter());

			for (int i = 0; i < numZombies; i++) {
				if (Zombie[i].isAlive()) {
					Zombie[i].update(time.asSeconds(), playerPosition);
				}
			}

			//Update the bullet in flight
			for (int i = 0; i < 100; i++) {
				if (bullets[i].isInFlight()) {
					bullets[i].update(timeAsSeconds);
				}
			}

			//update for pickups
			health.update(timeAsSeconds);
			ammo.update(timeAsSeconds);
			
			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < numZombies; j++) {
					if (bullets[i].isInFlight() && Zombie[j].isAlive()) {
						if (bullets[i].getPosition().intersects(Zombie[j].getPosition())) {
							//stop the bullet
							bullets[i].stop();

							//register the hit and check if it was a kill
							if (Zombie[j].hit()) {
								//not just a hit, but kill too
								score += 10;
								numZombiesAlive--;

								//When all zombies are dead
								if (numZombiesAlive == 0) {
									state = State::LEVELLING_UP;
								}
							}
							splat.play();
						}
					}
				}
			}//End of zombie shooting

			//zombies hitting the player
			for (int i = 0; i < numZombies; i++) {
				if (player.getPosition().intersects(Zombie[i].getPosition()) && Zombie[i].isAlive()) {
					if (player.hit(gameTimeTotal)) {
						hit.play();
					}
					if (player.getHealth() < 30) {
						healthBar.setFillColor(Color::Red);
					}

					if (player.getHealth() <= 0) {
						state = State::GAME_OVER;
					}
				}
			}//End of player being hit by zombies

			//has the player touched health pickup
			if (player.getPosition().intersects(health.getPosition()) && health.isSpawned()) {
				player.incHealthLevel(health.gotIt());
				pickup.play();
			}

			//Has the player touched ammo
			if (player.getPosition().intersects(ammo.getPosition()) && ammo.isSpawned()) {
				bulletSpare += ammo.gotIt();
				reload.play();
			}

			//Size up the health bar
			healthBar.setSize(Vector2f(player.getHealth() * 3, 50));
			back1.setSize(Vector2f(300, 50));

			//Increment the number of frames since the last HUD calculation
			sinceLastHUDUpdate++;
			//Calculate FPS every frame
			if (sinceLastHUDUpdate > frameInterval) {
				//Update the HUD Text
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssZombieAlive;
				
				//Update the ammo text
				ssAmmo << bulletsInClip << "/" << bulletSpare;
				ammoText.setString(ssAmmo.str());

				//update the score text
				ssScore << "Score: " << score;
				scoreText.setString(ssScore.str());

				//Update the wave text
				ssWave << "Wave: " << wave;
				waveNumberText.setString(ssWave.str());

				//Update the number of zombies left
				ssZombieAlive << "Zombies: " << numZombiesAlive;
				zombiesRemainingText.setString(ssZombieAlive.str());

				sinceLastHUDUpdate = 0;
			}//End of HUD update
		}//End of scene updating

		//==================================
		//			Draw the Scene
		//==================================

		if (state == State::PLAYING) {
			window.clear();

			//set the mainview to be displayed in the window and draw everything relating to it.
			window.setView(mainView);

			//Draw the background
			window.draw(background, &texturebackground);

			//draw the zombies
			for (int i = 0; i < numZombies; i++) {
				window.draw(Zombie[i].getSprite());
			}

			for (int i = 0; i < 100; i++) {
				if (bullets[i].isInFlight()) {
					window.draw(bullets[i].getShape());
				}
			}

			//Player Sprite
			window.draw(player.getSprite());

			//draw pickups when spawned
			if (ammo.isSpawned()) {
				window.draw(ammo.getSprite());
			}
			if (health.isSpawned()) {
				window.draw(health.getSprite());
			}
			//draw the cross hair
			window.draw(crosshairSprite);

			//Switch to the hud view
			window.setView(hudView);
			
			//Draw all the HUD elements
			window.draw(ammoSprite);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}

		if (state == State::LEVELLING_UP) {
			window.draw(KOSprite);
			window.draw(levelUpText);
		}

		if (state == State::PAUSED) {
			window.draw(pausedText);
		}

		if (state == State::GAME_OVER) {
			window.draw(KOSprite); 
			window.draw(gameOverText); 
			window.draw(scoreText);
		}

		window.display();

	}//End of game loop

	delete[] Zombie;

	return 0;
}