#pragma once
#include "Logger.h"
#include "inventory.h"
#include <fstream>
#include <string>


class Entity
{
protected:
	size_t id;
	std::string type;
	std::string name;
	int health;
	int damage;
	int defense;
	int expByKill;

	Logger<Entity> logger;
public:
	Entity() 
		: id(0), type("Unknown"), name("Unknown"), health(0), damage(0), defense(0), expByKill(0) {}

	Entity(const Entity& other)
		: id(other.id), type(other.type), name(other.name), health(other.health), damage(other.damage), defense(other.defense), expByKill(other.expByKill), logger("Entity") {
		logger.debug("Entity<" + std::to_string(id) + "> copied");
	}

	Entity(int id, std::string type, std::string name, int health, int damage, int defense, int expByKill)
		: id(id), type(type), name(name), health(health), damage(damage), defense(defense), expByKill(expByKill) {
		logger.debug("Entity<" + std::to_string(id) + "> created");
	}

	~Entity() {
		logger.debug("Entity<" + std::to_string(id) + "> destroyed");
	}

	size_t getId() const { return id; }
	std::string getType() const { return type; }
	std::string getName() const { return name; }
	int getHealth() const { return health; }
	int getDamage() const { return damage; }
	int getDefense() const { return defense; }
	int getExpByKill() const { return expByKill; }

	bool isAlive() const { return health > 0; }

	virtual void attack(Entity& target) {
		logger.debug("Entity<" + std::to_string(id) + "> attacks Entity<" + std::to_string(target.id) + ">");

		if (!target.isAlive()) {
			logger.debug("Entity<" + std::to_string(target.id) + "> can't be attacked because it is already dead");
			return;
		}
		std::cout << "\033[34m" << "[~] " << getName() << " attacking " << target.getName() << "\033[0m" << std::endl;

		if (rand() % 100 < 25)
		{
			logger.debug("Entity<" + std::to_string(target.id) + "> dodged the attack");
			std::cout << "\033[31m" << "[-] " << target.getName() << " dodged the attack " << getName() << "\033[0m" << std::endl;
			return;
		}

		target.takeDamage(damage);
	}

	virtual void takeDamage(int amount)
	{
		if (!isAlive()) {
			logger.debug("Entity<" + std::to_string(id) + "> can't take damage because it is already dead");
			return;
		}

		int damage = amount - defense;
		if (damage <= 0) {
			logger.debug("Entity<" + std::to_string(id) + "> takes no damage");
			std::cout << "\033[31m" << "[-] " << getName() << " takes no damage" << "\033[0m" << std::endl;
			return;
		}

		if (health - damage < 0) {
			logger.debug("Entity<" + std::to_string(id) + "> takes " + std::to_string(health) + " damage and died");
			health = 0;
			std::cout << "\033[32m" << "[+] " << getName() << " takes " << damage << " damage and died" << "\033[0m" << std::endl;
			return;
		}

		logger.debug("Entity<" + std::to_string(id) + "> takes " + std::to_string(damage) + " damage, new hp: " + std::to_string(health - damage));
		health -= damage;
		std::cout << "\033[32m" << "[+] " << getName() << " takes " << damage << " damage, " << health << " hp left" << "\033[0m" << std::endl;
	}

	virtual void display() {
		logger.debug("Displaying Entity<" + std::to_string(id) + ">(" + name + ") stats");

		std::cout << "[~] " + type + " " + name + " stats:" << std::endl;
		std::cout << "- Type    : "  << type    << std::endl;
		std::cout << "- Name    : "  << name    << std::endl;
		std::cout << "- Health  : "  << health  << std::endl;
		std::cout << "- Damage  : "  << damage  << std::endl;
		std::cout << "- Defense : "  << defense << std::endl;
	}

	virtual void save(std::ofstream& file) {
		size_t strSize = type.size();
		file.write(reinterpret_cast<const char*>(&strSize), 1);
		file.write(type.c_str(), strSize);
		strSize = name.size();
		file.write(reinterpret_cast<const char*>(&strSize), 1);
		file.write(name.c_str(), strSize);
		file.write(reinterpret_cast<const char*>(&health), 2);
		file.write(reinterpret_cast<const char*>(&damage), 2);
		file.write(reinterpret_cast<const char*>(&defense), 2);
		file.write(reinterpret_cast<const char*>(&expByKill), 2);
	}

	virtual void load(std::ifstream& file) {
		size_t strSize = 0;
		file.read(reinterpret_cast<char*>(&strSize), 1);
		type.resize(strSize);
		file.read(&type[0], strSize);
		file.read(reinterpret_cast<char*>(&strSize), 1);
		name.resize(strSize);
		file.read(&name[0], strSize);
		file.read(reinterpret_cast<char*>(&health), 2);
		file.read(reinterpret_cast<char*>(&damage), 2);
		file.read(reinterpret_cast<char*>(&defense), 2);
		file.read(reinterpret_cast<char*>(&expByKill), 2);
	}
};


class Character : public Entity
{
private:
	int level;
	int experience;

	std::unique_ptr<Inventory> inventory;
public:
	Character() : Entity(),
		level(0),
		experience(0),
		inventory(std::make_unique<Inventory>()) {}

	Character(std::string name, int health, int damage, int defense, int level, int experience)
		: Entity(0, "Character", name, health, damage, defense, 0),
		level(level),
		experience(experience),
		inventory(std::make_unique<Inventory>()) {}

	int getLevel() const { return level; }
	int getExperience() const { return experience; }

	void levelUp() {
		while (experience >= (level + 1) * 100) {
			std::cout << "\033[32m" << "[+] Leveled up" << "\033[0m" << std::endl;
			logger.debug("Character<" + std::to_string(id) + "> leveled up");
			level++;
			experience -= (level + 1) * 100;
		}
	}

	void gainExperience(int amount) {
		logger.debug("Character<" + std::to_string(id) + "> gained " + std::to_string(amount) + " experience");
		std::cout << "\033[32m" << "[+] Gained " << amount << " experience" << "\033[0m" << std::endl;
		experience += amount;
		levelUp();
	}

	void takeItem(const Item& item) { 
		logger.debug(name + " took item " + item.getName());
		inventory->addItem(std::make_shared<Item>(item)); 
	}

	void useItem(const std::string& name) {
		if (!inventory->useItem(name)) {
			logger.debug("Character<" + std::to_string(id) + "> tried to use " + name + " but it was not found in inventory");
			std::cout << "\033[31m" << "[~] Item " << name << " not found in inventory" << "\033[0m" << std::endl;
		}
		std::cout << "\033[32m" << "[~] Item " << name << " successfully used" << "\033[0m" << std::endl;
	}

	void heal() {
		if (!inventory->hasItem("Heal Potion")) {
			logger.debug("Character<" + std::to_string(id) + "> tried to use Heal Potion but it was not found in inventory");
			std::cout << "\033[31m" << "[~] Item Heal Potion not found in inventory" << "\033[0m" << std::endl;
			return;
		}
		logger.debug("Character<" + std::to_string(id) + "> used Heal Potion");
		std::cout << "\033[32m" << "[~] Item Heal Potion successfully used, healed 25hp" << "\033[0m" << std::endl;
		inventory->useItem("Heal Potion");
	}

	void attack(Entity& target) {
		Entity::attack(target);
		
		if (!target.isAlive())
			gainExperience(target.getExpByKill());
	}

	void display() override {
		Entity::display();
		std::cout << "- Level   : " << level      << std::endl;
		std::cout << "- Exp     : " << experience << "/" << (level + 1) * 100 << std::endl;
	}

	void save(std::ofstream& file) override {
		Entity::save(file);
		file.write(reinterpret_cast<const char*>(&level), 1);
		file.write(reinterpret_cast<const char*>(&experience), 2);
		inventory->save(file);

	}

	void load(std::ifstream& file) override {
		Entity::load(file);
		file.read(reinterpret_cast<char*>(&level), 1);
		file.read(reinterpret_cast<char*>(&experience), 2);
		inventory->load(file);
	}
};


class Monster : public Entity
{
public:
	Monster() : Entity() {}

	Monster(int id, std::string name, int health, int damage, int defense, int expByKill)
		: Entity(id, "Monster", name, health, damage, defense, expByKill) {}
};


class Goblin : public Monster
{
public:
	Goblin() : Monster() {}

	Goblin(int id) : Monster(id, "Goblin", 50, 15, 5, 25) {}
};


class Skeleton : public Monster
{
public:
	Skeleton() : Monster() {}

	Skeleton(int id) : Monster(id, "Skeleton", 100, 20, 10, 50) {}
};


class Dragon : public Monster
{
public:
	Dragon() : Monster() {}

	Dragon(int id) : Monster(id, "Dragon", 200, 30, 15, 100) {}
};