#include <iostream>
#include <string>
#include <memory>
#include <list>


class Inventory
{
private:
    std::list<std::unique_ptr<std::string>> items;
public:
    Inventory()
    {
        std::cout << "\n[~] Inventory created" << std::endl;
    }

    ~Inventory()
    {
        std::cout << "\n[~] Inventory destroyed" << std::endl;
    }

    void addItem(const std::string& item)
    {
        items.push_back(std::make_unique<std::string>(item));
        std::cout << "\n[~] Item " << item << " added to inventory" << std::endl;
    }

    void displayInventory() const
    {
        if (items.empty()) {
            std::cout << "\n[~] Inventory is empty." << std::endl;
            return;
        }

        std::cout << "\n[~] Inventory contains:" << std::endl;
        for (const auto& item : items) {
            std::cout << "\t- " << *item << "\n";
        }
    }
};

int main() {
    Inventory playerInventory;

    playerInventory.addItem("Sword");
    playerInventory.addItem("Shield");
    playerInventory.addItem("Health Potion");

    playerInventory.displayInventory();

    return 0;
}