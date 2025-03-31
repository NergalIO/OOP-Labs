#pragma once
#include "Dialogue.h"
#include "Logger.h"
#include "Entity.h"
#include <vector>
#include <memory>
#include <fstream>

class Game;

class Scenario {
private:
    std::string scenarioName;
    std::shared_ptr<DialogueSystem> dialogueSystem;
    std::shared_ptr<Character> player;
    std::vector<std::shared_ptr<Entity>> entities;

    std::string playerName;
    int playerHealth;
    int playerDamage;
    int playerDefense;
    int playerLevel;
    int playerExperience;

    std::shared_ptr<Logger<Scenario>> logger = std::make_shared<Logger<Scenario>>();

public:
    Scenario()
        : scenarioName("DefaultScenario"),
        dialogueSystem(std::make_shared<DialogueSystem>()),
        player(nullptr),
        playerName("Player"),
        playerHealth(100),
        playerDamage(10),
        playerDefense(5),
        playerLevel(1),
        playerExperience(0)
    {
        logger->debug("Default Scenario created");
    }

    Scenario(const std::string& scenarioName)
        : scenarioName(scenarioName),
        dialogueSystem(std::make_shared<DialogueSystem>()),
        player(nullptr),
        playerName("Player"),
        playerHealth(100),
        playerDamage(10),
        playerDefense(5),
        playerLevel(1),
        playerExperience(0)
    {
        logger->debug("Scenario created");
    }

    Scenario(const Scenario& other)
        : scenarioName(other.scenarioName),
        dialogueSystem(std::make_shared<DialogueSystem>(*other.dialogueSystem)),
        player(other.player),
        entities(other.entities),
        playerName(other.playerName),
        playerHealth(other.playerHealth),
        playerDamage(other.playerDamage),
        playerDefense(other.playerDefense),
        playerLevel(other.playerLevel),
        playerExperience(other.playerExperience) {
        logger->debug("Scenario copied");
    }

    ~Scenario() {
        logger->debug("Scenario destroyed");
    }

    void setPlayerName(const std::string& name) { playerName = name; }
    void setPlayerHealth(int health) { playerHealth = health; }
    void setPlayerDamage(int damage) { playerDamage = damage; }
    void setPlayerDefense(int defense) { playerDefense = defense; }
    void setPlayerLevel(int level) { playerLevel = level; }
    void setPlayerExperience(int experience) { playerExperience = experience; }

    std::string getScenarioName() const { return scenarioName; }
    std::shared_ptr<DialogueSystem> getDialogueSystem() const { return dialogueSystem; }
    std::shared_ptr<Character> getPlayer() const { return player; }
    std::vector<std::shared_ptr<Entity>> getEntities() const { return entities; }
    std::string getPlayerName() const { return playerName; }
    int getPlayerHealth() const { return playerHealth; }
    int getPlayerDamage() const { return playerDamage; }
    int getPlayerDefense() const { return playerDefense; }
    int getPlayerLevel() const { return playerLevel; }
    int getPlayerExperience() const { return playerExperience; }

    void addEntity(std::shared_ptr<Entity> entity) {
        logger->debug("Entity<" + std::to_string(entity->getId()) + "> added to spawn");
        entities.push_back(entity);
    }

    void execute(Game& game);

    void save(std::ofstream& file) {
        // Save scenario attributes
        logger->debug("Saving scenario name");
        size_t strSize = scenarioName.size();
        file.write(reinterpret_cast<const char*>(&strSize), 1);
        file.write(scenarioName.c_str(), strSize);

        // Save player attributes
		logger->debug("Saving player attributes");
        strSize = playerName.size();
        file.write(reinterpret_cast<const char*>(&strSize), 1);
        file.write(playerName.c_str(), strSize);
        file.write(reinterpret_cast<const char*>(&playerHealth), 2);
        file.write(reinterpret_cast<const char*>(&playerDamage), 2);
        file.write(reinterpret_cast<const char*>(&playerDefense), 2);
        file.write(reinterpret_cast<const char*>(&playerLevel), 2);
        file.write(reinterpret_cast<const char*>(&playerExperience), 2);

        // Save dialogue system
		logger->debug("Saving dialogue system");
        dialogueSystem->save(file);

        // Save entities
        size_t entityCount = entities.size();
		logger->debug("Saving " + std::to_string(entityCount) + " entities");
        file.write(reinterpret_cast<const char*>(&entityCount), 1);
        for (const auto& entity : entities) {
            entity->save(file);
			logger->debug("Saved entity<" + std::to_string(entity->getId()) + ">");
        }
    }

    void load(std::ifstream& file) {
        // Load scenario attributes
        logger->debug("Loading scenario name");
        size_t strSize = 0;
        file.read(reinterpret_cast<char*>(&strSize), 1);
        scenarioName.resize(strSize);
        file.read(&scenarioName[0], strSize);

        // Load player attributes
		logger->debug("Loading player attributes");
        file.read(reinterpret_cast<char*>(&strSize), 1);
        playerName.resize(strSize);
        file.read(&playerName[0], strSize);
        file.read(reinterpret_cast<char*>(&playerHealth), 2);
        file.read(reinterpret_cast<char*>(&playerDamage), 2);
        file.read(reinterpret_cast<char*>(&playerDefense), 2);
        file.read(reinterpret_cast<char*>(&playerLevel), 2);
        file.read(reinterpret_cast<char*>(&playerExperience), 2);

        // Load dialogue system
		logger->debug("Loading dialogue system");
        if (!dialogueSystem)
            dialogueSystem = std::make_shared<DialogueSystem>();
        dialogueSystem->load(file);

        // Load entities
        size_t entityCount = 0;
        file.read(reinterpret_cast<char*>(&entityCount), 1);
        logger->debug("Loading " + std::to_string(entityCount) + " entities to spawn");
        entities.clear();
        for (size_t i = 0; i < entityCount; ++i) {
            auto entity = std::make_shared<Entity>();
            entity->load(file);
            logger->debug("Entity<" + std::to_string(entity->getId()) + "> loaded to spawn");
            entities.push_back(entity);
        }
    }
};