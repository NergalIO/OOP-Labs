/* Includes */
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <iostream>
#include <string>
#include <cstdlib>

/* Entity classes */
class Entity
{
protected:
    std::string name;
    int health;
    int attack;
    int defense;
public:
    Entity(const std::string& name, int health, int attack, int defense)
        : name(name), health(health), attack(attack), defense(defense) {}

    std::string getName() const { return name; }

    int getHealth() const { return health; }

    int getAttack() const { return attack; }

    int getDefense() const { return defense; }

    virtual void displayInfo() const
    {
        std::cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    virtual void heal(int amount) { }

    virtual void takeDamage(int amount)
    {
        if (amount < 0)
        {
            std::cout << "it has no effect!" << std::endl;
        }
        else if (health - amount <= 0)
        {
			health = 0;
			std::cout << name << " took " << amount
				<< " damage, and died!" << std::endl;
        }
        else
        {
            health -= amount;
            std::cout << name << " took " << amount
                << " damage, new Health: " << health << std::endl;
        }
    }

    virtual void attackEnemy(Entity& target)
    {
        std::cout << name << " attacks " << target.name << " >> ";
        int damage = attack - target.defense;
        target.takeDamage(damage);
    }

    virtual ~Entity() {}
};

/* Character and Monster classes */
class Character : public Entity
{
public:
    Character(const std::string& name, int health, int attack, int defense)
        : Entity(name, health, attack, defense) {}

    void heal(int amount) override
    {
        if (health + amount > 100)
        {
            health = 100;
            std::cout << name << " healed to maximum health points, new Health: "
                << health << std::endl;
        }
        else
        {
            health += amount;
            std::cout << name << " healed " << amount
                << " health points, new Health: " << health << std::endl;
        }
    }

    void attackEnemy(Entity& target) override
    {
        if (target.getHealth() == 0)
            return;

        int damage = attack - target.getDefense();

        bool isCriticalHit = (rand() % 100) < 20;
        if (isCriticalHit)
            std::cout << "\033[35m" << name << " attacks " << target.getName() << " with critical hit x2 >> ";
        else
			std::cout << "\033[32m" << name << " attacks " << target.getName() << " >> ";
        target.takeDamage(isCriticalHit ? damage * 2 : damage);
        std::cout << "\033[0m";
    }

    void displayInfo() const override
    {
        std::cout << "Character: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

class Monster : public Entity
{
public:
    Monster(const std::string& name, int health, int attack, int defense)
        : Entity(name, health, attack, defense) {}

    void attackEnemy(Entity& target) override
    {
        int damage = attack - target.getDefense();

        bool hasExtraDamage = rand() % 100 < 30;
        if (hasExtraDamage)
            std::cout << "\033[35m" << name << " attacks " << target.getName() << " with extra damage +5 >> ";
        else
			std::cout << "\033[36m" << name << " attacks " << target.getName() << " >> ";

        target.takeDamage(hasExtraDamage ? damage + 5 : damage);
        std::cout << "\033[0m";
    }

    void displayInfo() const override
    {
        std::cout << "Monster: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

/* Global variables */
bool heroAlive = true;
std::vector<Monster> monsters;
std::mutex monstersMutex;

/* Function to generate new monsters */
void generateMonsters() {
    while (heroAlive) {
        std::this_thread::sleep_for(std::chrono::seconds(3)); // Новый монстр каждые 3 секунды
        std::lock_guard<std::mutex> lock(monstersMutex);
        monsters.push_back(Monster("Goblin", 50, 15, 5));
        std::cout << "New monster generated!\n";
    }
}

/* Function of calculating fight */
void fight(Character& hero) {
    while (heroAlive)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::lock_guard<std::mutex> lock(monstersMutex);
        for (auto& monster : monsters)
        {
            if (rand() % 100 < 50 ? false : true)
                std::cout << "\033[33m" << "Hero misses the attack!" << "\033[0m" << std::endl;
            else
                hero.attackEnemy(monster);
            
            std::this_thread::sleep_for(std::chrono::seconds(1));

            if (rand() % 100 < 50 ? false : true)
                std::cout << "\033[33m" << "Monster misses the attack!" << "\033[0m" << std::endl;
            else
                monster.attackEnemy(hero);
			

            if (hero.getHealth() <= 0)
            {
                std::cout << "\033[31m" << hero.getName() << " died!" << "\033[0m" << std::endl;
                heroAlive = false;
                break;
            }
        }
    }
}

/* Main function */
int main() {
    /* Monster generator thread */
    std::thread monsterGenerator(generateMonsters);
    monsterGenerator.detach();

    Character hero("Hero", 100, 20, 10);

    std::thread fightThread(fight, std::ref(hero));
	fightThread.detach();
    
    while (heroAlive)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    int killedMonsters = 0;
    for (auto& monster : monsters)
    {
        if (monster.getHealth() <= 0)
			killedMonsters++;
    }

    std::cout << "\033[34m" << "\nBefore died, hero kill " << killedMonsters << " monsters!\n" << "\033[31m" << std::endl;

    return 0;
}
