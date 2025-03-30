#include <iostream>
#include <string>


class Weapon
{
private:
    std::string name;
    int weight;
    int damage;
public:
    Weapon(const std::string& n, int w, int d)
        : name(n), weight(w), damage(d) {
        std::cout << "Weapon " << name << " created!\n";
    }

    std::string getName() const { return name; }

    int getWeight() const { return weight; }

    int getDamage() const { return damage; }

    Weapon operator+(const Weapon& other) const {
        return Weapon(name, weight, damage + other.damage);
    }

    bool operator>(const Weapon& other) const {
        return damage > other.damage;
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", Weight: " << weight
            << ", Damage: " << damage << std::endl;
    }
};


int main() {
    std::cout << "\n[~] Initial weapons:\n" << std::endl;
    Weapon sword("Sword", 5, 10);
    Weapon dagger("Dagger", 2, 5);

    std::cout << "\n[~] Information about weapons:\n" << std::endl;
    sword.displayInfo();
    dagger.displayInfo();

    std::cout << "\n[~] Combined weapon:\n" << std::endl;
    Weapon combined = sword + dagger;
    combined.displayInfo();

    std::cout << "\n[~] Comparing weapons:\n" << std::endl;
    if (sword > dagger) {
        std::cout << sword.getName() << " is stronger than " << dagger.getName() << std::endl;
    }
    else {
        std::cout << dagger.getName() << " is stronger than " << sword.getName() << std::endl;
    }
    std::cout << std::endl;

    return 0;
}
