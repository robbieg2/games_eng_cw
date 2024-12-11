#include "Game.h"
#include "Player.h"
#include "Police.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>
#include <vector>

Game::Game()
    : mWindow(sf::VideoMode(1920, 1080), "On The Run"), // Sets screen size and title
    TimePerFrame(sf::seconds(1.f / 60.f)),
    chunkSize(10)
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

    mView.setSize(1920.f / 8.f, 1080.f / 8.f); // Initialize to the window size
    mView.setCenter(mPlayer.getPosition()); // Center on the player

    mapBounds = sf::FloatRect(0.f, 0.f, path.getSize().x, path.getSize().y); // Adjust dimensions to your map size


    spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 1000.f), 0.f);
    spawnPoliceCar(policeCars, sf::Vector2f(400.f, 1000.f), 0.f);
    spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 400.f), 0.f);
}

void Game::mapCollision()
{
    sf::Vector2u size = path.getSize();
    unsigned width = size.x;
    unsigned height = size.y;
    collisionGrid.assign(height, std::vector<bool>(width, false));

    for (unsigned y = 0; y < path.getSize().y; ++y)
    {
        for (unsigned x = 0; x < path.getSize().x; ++x)
        {
            sf::Color pixelColor = path.getPixel(x, y);
            collisionGrid[y][x] = (pixelColor.r > 200 && pixelColor.g > 200 && pixelColor.b > 200);

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

    return y < collisionGrid.size() && x < collisionGrid[0].size() && collisionGrid[y][x];
}

void Game::loadMapAroundPlayer()
{
    sf::Vector2f playerPos = mPlayer.getPosition();
    int chunkX = static_cast<int>(playerPos.x) / chunkSize;
    int chunkY = static_cast<int>(playerPos.y) / chunkSize;

    for (int dx = -1; dx <= 1; ++ dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            if (!isChunkLoaded(chunkX + dx, chunkY + dy))
            {
                loadChunk(chunkX + dx, chunkY + dy);
            }
        }
    }
    unloadFarChunks(chunkX, chunkY);
}

bool Game::isChunkLoaded(int x, int y)
{
    return loadedChunks.find({ x, y }) != loadedChunks.end();
}

void Game::loadChunk(int x, int y)
{
    if (isChunkLoaded(x, y))
        return;

    loadedChunks.insert({ x, y });
    int chunkStartX = x * chunkSize;
    int chunkStartY = y * chunkSize;

    for (int cy = 0; cy < chunkSize; ++cy)
    {
        for (int cx = 0; cx < chunkSize; ++cx)
        {
            int gridX = chunkStartX + cx;
            int gridY = chunkStartY + cy;

            if (gridX < path.getSize().x && gridY < path.getSize().y)
            {
                sf::Color pixelColor = path.getPixel(gridX, gridY);
                collisionGrid[gridY][gridX] = (pixelColor.r > 200 && pixelColor.g > 200 && pixelColor.b > 200);
            }
        }
    }
}

void Game::unloadFarChunks(int currentChunkX, int currentChunkY)
{
    auto it = loadedChunks.begin();
    while (it != loadedChunks.end())
    {
        int ChunkX = it->first;
        int ChunkY = it->second;

        if (std::abs(chunkX - currentChunkX) > 2 || std::abs(chunkY - currentChunkY) > 2)
        {
            it = loadedChunks.erase(it);
        }
        else
        {
            ++it;
        }
    }
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
    sf::Vector2f prevPosition = mPlayer.getPosition();
    mPlayer.updateMovement(deltaTime);

    if (!checkPositionFast(mPlayer.getPosition()))
    {
        mPlayer.setPosition(prevPosition);
    }

    loadMapAroundPlayer();

    for (auto& police : policeCars)
    {
        police.setCollisionGrid(collisionGrid);
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

