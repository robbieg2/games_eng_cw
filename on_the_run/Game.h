#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Police.h"

class Game
{
public:
	Game();
	void	run();

private:
	void	processEvents();
	void	update(sf::Time deltaTime);
	void	render();

private:
	sf::RenderWindow mWindow;
	sf::Time TimePerFrame = sf::seconds(1.f / 60.f);

	sf::Texture mBackgroundTexture;
	sf::Sprite mBackgroundSprite;

	std::vector<Police> policeCars;

	Player mPlayer;
	Police mPolice;
};

int main()
{
	Game Game;
	Game.run();
}
