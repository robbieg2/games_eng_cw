#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Police.h"
#include <vector>

class Game
{
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void mapCollision();
    bool checkPosition(const sf::Vector2f& position);
    bool checkPositionFast(const sf::Vector2f& position);

private:
    sf::RenderWindow mWindow;
    sf::Time TimePerFrame = sf::seconds(1.f / 60.f);

    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;

    sf::Image path;
    std::vector<std::vector<bool>> collisionGrid;

    std::vector<Police> policeCars;

    Player mPlayer;
    Police mPolice;

    sf::View mView; // View for camera
    sf::FloatRect mapBounds; // Map bounds for clamping

    // Menu-related variables
    bool isMenuActive = true; // Determines if the menu is displayed
    sf::Texture menuTexture; // Texture for the menu
    sf::Sprite menuSprite; // Sprite for the menu
    sf::IntRect playButtonArea; // Area for the "PLAY" button
    sf::IntRect quitButtonArea; // Area for the "QUIT" button
};


int main()
{
    Game Game;
    Game.run();
}
