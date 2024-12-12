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

    // Load background texture
    if (!mBackgroundTexture.loadFromFile("res/Background/fullmap.png")) {
        std::cerr << "Error loading background texture" << std::endl;
    }
    mBackgroundSprite.setTexture(mBackgroundTexture);

    if (!path.loadFromFile("res/Background/path.png"))
    {
        std::cerr << "Error loading path" << std::endl;
    }

    // Load menu texture
    if (!menuTexture.loadFromFile("res/Background/menu.png")) {
        std::cerr << "Error loading menu image" << std::endl;
    }
    else {
        menuSprite.setTexture(menuTexture);

        // Scale the menu sprite to fit 1920x1080
        float menuScaleX = static_cast<float>(1920) / menuTexture.getSize().x;
        float menuScaleY = static_cast<float>(1080) / menuTexture.getSize().y;
        menuSprite.setScale(menuScaleX, menuScaleY);
        menuSprite.setPosition(0, 0); // puts menu in top corner
    }


    mapCollision();

    // Configure view (camera)
    mView.setSize(1920.f, 1080.f);
    mView.setCenter(mPlayer.getPosition());
    mView.setSize(1920.f / 8.f, 1080.f / 8.f);

    // Set map boundaries
    mapBounds = sf::FloatRect(0.f, 0.f, path.getSize().x, path.getSize().y); // Adjust dimensions to your map size

    // Spawn police cars
    spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 1000.f), 0.f);
    spawnPoliceCar(policeCars, sf::Vector2f(400.f, 1000.f), 0.f);
    spawnPoliceCar(policeCars, sf::Vector2f(1000.f, 400.f), 0.f);

    // Define interactive areas for the buttons
    playButtonArea = sf::IntRect(50, 200, 900, 450); // dimensions of menu buttons
    quitButtonArea = sf::IntRect(50, 600, 900, 450); 

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
            if (!isMenuActive) {
                update(TimePerFrame);
            }
        }
        render();
    }
}

void Game::processEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) {
            mWindow.close();
        }

        // Debug: Check menu state
        std::cout << "Is Menu Active (processEvents): " << isMenuActive << std::endl;

        if (isMenuActive) {
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(mWindow);
                std::cout << "Mouse Position: " << mousePos.x << ", " << mousePos.y << std::endl;

                if (playButtonArea.contains(mousePos)) {
                    std::cout << "Play button clicked!" << std::endl;
                    isMenuActive = false; // Start the game
                }
                else if (quitButtonArea.contains(mousePos)) {
                    std::cout << "Quit button clicked!" << std::endl;
                    mWindow.close(); // Exit the game
                }
            }
        }
        else {
            if (event.type == sf::Event::KeyPressed) {
                mPlayer.handlePlayerInput(event.key.code, true);
            }
            else if (event.type == sf::Event::KeyReleased) {
                mPlayer.handlePlayerInput(event.key.code, false);
            }
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

   

    mView.setCenter(viewCenter);
    mWindow.setView(mView);
}

void Game::render()
{
    mWindow.clear(); // Clear the window

    if (isMenuActive) {
        mWindow.setView(mWindow.getDefaultView()); // Reset the view to default for menu
        mWindow.draw(menuSprite); // Draw the menu fullscreen
    }
    else {
        mWindow.setView(mView); // Set the game camera view
        mWindow.draw(mBackgroundSprite); // Draw the game background
        mWindow.draw(mPlayer.getSprite()); // Draw the player sprite

        for (const auto& police : policeCars) {
            police.draw(mWindow); // Draw each police car
        }

    }

    

    mWindow.display(); // Display the rendered frame
}


