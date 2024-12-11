#pragma once
#include <SFML/Graphics.hpp>
#include <set>
#include "Police.h"

class Game
{
public:
    Game();
    void unloadFarChunks(int currentChunkX, int currentChunkY);
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void mapCollision();

    bool checkPosition(const sf::Vector2f& position);
    bool checkPositionFast(const sf::Vector2f& position);

    void loadMapAroundPlayer();

    bool isChunkLoaded(int x, int y);

    void loadChunk(int chunkX, int chunkY);

private:
    sf::RenderWindow mWindow;
    sf::Time TimePerFrame = sf::seconds(1.f / 60.f);

    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;

    sf::Image path;

    std::vector<Police> policeCars;

    Player mPlayer;
    Police mPolice;

    int chunkSize;
    sf::Vector2f playerPos;
    int chunkX;
    int chunkY;

    sf::View mView; // View for camera
    sf::FloatRect mapBounds; // Map bounds for clamping
    std::vector<std::vector<bool>> collisionGrid;
    std::set<std::pair<int, int>> loadedChunks;
};




int main()
{
    Game Game;
    Game.run();
    return 0;
}
