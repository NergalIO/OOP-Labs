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

    ~Weapon() {
        std::cout << "Weapon " << name << " destroyed!\n";
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", Weight: " << weight
            << ", Damage: " << damage << std::endl;
    }
};


int main()
{
    std::cout << "\n[~] Initial weapons:\n" << std::endl;
    Weapon* sword = new Weapon("Sword", 5, 10);
    Weapon* dagger = new Weapon("Dagger", 2, 5);
    Weapon* axe = new Weapon("Axe", 8, 15);

    std::cout << "\n[~] Information about weapons:\n" << std::endl;
    sword->displayInfo();
    dagger->displayInfo();
    axe->displayInfo();

    std::cout << "\n[~] Deleting weapons:\n" << std::endl;
    delete sword;
    delete dagger;
    delete axe;
    std::cout << std::endl;

    return 0;
}