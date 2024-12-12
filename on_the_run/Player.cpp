#include "Player.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>

Player::Player()
	: movement(sf::Vector2f(0.f, 0.f)),
	maxSpeedForward(40.f),
	maxSpeedBackwards(10.f),
	movementSpeed(0.f),
	pIsMovingUp(false),
	pIsMovingDown(false),
	pIsMovingLeft(false),
	pIsMovingRight(false),
	currentFrame(0),
	frameCounter(0)
{
	mPlayerTexture.loadFromFile("res/player/PlayerMovingSpriteSheet.png");// Location of sprite
	// Setting player sprite position and size
	mPlayer.setTexture(mPlayerTexture);
	mPlayer.setTextureRect(sf::IntRect(0, 0, 30, 30));// Takes a portion of the sprite sheet
	mPlayer.setScale(sf::Vector2f(0.4, 0.4));
	mPlayer.setOrigin(sf::Vector2f(15, 8));
	mPlayer.setPosition(945.f, 100.f);
}


void Player::handlePlayerInput(sf::Keyboard::Key key,
	bool isPressed)
{
	// If a movement key is pressed then its corrosponding variable is set to true
	// References Game::processEvents()
	if (key == sf::Keyboard::W)
		pIsMovingUp = isPressed;
	else if (key == sf::Keyboard::S)
		pIsMovingDown = isPressed;
	else if (key == sf::Keyboard::A)
		pIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::D)
		pIsMovingRight = isPressed;
}

void Player::updateAnimationMovement()
{

	mPlayer.setTextureRect(sf::IntRect(30 * currentFrame, 0, 30, 30));

	if (frameCounter == 15)
	{
		currentFrame = (currentFrame + 1) % 2;
		frameCounter = 0;
	}
	frameCounter++;
}

void Player::updateAccelleration(sf::Time deltaTime)
{
	float maxSpeed = 0.f;

	if (pIsMovingUp)
		maxSpeed = maxSpeedForward;
	if (pIsMovingDown)
		maxSpeed = maxSpeedBackwards;

	float speedIncrease = maxSpeed / 2.0f;
	float Acceleration = speedIncrease * deltaTime.asSeconds();

	movementSpeed += Acceleration;
	if (movementSpeed > maxSpeed)
	{
		movementSpeed = maxSpeed;
	}
}

void Player::updateDeceleration(sf::Time deltaTime)
{
	float deceleration = 50.f;
	if (movementSpeed > 0)
	{
		movementSpeed -= deceleration * deltaTime.asSeconds();
		if (movementSpeed < 0)
			movementSpeed = 0;
	}
}

void Player::updateMovement(sf::Time deltaTime)
{
	// Moves the player in the a direction depending on what/if a key is pressed
	if (pIsMovingUp)
	{
		movement.x = 0.f;
		movement.y = -1.f;
		updateAnimationMovement();
		updateAccelleration(deltaTime);
	}
	if (pIsMovingDown)
	{
		movement.x = 0.f;
		movement.y = 1.f;
		updateAnimationMovement();
		updateAccelleration(deltaTime);
	}
	// Rotates the sprite to turn in the corresponding directions
	if (pIsMovingLeft && movementSpeed > 0)
	{
		if (movement.y < 0)
			mPlayer.rotate(-90.f * deltaTime.asSeconds());
		if (movement.y > 0)
			mPlayer.rotate(90.f * deltaTime.asSeconds());
	}
	if (pIsMovingRight && movementSpeed > 0)
	{
		if (movement.y < 0)
			mPlayer.rotate(90.f * deltaTime.asSeconds());
		if (movement.y > 0)
			mPlayer.rotate(-90.f * deltaTime.asSeconds());
	}

	if (!pIsMovingUp && !pIsMovingDown)
	{
		updateDeceleration(deltaTime);
	}

	if (movementSpeed > 0.f)
	{
		move(deltaTime);
	}
}


void Player::move(sf::Time deltaTime)
{
	// Finds the direction the player is facing
	float rotation = mPlayer.getRotation();

	// Converts the angle to a radian
	float angleInRadians = rotation * (3.14159f / 180.f);

	float cosRotation = cos(angleInRadians);
	float sinRotation = sin(angleInRadians);

	// Calculates the movement direction of the player
	rotatedMovement.x = movement.x * cosRotation - movement.y * sinRotation;
	rotatedMovement.y = movement.x * sinRotation + movement.y * cosRotation;

	//calculates movement trajectory
	if (movementSpeed > 0)
	{
		mPlayer.move(rotatedMovement * deltaTime.asSeconds() * movementSpeed);
	}
}