#include <iostream>
#include <string>


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
        int damage = attack - target.getDefense();

        bool isCriticalHit = (rand() % 100) < 20;
        if (isCriticalHit)
            std::cout << name << " attacks " << target.getName() << " with critical hit x2 >> ";
        target.takeDamage(isCriticalHit ? damage * 2 : damage);
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
            std::cout << name << " attacks " << target.getName() << " with extra damage +5 >> ";
        target.takeDamage(hasExtraDamage ? damage + 5 : damage);
    }

    void displayInfo() const override
    {
        std::cout << "Monster: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};


class Boss : public Monster
{
private:
    std::string specialAbility;
public:
    Boss(const std::string& name, int health, int attack, int defense, const std::string& specialAbility)
        : Monster(name, health, attack, defense), specialAbility(specialAbility) {}

    void displayInfo() const override
    {
        Monster::displayInfo();
        std::cout << "Special Ability: " << specialAbility << std::endl;
    }

    void attackEnemy(Entity& target) override
    {
        int damage = attack - target.getDefense();

        bool isSpecialAttack = (rand() % 100) < 20;
        if (isSpecialAttack)
            std::cout << name << " uses " << specialAbility << " >> ";
        target.takeDamage(isSpecialAttack ? damage * 4 : damage);
    }
};


int main() {
    Character player("Player", 100, 20, 10);
    Monster goblin("goblin", 50, 15, 5);
    Boss dragon("Dragon", 150, 25, 20, "Fireball");

    Entity* entities[] = { &player, &goblin, &dragon };

    std::cout << "\n[~] Information about entities:\n" << std::endl;
    for (const auto& entity : entities) {
        entity->displayInfo();
    }

    std::cout << "\n[~] Player attacks enemies:\n" << std::endl;
    player.attackEnemy(goblin);
    player.attackEnemy(dragon);

    std::cout << "\n[~] Goblin attacks enemies:\n" << std::endl;
    goblin.attackEnemy(player);
    goblin.attackEnemy(dragon);

    std::cout << "\n[~] Dragon attacks enemies:\n" << std::endl;
    dragon.attackEnemy(player);
    dragon.attackEnemy(goblin);

    std::cout << "\n[~] Player heals 10 hp:\n" << std::endl;
    player.heal(10);
    std::cout << std::endl;

    return 0;
}
