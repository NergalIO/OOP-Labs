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

    Logger<Scenario> logger;

public:
    Scenario()
        : scenarioName("DefaultScenario"),
        dialogueSystem(std::make_shared<DialogueSystem>()), logger("Scenario"),
        player(nullptr),
        playerName("Player"),
        playerHealth(100),
        playerDamage(10),
        playerDefense(5),
        playerLevel(1),
        playerExperience(0)
    {
        logger.debug("Default Scenario created");
    }

    Scenario(const std::string& scenarioName)
        : scenarioName(scenarioName),
        dialogueSystem(std::make_shared<DialogueSystem>()), logger("Scenario"),
        player(nullptr),
        playerName("Player"),
        playerHealth(100),
        playerDamage(10),
        playerDefense(5),
        playerLevel(1),
        playerExperience(0)
    {
        logger.debug("Scenario created");
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
        playerExperience(other.playerExperience),
        logger("Scenario") {
        logger.debug("Scenario copied");
    }

    ~Scenario() {
        logger.debug("Scenario destroyed");
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
        entities.push_back(entity);
    }

    void execute(Game& game);

    void save(std::ofstream& file) const {
        // Save scenario attributes
        size_t strSize = scenarioName.size();
        file.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
        file.write(scenarioName.c_str(), strSize);

        // Save player attributes
        strSize = playerName.size();
        file.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
        file.write(playerName.c_str(), strSize);
        file.write(reinterpret_cast<const char*>(&playerHealth), sizeof(playerHealth));
        file.write(reinterpret_cast<const char*>(&playerDamage), sizeof(playerDamage));
        file.write(reinterpret_cast<const char*>(&playerDefense), sizeof(playerDefense));
        file.write(reinterpret_cast<const char*>(&playerLevel), sizeof(playerLevel));
        file.write(reinterpret_cast<const char*>(&playerExperience), sizeof(playerExperience));

        // Save dialogue system
        dialogueSystem->save(file);

        // Save entities
        size_t entityCount = entities.size();
        file.write(reinterpret_cast<const char*>(&entityCount), sizeof(entityCount));
        for (const auto& entity : entities) {
            entity->save(file);
        }
    }

    void load(std::ifstream& file) {
        // Load scenario attributes
        size_t strSize;
        file.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
        scenarioName.resize(strSize);
        file.read(&scenarioName[0], strSize);

        // Load player attributes
        file.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
        playerName.resize(strSize);
        file.read(&playerName[0], strSize);
        file.read(reinterpret_cast<char*>(&playerHealth), sizeof(playerHealth));
        file.read(reinterpret_cast<char*>(&playerDamage), sizeof(playerDamage));
        file.read(reinterpret_cast<char*>(&playerDefense), sizeof(playerDefense));
        file.read(reinterpret_cast<char*>(&playerLevel), sizeof(playerLevel));
        file.read(reinterpret_cast<char*>(&playerExperience), sizeof(playerExperience));

        // Load dialogue system
        dialogueSystem = std::make_shared<DialogueSystem>();
        dialogueSystem->load(file);

        // Load entities
        size_t entityCount;
        file.read(reinterpret_cast<char*>(&entityCount), sizeof(entityCount));
        entities.clear();
        for (size_t i = 0; i < entityCount; ++i) {
            auto entity = std::make_shared<Entity>();
            entity->load(file);
            entities.push_back(entity);
        }
    }
};