#pragma once
#include "Scenario.h"
#include "Entity.h"
#include "Logger.h"
#include <vector>
#include <memory>
#include <thread>
#include <fstream>

class Game {
private:
    std::shared_ptr<bool> isGameOverFlag = std::make_shared<bool>(false);
    std::shared_ptr<bool> isFighting = std::make_shared<bool>(false);
    std::shared_ptr<Scenario> scenario = nullptr;
    std::shared_ptr<Character> player = nullptr;
    std::shared_ptr<std::vector<std::shared_ptr<Entity>>> entities = std::make_shared<std::vector<std::shared_ptr<Entity>>>();

    std::shared_ptr<Logger<Game>> logger = std::make_shared<Logger<Game>>();
public:
    Game() {
        logger->debug("Game created");
    }

    ~Game() {
        logger->debug("Game destroyed");
    }

    void setScenario(std::shared_ptr<Scenario> scenario) { this->scenario = scenario; }
    void setPlayer(std::shared_ptr<Character> player) { this->player = player; }
    void addEntity(std::shared_ptr<Entity> entity) { entities->push_back(entity); }

    std::shared_ptr<Character> getPlayer() { return player; }
    bool inFight() { return *isFighting; }
    bool isGameOver() { return *isGameOverFlag; }

    void start() {
        logger->debug("Game started");
        scenario->execute(*this);
    }

    void startFight() {
        if (entities->empty()) {
            logger->error("No entities");
            return;
        }

        logger->debug("Fight started");

        *isFighting = true;
        while (*isFighting && !*isGameOverFlag) {
            DialogueSystem* fightDialogueSystem = new DialogueSystem();

            fightDialogueSystem->createNewDialogue("[~] You are in a battle, choose an action:");
            fightDialogueSystem->createNewDialogue("[~] Choose who you will attack:");

            fightDialogueSystem->setNextDialogue(fightDialogueSystem->getStartDialogue(), nullptr);
            fightDialogueSystem->setNextDialogue(fightDialogueSystem->getEndDialogue(), nullptr);

            fightDialogueSystem->addChoiceToDialogue("Attack", fightDialogueSystem->searchDialogue(1), 0);
            fightDialogueSystem->addChoiceToDialogue("Heal", [&](void*) {
                player->heal();
                }, nullptr, 0);
            fightDialogueSystem->addChoiceToDialogue("Show your data", [&](void*) {
                player->display();
                }, nullptr, 0);

            for (auto& entity : *entities) {
                if (!entity->isAlive())
                    continue;
                fightDialogueSystem->addChoiceToDialogue(
                    entity->getName() + " -> HP: " + std::to_string(entity->getHealth()),
                    [&](void*) {
                        player->attack(*entity);
                        if (!entity->isAlive())
							return;
                        std::this_thread::sleep_for(std::chrono::milliseconds(300));
                        entity->attack(*player);
                    },
                    nullptr, 1
                );
            }

            fightDialogueSystem->execute(nullptr);
            delete fightDialogueSystem;

            bool hasAlive = false;
            for (auto& entity : *entities) {
                if (entity->isAlive()) {
                    hasAlive = true;
                    break;
                }
            }

            if (!hasAlive) {
                std::cout << "\033[32m[~] Monsters defeated!\033[0m" << std::endl;
                logger->debug("Monsters defeated");
                *isFighting = false;
                break;
            }

            if (!player->isAlive()) {
                std::cout << "\033[31m[-] The player died from the injuries\033[0m" << std::endl;
                logger->debug("Player died");
                *isGameOverFlag = true;
                break;
            }
        }
        entities->clear();
        logger->debug("Fight ended");
    }

    void save(const std::string& filename) const {
        logger->debug("Starting save game");
		std::ofstream file(filename, std::ios::binary);

        if (!file) {
			logger->error("Failed to open file for writing");
			return;
        }

        logger->debug("Saving game states");
        // Save game state
        file.write(reinterpret_cast<const char*>(&(*isGameOverFlag)), 1);
        file.write(reinterpret_cast<const char*>(&(*isFighting)), 1);

        logger->debug("Saving game objects");
        // Save player
        logger->debug("Saving player");
        player->save(file);

        // Save scenario
        logger->debug("Saving scenario");
        scenario->save(file);

        // Save entities
        size_t entityCount = entities->size();
        logger->debug("Saving " + std::to_string(entityCount) + " entities");
        file.write(reinterpret_cast<const char*>(&entityCount), 1);
        for (const auto& entity : *entities) {
            logger->debug("Saving entity<" + std::to_string(entity->getId()) + ">");
            entity->save(file);
        }
    }

    void load(const std::string& filename) {
        logger->debug("Loading game");
        std::ifstream file(filename, std::ios::binary);

        if (!file) {
            logger->error("Failed to open file for writing");
            return;
        }

        // Load game state
		logger->debug("Loading game states");
        file.read(reinterpret_cast<char*>(&(*isGameOverFlag)), 1);
        file.read(reinterpret_cast<char*>(&(*isFighting)), 1);

        // Load player
		logger->debug("Loading player");
        player = std::make_shared<Character>();
        player->load(file);

        // Load scenario
		logger->debug("Loading scenario");
        if (!scenario)
            scenario = std::make_shared<Scenario>();
        scenario->load(file);

        // Load entities
        size_t entityCount;
        file.read(reinterpret_cast<char*>(&entityCount), 1);
        if (entityCount > 20)
            entityCount = 0;
        logger->debug("Loading " + std::to_string(entityCount) + " entities");
        entities->clear();
        for (size_t i = 0; i < entityCount; ++i) {
            auto entity = std::make_shared<Entity>();
            entity->load(file);
            entities->push_back(entity);
        }
    }
};