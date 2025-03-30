#include <iostream>
#include <string>


class Character
{
private:
	std::string name;
	int health;
	int maxHealth;
	int attack;
	int defense;
public:
	Character(const std::string& name, int health, int maxHealth, int attack, int defense)
		: name(name), health(health), attack(attack), maxHealth(maxHealth), defense(defense) {}

	int getHealth()
	{
		return health;
	}

	void displayInfo()
	{
		std::cout << "Name: " << name << ", HP: " << health
			<< ", Attack: " << attack << ", Defense: " << defense << std::endl;
	}

	void heal(int amount)
	{
		if (health + amount > maxHealth)
		{
			health = maxHealth;
			std::cout << name << " healed to maximum health points, new Health: "
				<< health << std::endl;
		}
		else
		{
			health += amount;
			std::cout << name << " healed by " << amount
				<< " health points, new Health: " << health << std::endl;
		}
	}

	void takeDamage(int amount)
	{
		if (amount < 0)
		{
			std::cout << "it has no effect!" << std::endl;
		}

		if (health - amount < 0)
		{
			health = 0;
			std::cout << name << " took " << amount
				<< " damage and died!" << std::endl;
		}
		else
		{
			health -= amount;
			std::cout << name << " took " << amount
				<< " damage, new Health: " << health << std::endl;
		}
	}

	void attackEnemy(Character enemy)
	{
		std::cout << name << " attacks " << enemy.name << " >> ";
		enemy.takeDamage(attack - enemy.defense);
	}
};


int main()
{
	Character hero("Hero", 100, 100, 20, 10);
	Character monster("Goblin", 50, 50, 15, 5);

	std::cout << "[~] Initial state:\n" << std::endl;
	hero.displayInfo();
	monster.displayInfo();

	std::cout << "\n[~] Hero attack monster:\n" << std::endl;
	hero.attackEnemy(monster);

	std::cout << "\n[~] Monster heals 10 hp:\n" << std::endl;
	monster.heal(10);

	std::cout << "\n[~] End state:\n" << std::endl;
	hero.displayInfo();
	monster.displayInfo();
	std::cout << std::endl;

	return 0;

}