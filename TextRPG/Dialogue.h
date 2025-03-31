#pragma once
#include "Logger.h"
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>

template <typename T>
class Choice {
private:
    std::string text;
    std::function<void(T)> action;

public:
    /* Constructor */
    Choice(const std::string& text, std::function<void(T)> action)
        : text(text), action(action) {
    }

    /* Methods */
    std::string getText() const { return text; }
    void execute(T param) const { action(param); }
};

class Dialogue {
private:
    int id;
    std::string text;
    int typeSpeed;
    std::vector<std::shared_ptr<Choice<void*>>> choices;
    std::shared_ptr<Dialogue> prevDialogue{ nullptr };
    std::shared_ptr<Dialogue> nextDialogue{ nullptr };

    std::shared_ptr<Logger<Dialogue>> logger = std::make_shared<Logger<Dialogue>>();
public:
    /* Constructor */
    Dialogue(const std::string& text)
        : id(0), text(text), typeSpeed(100) {
        logger->debug("Dialogue<" + std::to_string(id) + "> created");
    }

    Dialogue(std::shared_ptr<Dialogue> prevDialogue, const std::string& text)
        : text(text), prevDialogue(prevDialogue), typeSpeed(100) {
        this->id = prevDialogue->getId() + 1;
        logger->debug("Dialogue<" + std::to_string(id) + "> created");
    }

    /* Destructor */
    ~Dialogue() { logger->debug("Dialogue<" + std::to_string(id) + "> destroyed"); }

    /* Getters */
    int getId() const { return id; }
    bool hasChoice() const { return choices.size() > 0; }
    size_t choiceCount() const { return choices.size(); }
    std::shared_ptr<Dialogue> getNextDialogue() const { return nextDialogue; }

    /* Setters */
    void setTypeSpeed(int value) {
        if (value <= 0)
            throw std::invalid_argument("Type speed must be greater than 0");
        logger->debug("Updated type speed for Dialogue<" + std::to_string(id) + ">");
        typeSpeed = value;
    }
    void setId(int id) { this->id = id; }
    void setNextDialogue(std::shared_ptr<Dialogue> dialogue) {
        if (dialogue) {
            logger->debug("Updated next dialogue for Dialogue<" + std::to_string(id) + "> to Dialogue<" + std::to_string(dialogue->getId()) + ">");
            dialogue->setId(id + 1);
        }
        else {
			logger->debug("Updated next dialogue for Dialogue<" + std::to_string(id) + "> to nullptr");
        }
        nextDialogue = dialogue;
    }

    void addChoice(const std::string& choiceText, std::function<void(void*)> action, std::shared_ptr<Dialogue> nextDialogue = nullptr) {
        if (!action) {
            logger->error("Invalid action function");
            throw std::invalid_argument("Invalid action function");
        }
        choices.push_back(std::make_shared<Choice<void*>>(choiceText, [this, action, nextDialogue](void* param) {
            action(param);
            this->setNextDialogue(nextDialogue);
            }));
        logger->debug("Added choice with text: " + choiceText);
    }

    void addChoice(const std::string& choiceText, std::shared_ptr<Dialogue> nextDialogue) {
        choices.push_back(std::make_shared<Choice<void*>>(choiceText, [this, nextDialogue](void* param) {
            this->setNextDialogue(nextDialogue);
            }));
        logger->debug("Added choice with text: " + choiceText);
    }

    void display(bool output = true) const {
        if (!output)
            return;
        logger->debug("Displaying Dialogue<" + std::to_string(id) + ">");
        std::cout << "\033[34m";
        for (auto& letter : text) {
            std::cout << letter;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / typeSpeed));
        }
        std::cout << "\033[0m" << std::endl;
    }

    size_t choose(void* param, bool output = true, int choice = 255) const {
        if (choice == 255 && !output)
            return choice;

        while (choice - 1 < 0 || choice - 1 >= choices.size())
        {
            if (output) {
                display(output);
                for (size_t i = 0; i < choices.size(); ++i)
                {
                    std::cout << "- " << i + 1 << "." << choices[i]->getText() << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                std::cout << "\033[35m[?] Enter your choice: ";
            }
            std::cin >> choice;
            std::cin.ignore();
            if (choice - 1 < 0 || choice - 1 >= choices.size())
                std::cout << "\033[31m[-] Invalid choice" << "\033[0m" << std::endl;
            else
                break;
        }
        choices[choice - 1]->execute(param);
        logger->debug("Selected choice " + std::to_string(choice) + " for Dialogue<" + std::to_string(id) + ">");
        return choice;
    }

    int execute(void* param, bool output = true) {
        if (hasChoice()) {
            int choice = choose(param, output);
            logger->debug("Returning from Dialogue<" + std::to_string(id) + "> with choice <" + std::to_string(choice) + ">");
            return choice;
        }
        display(output);
        logger->debug("Returning from Dialogue<" + std::to_string(id) + "> with no choice");
        return -1;
    }
};

class DialogueSystem {
private:
    bool playing = false;

    std::shared_ptr<std::vector<std::shared_ptr<Dialogue>>> allDialogues;
    std::shared_ptr<Dialogue> startDialogue{ nullptr };
    std::shared_ptr<Dialogue> currentDialogue{ nullptr };
    std::shared_ptr<Dialogue> endDialogue{ nullptr };

    std::shared_ptr<std::vector<int>> choices;

    std::shared_ptr<Logger<DialogueSystem>> logger = std::make_shared<Logger<DialogueSystem>>();
public:
    DialogueSystem()
        : choices(std::make_shared<std::vector<int>>()),
        allDialogues(std::make_shared<std::vector<std::shared_ptr<Dialogue>>>()) {
        logger->debug("DialogueSystem created");
    }

    ~DialogueSystem() {
        logger->debug("DialogueSystem destroyed");
    }

    std::shared_ptr<Dialogue> getStartDialogue() const { return startDialogue; }
    std::shared_ptr<Dialogue> getCurrentDialogue() const { return currentDialogue; }
    std::shared_ptr<Dialogue> getEndDialogue() const { return endDialogue; }

    void createNewDialogue(std::string text) {
        std::shared_ptr<Dialogue> dialogue = std::make_shared<Dialogue>(text);
        if (!startDialogue) {
            logger->debug("Selecting Dialogue<0> as start dialogue");
            startDialogue = dialogue;
            currentDialogue = dialogue;
            endDialogue = dialogue;
        }
        else {
            logger->debug("Selecting Dialogue<" + std::to_string(endDialogue->getId() + 1) + "> as end dialogue");
            dialogue->setId(endDialogue->getId() + 1);
            endDialogue->setNextDialogue(dialogue);
            endDialogue = endDialogue->getNextDialogue();
        }
		allDialogues->push_back(dialogue);
    }

    void setNextDialogue(std::shared_ptr<Dialogue> dialogue, std::shared_ptr<Dialogue> nextDialogue)
    {
        dialogue->setNextDialogue(nextDialogue);
    }

    std::shared_ptr<Dialogue> searchDialogue(size_t id) {
        logger->debug("Searching Dialogue<" + std::to_string(id) + ">");
        std::shared_ptr<Dialogue> dialogue = startDialogue;
        for (auto& dialogue : *allDialogues)
        {
            if (dialogue->getId() == id) {
                logger->debug("Dialogue<" + std::to_string(id) + "> found");
                return dialogue;
            }
        }
        logger->debug("Dialogue<" + std::to_string(id) + "> not found");
        return nullptr;
    }

    void addChoiceToDialogue(std::string text, std::shared_ptr<Dialogue> nextDialogue, int id = -1) {
        if (!startDialogue) {
            logger->error("No dialogue created");
            return;
        }
        else if (id == -1) {
            logger->debug("Adding choice to end of Dialogue<" + std::to_string(endDialogue->getId()) + ">");
            endDialogue->addChoice(text, nextDialogue);
        }
        else if (id < 0) {
            logger->error("Invalid id");
            throw std::invalid_argument("Invalid id");
        }

        std::shared_ptr<Dialogue> dialogue = searchDialogue(id);
        if (!dialogue) {
            logger->error("Dialogue<" + std::to_string(id) + "> not found");
            throw std::invalid_argument("Dialogue<" + std::to_string(id) + "> not found");
        }

        logger->debug("Adding choice to Dialogue<" + std::to_string(dialogue->getId()) + ">");
        dialogue->addChoice(text, nextDialogue);
    }

    void addChoiceToDialogue(std::string text, std::function<void(void*)> action, std::shared_ptr<Dialogue> nextDialogue, int id = -1) {
        if (!startDialogue.get()) {
            logger->error("No dialogue created");
            return;
        }

        if (id == -1) {
            logger->debug("Adding choice to end of Dialogue<" + std::to_string(endDialogue->getId()) + ">");
            endDialogue->addChoice(text, action);
            return;
        }
        else if (id < 0) {
            logger->error("Invalid id");
            throw std::invalid_argument("Invalid id");
        }

        std::shared_ptr<Dialogue> dialogue = searchDialogue(id);
        if (!dialogue) {
            logger->error("Dialogue<" + std::to_string(id) + "> not found");
            throw std::invalid_argument("Dialogue<" + std::to_string(id) + "> not found");
        }

        logger->debug("Adding choice to Dialogue<" + std::to_string(dialogue->getId()) + ">");
        dialogue->addChoice(text, action, nextDialogue);
    }

    void stop() { playing = false; }

    void execute(void* param) {
        playing = true;
        while (currentDialogue && playing) {
            logger->debug("Executing Dialogue<" + std::to_string(currentDialogue->getId()) + ">");
            int choice = currentDialogue->execute(param);
            choices->push_back(choice);

            if (currentDialogue->getNextDialogue()) {
                logger->debug("Selected next Dialogue<" + std::to_string(currentDialogue->getNextDialogue()->getId()) + ">");
                currentDialogue = currentDialogue->getNextDialogue();
            }
            else {
                logger->debug("No next dialogue found, ending dialogue sequence");
                break;
            }

            logger->debug("Dialogue<" + std::to_string(currentDialogue->getId()) + "> finished");
        }
        logger->debug("DialogueSystem finished");
    }

    void fastTravel(std::shared_ptr<std::vector<int>> choices) {
        currentDialogue = startDialogue;
		for (auto& choice : *choices) {
            if (currentDialogue->hasChoice() && choice != 255)
                currentDialogue->choose(nullptr, false, choice);
            currentDialogue = currentDialogue->getNextDialogue();
		}
    }

    void save(std::ofstream& file) {
        size_t size = choices->size();
        file.write(reinterpret_cast<const char*>(&size), 2);
        logger->debug("Saving " + std::to_string(choices->size()) + " choices");
        for (auto& choice : *choices) {
            file.write(reinterpret_cast<const char*>(&choice), 1);
        }
    }

    void load(std::ifstream& file) {
        size_t size = 0;
        file.read(reinterpret_cast<char*>(&size), 2);
        logger->debug("Loading " + std::to_string(size) + " choices");
        choices->clear();
        for (size_t i = 0; i < size; ++i) {
            int choice = 0;
            file.read(reinterpret_cast<char*>(&choice), 1);
            choices->push_back(choice);
        }

        if (startDialogue) {
            logger->debug("Dialog tree recovery");
            fastTravel(choices);
        }
        else {
            logger->debug("No start dialogue found, skipping recovery dialog tree");
			choices->clear();
        }
    }
};