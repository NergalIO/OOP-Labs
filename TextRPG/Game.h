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
            fightDialogueSystem->addChoiceToDialogue("Try to escape", [&](void*) {
                int chance = std::max(10, static_cast<int>(100 / entities->size()));
                if (rand() % 100 < chance) {
                    logger->debug("Player escaped");
                    std::cout << "\033[32m" << "[~] You successfully escaped" << "\033[0m" << std::endl;
                    *isFighting = false;
                }
                else if (rand() % 100 < chance / 2) {
                    logger->debug("While the player was trying to escape, the monster attacked him");
                    std::cout << "\033[31m" << "[~] While you were trying to escape, the monster attacked you" << "\033[0m" << std::endl;
                    entities->at(rand() % entities->size())->attack(*player);
                }
                else {
                    logger->debug("Player failed to escape");
                    std::cout << "\033[31m" << "[~] You failed to escape" << "\033[0m" << std::endl;
                }
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
        file.write(reinterpret_cast<const char*>(&(*isGameOverFlag)), sizeof(*isGameOverFlag));
        file.write(reinterpret_cast<const char*>(&(*isFighting)), sizeof(*isFighting));

        logger->debug("Saving game objects");
        // Save player
        logger->debug("Saving player");
        player->save(file);

        // Save scenario
        //logger->debug("Saving scenario");
        //scenario->save(file);

        // Save entities
		logger->debug("Saving entities");
        size_t entityCount = entities->size();
        file.write(reinterpret_cast<const char*>(&entityCount), sizeof(entityCount));
        for (const auto& entity : *entities) {
            logger->debug("Saving entity<" + std::to_string(entity->getId()) + ">");
            entity->save(file);
        }
    }

    void load(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);

        if (!file) {
            logger->error("Failed to open file for writing");
            return;
        }

        // Load game state
        file.read(reinterpret_cast<char*>(&(*isGameOverFlag)), sizeof(*isGameOverFlag));
        file.read(reinterpret_cast<char*>(&(*isFighting)), sizeof(*isFighting));

        // Load player
        player = std::make_shared<Character>();
        player->load(file);

        // Load scenario
        //scenario = std::make_shared<Scenario>();
        //scenario->load(file);

        // Load entities
        size_t entityCount;
        file.read(reinterpret_cast<char*>(&entityCount), sizeof(entityCount));
        entities->clear();
        for (size_t i = 0; i < entityCount; ++i) {
            auto entity = std::make_shared<Entity>();
            entity->load(file);
            entities->push_back(entity);
        }
    }
};