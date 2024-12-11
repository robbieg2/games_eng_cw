#pragma once
#ifndef POLICE_H
#define POLICE_H

#include "Player.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <cmath>


class Police {
public:
	Police();
	Police(sf::Vector2f startPosition, float startRotation);

	void draw(sf::RenderWindow& window) const;
	void updateAnimationMovement();
	void followPlayer(const Player& player, float deltaTime);

	sf::Sprite& getSprite() { return mPolice; }

	std::vector<sf::Vector2i> aStarPathfinding(const sf::Vector2i& start, const sf::Vector2i& target);
	bool isOnPath(const sf::Vector2i& node);
	std::vector<sf::Vector2i> getNeighbors(const sf::Vector2i& node);

	void setCollisionGrid(const std::vector<std::vector<bool>>& grid) {
		collisionGrid = grid;
	}

private:
	sf::Sprite mPolice;

	float aiSpeed;
	float aiTurnSpeed;
	float mRotation;
	float degreseToRadian;

	int currentFrame;
	int frameCounter;

	std::vector<sf::Vector2i> path;
	std::vector<std::vector<bool>> collisionGrid;
	sf::Vector2f mPosition;

	struct Node
	{
		sf::Vector2i position;
		float gCost;
		float hCost;
		Node* parent;

		bool operator<(const Node& other) const
		{
			return (gCost + hCost) > (other.gCost + other.hCost);
		}
	};

	float normalizeAngle(float angle);
};

void spawnPoliceCar(std::vector<Police>& policeCars, sf::Vector2f position, float rotation);

#endif
