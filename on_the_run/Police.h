#pragma once
#ifndef POLICE_H
#define POLICE_H

#include "Player.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

class Police {
public:
	Police();
	Police(sf::Vector2f startPosition, float startRotation);

	void draw(sf::RenderWindow& window) const;

	void updateAnimationMovement();
	void followPlayer(const Player& player, float deltaTime);

	sf::Sprite& getSprite() { return mPolice; }

private:
	sf::Sprite mPolice;

	float aiSpeed;
	float aiTurnSpeed;
	float mRotation;
	float degreseToRadian;

	int currentFrame;
	int frameCounter;

	float normalizeAngle(float angle);
};

void spawnPoliceCar(std::vector<Police>& policeCars, sf::Vector2f position, float rotation);

#endif
