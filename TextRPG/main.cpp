#include "Scenario.h"
#include "Dialogue.h"
#include "Logger.h"
#include "Items.h"
#include "Entity.h"
#include "Game.h"
#include <memory>
#include <csignal>
#include <cstdlib>
#include <fstream>

class Main {};
std::shared_ptr<Logger<Main>> logger = std::make_shared<Logger<Main>>();

Game game;

void handleSignal(int signal) {
    logger->debug("Interrupt signal (" + std::to_string(signal) + ") received.");
    game.save("data.bin");
    exit(signal);
}

bool saveFileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

int main() {
    std::setlocale(LC_ALL, "en_US.UTF-8");
    signal(SIGINT, handleSignal);

    try {
        Scenario scenario("darkForestScenario");

        scenario.setPlayerName("Adventurer");
        scenario.setPlayerDamage(12);
        scenario.setPlayerHealth(120);
        scenario.setPlayerDefense(7);
        scenario.setPlayerLevel(1);
        scenario.setPlayerExperience(0);

        std::shared_ptr<DialogueSystem> ds = scenario.getDialogueSystem();

        ds->createNewDialogue("You have entered the haunted castle. The air is thick with the smell of decay.");
        ds->createNewDialogue("A ghostly figure appears before you. 'Who dares to enter my domain?' it asks.");
        ds->createNewDialogue("When you defeat the ghost, you can rest for a while and recover your strength.");


        ds->addChoiceToDialogue("Fight the ghost", [&](void*) {
            std::cout << "\033[34mYou draw your sword, ready to fight the ghost.\033[0m\n";
            game.addEntity(std::make_shared<Monster>(0, "Ghost", 50, 10, 5, 100));
            game.startFight();
        }, ds->searchDialogue(2), 1);
        ds->addChoiceToDialogue("Run away", [&](void*) {
            std::cout << "\033[31mYou decide to run away, but the ghost blocks your path.\033[0m\n";
            game.addEntity(std::make_shared<Monster>(0, "Ghost", 50, 10, 5, 100));
            game.startFight();
        }, ds->searchDialogue(2), 1);


        game.setScenario(std::make_shared<Scenario>(scenario));

        if (saveFileExists("data.bin")) {
            std::cout << "A save file has been found. Do you want to load it? (yes/no): ";
            std::string choice;
            std::cin >> choice;
            if (choice == "yes" || choice == "y") {
                game.load("data.bin");
                if (game.isGameOver()) {
                    std::cout << "Game over.\n";
                    return 0;
                }
                else if (game.inFight()) {
                    std::cout << "Resuming fight...\n";
                    game.startFight();
                    game.save("data.bin");
                    return 0;
                }
            }
            else {
                std::cout << "Starting a new game...\n";
            }
        }

        game.start();
        game.save("data.bin");
    }
    catch (const std::exception& e) {
        logger->error(e.what());
        return -1;
    }
    return 0;
}
