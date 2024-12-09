#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>


class Player {
public:
	Player();

	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

	void updateAnimationMovement();

	void updateAccelleration(sf::Time deltaTime);

	void updateDeceleration(sf::Time deltaTime);

	void updateMovement(sf::Time deltaTime);

	void move(sf::Time deltaTime);

	sf::Sprite& getSprite() { return mPlayer; }

	sf::Vector2f getPosition() const { return mPlayer.getPosition(); }

private:
	sf::Sprite mPlayer;

	sf::Texture mPlayerTexture;

	float movementSpeed;
	float maxSpeedForward;
	float maxSpeedBackwards;

	bool pIsMovingUp;
	bool pIsMovingDown;
	bool pIsMovingLeft;
	bool pIsMovingRight;

	int currentFrame;
	int frameCounter;

	sf::Vector2f movement;
	sf::Vector2f rotatedMovement;
};