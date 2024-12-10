#include "Game.h"
#include "Player.h"
#include "Police.h"
#include <SFML/Graphics.hpp>
#include <iostream>

Game::Game()
    : mWindow(sf::VideoMode(1920, 1080), "On The Run") // Sets screen size and title
    , TimePerFrame(sf::seconds(1.f / 60.f))
{
    mWindow.setFramerateLimit(60);
    if (!mBackgroundTexture.loadFromFile("C:/Users/finga/OneDrive/Documents/BEng-Computing/3rd-Year/Games/games_5/games_eng_cw/res/Background/fullmap.png"))
    {
        std::cerr << "Error loading background texture" << std::endl;
    }
    mBackgroundSprite.setTexture(mBackgroundTexture);

    mView.setSize(1920.f, 1080.f); // Initialize to the window size
    mView.setCenter(mPlayer.getPosition()); // Center on the player
    mView.setSize(1920.f / 8.f, 1080.f / 8.f); // Zoom in (adjust divisor as needed)

    mapBounds = sf::FloatRect(0.f, 0.f, 1200.f, 700.f); // Adjust dimensions to your map size

    spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 1000.f), 0.f);
    spawnPoliceCar(policeCars, sf::Vector2f(400.f, 1000.f), 0.f);
    spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 400.f), 0.f);
}

void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

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
        case sf::Event::KeyPressed:
            mPlayer.handlePlayerInput(event.key.code, true);
            break;
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

    sf::Vector2f viewCenter = mPlayer.getPosition();

    float halfWidth = mView.getSize().x / 2.f;
    float halfHeight = mView.getSize().y / 2.f;

    if (viewCenter.x - halfWidth < mapBounds.left)
        viewCenter.x = mapBounds.left + halfWidth;
    if (viewCenter.x + halfWidth > mapBounds.left + mapBounds.width)
        viewCenter.x = mapBounds.left + mapBounds.width - halfWidth;

    if (viewCenter.y - halfHeight < mapBounds.top)
        viewCenter.y = mapBounds.top + halfHeight;
    if (viewCenter.y + halfHeight > mapBounds.top + mapBounds.height)
        viewCenter.y = mapBounds.top + mapBounds.height - halfHeight;

    mView.setCenter(viewCenter);
    mWindow.setView(mView);
}

void Game::render()
{
    mWindow.clear();
    mWindow.setView(mView);

    mWindow.draw(mBackgroundSprite);
    mWindow.draw(mPlayer.getSprite());

    for (const auto& police : policeCars)
    {
        police.draw(mWindow);
    }

    mWindow.display();
}

