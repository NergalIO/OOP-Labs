#include "Scenario.h"
#include "Game.h" 

void Scenario::execute(Game& game) {
	logger->debug("Scenario started");

	this->player = std::make_shared<Character>(playerName, playerHealth, playerDamage, playerDefense, playerLevel, playerExperience);
	game.setPlayer(this->player);

	for (auto& entity : entities)
		game.addEntity(entity);

	dialogueSystem->execute(nullptr);
}