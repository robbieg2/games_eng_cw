#include "Game.h"
#include "Player.h"
#include "Police.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>
#include <vector>

Game::Game()
    : mWindow(sf::VideoMode(1920, 1080), "On The Run") // Sets screen size and title
    , TimePerFrame(sf::seconds(1.f / 60.f))
{
    mWindow.setFramerateLimit(60);
    if (!mBackgroundTexture.loadFromFile("res/Background/fullmap.png"))
    {
        std::cerr << "Error loading background texture" << std::endl;
    }
    mBackgroundSprite.setTexture(mBackgroundTexture);

    if (!path.loadFromFile("res/Background/path.png"))
    {
        std::cerr << "Error loading path" << std::endl;
    }

    mapCollision();

    mView.setSize(1920.f, 1080.f); // Initialize to the window size
    mView.setCenter(mPlayer.getPosition()); // Center on the player
    mView.setSize(1920.f / 8.f, 1080.f / 8.f); // Zoom in (adjust divisor as needed)

    mapBounds = sf::FloatRect(0.f, 0.f, path.getSize().x, path.getSize().y); // Adjust dimensions to your map size

    spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 1000.f), 0.f);
    spawnPoliceCar(policeCars, sf::Vector2f(400.f, 1000.f), 0.f);
    spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 400.f), 0.f);
}

void Game::mapCollision()
{
    collisionGrid.resize(path.getSize().x, std::vector<bool>(path.getSize().y, false));

    for (unsigned x = 0; x < path.getSize().x; ++x)
    {
        for (unsigned y = 0; y < path.getSize().y; ++y)
        {
            sf::Color pixelColor = path.getPixel(x, y);
            collisionGrid[x][y] = (pixelColor.r > 200 && pixelColor.g > 200 && pixelColor.b > 200);
        }
    }
}

bool Game::checkPosition(const sf::Vector2f& position)
{
    sf::Vector2u pixelPos(static_cast<unsigned>(position.x), static_cast<unsigned>(position.y));
    if (pixelPos.x >= path.getSize().x || pixelPos.y >= path.getSize().y)
    {
        return false;
    }

    sf::Color pixelColor = path.getPixel(pixelPos.x, pixelPos.y);
    return (pixelColor.r > 200 && pixelColor.g > 200 && pixelColor.b > 200);
}

bool Game::checkPositionFast(const sf::Vector2f& position)
{
    unsigned x = static_cast<unsigned>(position.x);
    unsigned y = static_cast<unsigned>(position.y);

    return (x < collisionGrid.size() && y < collisionGrid[0].size() && collisionGrid[x][y]);
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
    sf::Vector2f newPosition = mPlayer.getPosition();
    mPlayer.updateMovement(deltaTime);

    if (!checkPositionFast(mPlayer.getPosition()))
    {
        mPlayer.setPosition(newPosition);
    }

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

