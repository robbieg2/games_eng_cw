#include "Police.h"
#include "Player.h"
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>

static sf::Texture mPoliceTexture;

Police::Police()
	: currentFrame(0),
	frameCounter(0),
	aiSpeed(40.f),
	aiTurnSpeed(90.f),
	mRotation(0.f),
	degreseToRadian(3.14159265f / 180.f)
{
	if (!mPoliceTexture.loadFromFile("res/police/PoliceMovingSpriteSheet.png"))
	{
		std::cerr << "Error loading the police texture" << std::endl;
	}
	else
	{
		mPolice.setTexture(mPoliceTexture);
	}// Location of sprite
	// Setting police sprite position and size
	mPolice.setTextureRect(sf::IntRect(0, 0, 30, 30));// Takes a portion of the sprite sheet
	mPolice.setScale(sf::Vector2f(1, 1));
	mPolice.setOrigin(sf::Vector2f(15, 8));
	mPolice.setPosition(1000.f, 1000.f);
}

Police::Police(sf::Vector2f startPosition, float startRotation = 0.f)
	: currentFrame(0),
	frameCounter(0),
	aiSpeed(40.f),
	aiTurnSpeed(90.f),
	mRotation(startRotation),
	degreseToRadian(3.14159265f / 180.f)
{
	if (!mPoliceTexture.loadFromFile("res/police/HelicopterSpriteSheet.png"))
	{
		std::cerr << "Error loading a police texture" << std::endl;
	}
	else
	{
		mPolice.setTexture(mPoliceTexture);
	}// Location of sprite
	// Setting police sprite position and size
	mPolice.setTextureRect(sf::IntRect(0, 0, 30, 30));// Takes a portion of the sprite sheet
	mPolice.setScale(sf::Vector2f(0.4, 0.4));
	mPolice.setOrigin(sf::Vector2f(15, 8));
	mPolice.setPosition(startPosition);
}

void spawnPoliceCar(std::vector<Police>& policeCars, sf::Vector2f position, float rotation = 0.f)
{
	policeCars.emplace_back(position, rotation);
}

void Police::draw(sf::RenderWindow& window) const
{
	window.draw(mPolice);
}

void Police::updateAnimationMovement()
{
	int frameWidth = 30;
	int frameHeight = 30;
	int numFrames = 2;

	mPolice.setTextureRect(sf::IntRect(frameWidth * currentFrame, 0, frameWidth, frameHeight));

	if (frameCounter == 15)
	{
		currentFrame = (currentFrame + 1) % numFrames;
		frameCounter = 0;
	}
	frameCounter++;
}

float Police::normalizeAngle(float angle)
{
	while (angle > 180.f) angle -= 360.f;
	while (angle < -180.f) angle += 360.f;
	return angle;
}

void Police::followPlayer(const Player& player, float deltaTime)
{
	sf::Vector2f direction = player.getPosition() - mPolice.getPosition();
	float angleToPlayer = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
	angleToPlayer += 90.f;

	float angleDifference = angleToPlayer - mRotation;
	angleDifference = normalizeAngle(angleDifference);

	if (std::abs(angleDifference) > 1.f)
	{
		if (angleDifference > 0)
			mRotation += aiTurnSpeed * deltaTime;
		else
			mRotation -= aiTurnSpeed * deltaTime;

		mRotation = normalizeAngle(mRotation);
	}
	else
	{
		float radianRotation = (mRotation - 90.f) * (3.14159265f / 180.f);
		sf::Vector2f velocity(std::cos(radianRotation) * aiSpeed,
			std::sin(radianRotation) * aiSpeed);
		mPolice.move(velocity * deltaTime);
	}

	mPolice.setRotation(mRotation);
}
