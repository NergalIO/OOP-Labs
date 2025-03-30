#include <iostream>
#include <string>

class Entity
{
protected:
    std::string name;
    int health;
public:
    Entity(const std::string& name, int health)
        : name(name), health(health) {}

    virtual void displayInfo() const
    {
        std::cout << "Name: " << name << ", HP: " << health << std::endl;
    }

    virtual ~Entity() {}
};


class Player : Entity
{
private:
    int experience;
public:
    Player(const std::string& name, int health, int experience)
        : Entity(name, health), experience(experience) {}

    void displayInfo() const override
    {
        Entity::displayInfo();
        std::cout << "Experience: " << experience << std::endl;
    }
};


class Enemy : Entity
{
private:
    std::string type;
public:
    Enemy(const std::string& name, int health, const std::string& type)
        : Entity(name, health), type(type) {}

    void displayInfo() const override
    {
        Entity::displayInfo();
        std::cout << "Type: " << type << std::endl;
    }
};


class Boss : Enemy
{
private:
    std::string specialAbility;
public:
    Boss(const std::string& name, int health, const std::string& type, const std::string& specialAbility)
        : Enemy(name, health, type), specialAbility(specialAbility) {}

    void displayInfo() const override
    {
        Enemy::displayInfo();
        std::cout << "Special Ability: " << specialAbility << std::endl;
    }
};


int main()
{
    Player hero("Hero", 100, 10);
    Enemy goblin("Goblin", 50, "Goblin");
    Boss boss("Hoggoblin", 200, "Boss", "boss special ability");

    std::cout << "\n[~] Information about entities:\n" << std::endl;
    hero.displayInfo();
    std::cout << std::endl;
    goblin.displayInfo();
    std::cout << std::endl;
    boss.displayInfo();
    std::cout << std::endl;

    return 0;
}