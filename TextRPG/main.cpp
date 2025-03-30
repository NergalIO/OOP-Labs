#include "Scenario.h"
#include "Dialogue.h"
#include "Items.h"
#include "Entity.h"
#include "Game.h"
#include <memory>
#include <csignal>
#include <cstdlib>
#include <fstream>

Game game;

void handleSignal(int signal) {
    std::cout << "Interrupt signal (" << signal << ") received.\n";
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

        Scenario scenario("darkForestScenario");

        scenario.setPlayerName("Adventurer");
        scenario.setPlayerDamage(12);
        scenario.setPlayerHealth(120);
        scenario.setPlayerDefense(7);
        scenario.setPlayerLevel(1);
        scenario.setPlayerExperience(0);

        std::shared_ptr<DialogueSystem> ds = scenario.getDialogueSystem();

        ds->createNewDialogue("[~] You wake up to the screeching sound of alarms. The station is under lockdown.");
        ds->createNewDialogue("[~] You rush to the control room, where sparks fly from damaged panels.");
        ds->createNewDialogue("[~] A hologram of the station commander flickers on the screen, urgently issuing commands.");
        ds->createNewDialogue("[~] The reactor is about to overload! You need to shut it down before it's too late.");
        ds->createNewDialogue("[~] The doors to the control room slam shut, trapping you inside.");
        ds->createNewDialogue("[~] You hear footsteps approaching, but they stop suddenly, as if someone is watching you.");
        ds->createNewDialogue("[~] A voice over the intercom orders you to stay put. It sounds familiar, but strange.");
        ds->createNewDialogue("[~] Do you attempt to hack into the station's systems to regain control?");
        ds->createNewDialogue("[~] You begin to search for a way to override the lockdown. The system is encrypted, but you spot a terminal with a decryption key.");
        ds->createNewDialogue("[~] As you work, you hear someone approaching. It's the owner of the station, and he's not alone.");
        ds->createNewDialogue("[~] He enters, followed by a man in a military uniform. The atmosphere grows tense.");
        ds->createNewDialogue("[~] The owner demands answers, but the man in uniform says, 'We need to deal with the reactor first.'");
        ds->createNewDialogue("[~] The reactor's core is unstable. You must decide: will you try to stabilize it manually, or use the backup system to cool it down?");
        ds->createNewDialogue("[~] You opt to stabilize the reactor manually, but the controls are malfunctioning. The temperature is rising.");
        ds->createNewDialogue("[~] The military man starts arguing with the owner, and in the confusion, you notice a hatch leading to a maintenance tunnel.");
        ds->createNewDialogue("[~] You decide to make a run for the tunnel, hoping to find a way out of the madness.");
        ds->createNewDialogue("[~] The hatch is locked, but the owner throws you a key and urges you to leave.");
        ds->createNewDialogue("[~] As you enter the tunnel, you hear the sounds of explosions. The station is falling apart.");
        ds->createNewDialogue("[~] You crawl through the dark, narrow tunnel, feeling the walls tremble with each passing moment.");
        ds->createNewDialogue("[~] After what feels like an eternity, you emerge in a hidden chamber filled with strange equipment.");
        ds->createNewDialogue("[~] In the center of the room, there's a strange device. It looks like it could either help you escape or endanger everyone.");
        ds->createNewDialogue("[~] Do you activate the device, or leave it alone and find another way out?");
        ds->createNewDialogue("[~] You decide to activate the device. A hologram flickers to life, revealing an emergency escape plan.");
        ds->createNewDialogue("[~] The hologram warns that the escape will take you through a nearby asteroid field, but it's your only option.");
        ds->createNewDialogue("[~] You climb into the escape pod and launch towards the unknown.");
        ds->createNewDialogue("[~] The pod shakes violently as it enters the asteroid field, but you manage to navigate through the chaos.");
        ds->createNewDialogue("[~] Just when you think you've made it, you spot a massive black hole ahead. You need to make a choice.");
        ds->createNewDialogue("[~] Do you try to escape the gravitational pull of the black hole, or use it to your advantage and dive in?");
        ds->createNewDialogue("[~] You attempt to avoid the black hole, but your pod is caught in its pull. Everything goes dark.");
        ds->createNewDialogue("[~] As you approach the event horizon, you feel a strange sensation—time seems to stretch.");
        ds->createNewDialogue("[~] Suddenly, you're back in the station, but everything has changed. Is this a new reality, or just another layer of the nightmare?");
        
        ds->addChoiceToDialogue("Hack into the station's systems", [&](void*) {
            std::cout << "\033[34m" << "[~] You manage to access the encryption key and begin overriding the system!" << "\033[0m" << std::endl;
            }, ds->searchDialogue(8), 3);
        ds->addChoiceToDialogue("Escape through the hatch", [&](void*) {
            std::cout << "\033[34m" << "[~] You rush through the hatch, leaving the chaos behind." << "\033[0m" << std::endl;
            }, ds->searchDialogue(14), 7);
        ds->addChoiceToDialogue("Activate the device", [&](void*) {
            std::cout << "\033[34m" << "[~] You activate the device, revealing a way out!" << "\033[0m" << std::endl;
            }, ds->searchDialogue(20), 12);
        ds->addChoiceToDialogue("Dive into the black hole", [&](void*) {
            std::cout << "\033[34m" << "[~] You take a leap of faith, diving into the black hole!" << "\033[0m" << std::endl;
            }, ds->searchDialogue(22), 18);

        game.setScenario(std::make_shared<Scenario>(scenario));
        game.start();
        game.save("data.bin");
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
