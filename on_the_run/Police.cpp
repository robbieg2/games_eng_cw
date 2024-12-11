#include "Police.h"
#include "Player.h"
#include <cmath>
#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
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
	mPolice.setTexture(mPoliceTexture);
	mPolice.setTextureRect(sf::IntRect(0, 0, 30, 30));// Takes a portion of the sprite sheet
	mPolice.setScale(sf::Vector2f(1, 1));
	mPolice.setOrigin(sf::Vector2f(15, 8));
	mPolice.setPosition(1000.f, 1000.f);
}

Police::Police(sf::Vector2f startPosition, float startRotation = 0.f)
	: Police()
{
	mPolice.setScale(0.4f, 0.4f);
	mPolice.setPosition(startPosition);
	mRotation = startRotation;
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

bool Police::isOnPath(const sf::Vector2i& node)
{
	return node.x >= 0 && node.y >= 0 &&
		node.x < collisionGrid[0].size() && node.y < collisionGrid.size() &&
		collisionGrid[node.y][node.x] == 0;
}

std::vector<sf::Vector2i> Police::getNeighbors(const sf::Vector2i& node)
{
	int dx[] = { 1, -1, 0, 0 };
	int dy[] = { 0, 0, 1, -1 };

	std::vector<sf::Vector2i> neighbors;
	for (int i = 0; i < 4; ++i)
	{
		sf::Vector2i neighbor(node.x + dx[i], node.y + dy[i]);	
		if (isOnPath(neighbor))
		{
			neighbors.push_back(neighbor);
		}
	}
	return neighbors;
}

std::vector<sf::Vector2i> Police::aStarPathfinding(const sf::Vector2i& start, const sf::Vector2i& target)
{
	constexpr int searchRadius = 10;
	const int gridWidth = collisionGrid[0].size();
	const int gridHeight = collisionGrid.size();

	auto isValidPosition = [&](const sf::Vector2i& pos) {
		return pos.x >= 0 && pos.y >= 0 && pos.x < gridWidth && pos.y < gridHeight &&
			std::abs(pos.x - start.x) <= searchRadius &&
			std::abs(pos.y - start.y) <= searchRadius &&
			collisionGrid[pos.y][pos.x] == 0;
	};

	Node startNode;
	startNode.position = start;
	startNode.gCost = 0.f;
	startNode.hCost = std::abs(target.x - start.x) + std::abs(target.y - start.y);
	startNode.parent = nullptr;

	std::priority_queue<Node> openSet;
	std::vector<std::vector<bool>> closedSet(gridHeight, std::vector<bool>(gridWidth, false));

	openSet.push(startNode);

	while (!openSet.empty())
	{
		Node currentNode = openSet.top();
		openSet.pop();

		if (currentNode.position == target)
		{
			std::vector<sf::Vector2i> path;
			for (Node* node = &currentNode; node; node = node->parent)
			{
				path.push_back(node->position);
			}
		std:: reverse(path.begin(), path.end());
		return path;
		}
		closedSet[currentNode.position.y][currentNode.position.x] = true;

		for (const sf::Vector2i& neighbor : getNeighbors(currentNode.position))
		{
			if (!isValidPosition(neighbor) || closedSet[neighbor.y][neighbor.x]) continue;

			openSet.push({neighbor, currentNode.gCost + 1,
						static_cast<float>(std::abs(target.x - neighbor.x) + std::abs(target.y - neighbor.y)),
						new Node(currentNode)});
		}
	}
	return {};
}



void Police::followPlayer(const Player& player, float deltaTime)
{
	sf::Vector2i playerGridPos(static_cast<int>(player.getPosition().x / 30), static_cast<int>(player.getPosition().y / 30));
	sf::Vector2i policeGridPos(static_cast<int>(mPolice.getPosition().x / 30), static_cast<int>(mPolice.getPosition().y / 30));

	path = aStarPathfinding(policeGridPos, playerGridPos);

	if (!path.empty())
	{
		sf::Vector2i nextStep = path.front();
		mPosition = sf::Vector2f(nextStep.x * 30.f, nextStep.y * 30.f);

		sf::Vector2f direction = mPosition - mPolice.getPosition();
		float angleToMove = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
		angleToMove += 90.f;

		float angleDifference = normalizeAngle(angleToMove - mRotation);
		
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
}
