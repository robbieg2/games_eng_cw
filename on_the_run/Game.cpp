#include "Game.h"
#include "Player.h"
#include "Police.h"
#include <SFML/Graphics.hpp>
#include <iostream>

Game::Game()
	:mWindow(sf::VideoMode(1920, 1080), "On The Run")// Sets screen size and title
	, TimePerFrame(sf::seconds(1.f / 60.f))
{
	mWindow.setFramerateLimit(60);
	if (!mBackgroundTexture.loadFromFile("bin/Debug/res/Background/tempBackground.png"))
	{
		std::cerr << "Error loading background texture" << std::endl;
	}
	mBackgroundSprite.setTexture(mBackgroundTexture);
}

void Game::run()

{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 1000.f), 0.f);
	spawnPoliceCar(policeCars, sf::Vector2f(400.f, 1000.f), 0.f);
	spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 400.f), 0.f);


	while (mWindow.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
			update(TimePerFrame);
		}
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
			// Sets movement to true when a movement key is pressed
		case sf::Event::KeyPressed:
			mPlayer.handlePlayerInput(event.key.code, true);
			break;
			// Sets movement to false when a movement key is released
		case sf::Event::KeyReleased:
			mPlayer.handlePlayerInput(event.key.code, false);
			break;
		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update(sf::Time deltaTime)
{
	mPlayer.updateMovement(deltaTime);

	for (auto& police : policeCars)
	{
		police.followPlayer(mPlayer, deltaTime.asSeconds());
	}
}

// Renders the window
void Game::render()
{
	mWindow.clear();
	mWindow.draw(mBackgroundSprite);
	mWindow.draw(mPlayer.getSprite());

	for (const auto& police : policeCars)
	{
		police.draw(mWindow);
	}
	mWindow.display();
}