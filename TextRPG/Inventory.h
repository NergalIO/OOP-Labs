#pragma once
#include "Logger.h"
#include <algorithm>
#include <functional>
#include <fstream>
#include <string>
#include <vector>
#include <memory>


class Item {
private:
    size_t id;
    std::string name;
    std::string description;
    size_t count;
    size_t maxCount;

public:
    // Конструкторы
    Item() : id(0), name(""), description(""), count(0), maxCount(0) {}
    Item(size_t id, const std::string& name, const std::string& description, size_t count = 1, size_t maxCount = 64)
        : id(id), name(name), description(description), count(count), maxCount(maxCount) {
        if (maxCount == 0) {
            throw std::invalid_argument("maxCount cannot be zero");
        }
        else if (count > maxCount) {
            count = maxCount;
        }
    }

    ~Item() {}

    // Геттеры
    size_t getId() const { return id; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    size_t getCount() const { return count; }

    // Сеттеры
    void setName(const std::string& name) { this->name = name; }
    void setDescription(const std::string& description) { this->description = description; }

    // Методы
    virtual void action() {}
    virtual bool use() {
        if (count > 0) {
            action();
            --count;
            return true;
        }
        return false;
    }

    void add(Item& item) {
        if (item.getName() == name) {
            if (item.getCount() + count > maxCount) {
                item.count -= maxCount - count;
                count = maxCount;
            }
            else {
                count += item.count;
                item.count = 0;
            }
        }
    }

    void display() const {
        std::cout << "\033[1,34m" << "[~] Item<" << id << ">:" << "\033[0m" << std::endl;

		std::cout << "\033[34m"   << "Name        : "      << name        << "\033[0m" << std::endl;
		std::cout << "\033[34m"   << "Description : "      << description << "\033[0m" << std::endl;
		std::cout << "\033[34m"   << "Value       : "      << count       << "/" << maxCount << "\033[0m" << std::endl;

    }

    void save(std::ofstream& file) const {
        file.write(reinterpret_cast<const char*>(&id), sizeof(id));
        size_t strSize = name.size();
        file.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
        file.write(name.c_str(), strSize);

        strSize = description.size();
        file.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
        file.write(description.c_str(), strSize);

        file.write(reinterpret_cast<const char*>(&count), sizeof(count));
        file.write(reinterpret_cast<const char*>(&maxCount), sizeof(maxCount));
    }

    void load(std::ifstream& file) {
        file.read(reinterpret_cast<char*>(&id), sizeof(id));
        size_t strSize;
        file.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
        name.resize(strSize);
        file.read(&name[0], strSize);

        file.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
        description.resize(strSize);
        file.read(&description[0], strSize);

        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        file.read(reinterpret_cast<char*>(&maxCount), sizeof(maxCount));
    }
};


class Inventory {
private:
    std::vector<std::shared_ptr<Item>> items;
    size_t maxSize;

    Logger<Inventory> logger;
public:
    Inventory(size_t maxSize = 32) : items(std::vector<std::shared_ptr<Item>>()), maxSize(maxSize) { 
        logger.debug("Inventory created"); 
    }

    ~Inventory() { 
        logger.debug("Inventory destroyed"); 
    }

    bool hasItem(const std::string& name) {
        logger.debug("Check if item " + name + " is in inventory");
        return std::find_if(items.begin(), items.end(), [&name](std::shared_ptr<Item> item) { return item->getName() == name; }) != items.end();
    }

    bool useItem(const std::string& item) {
        for (auto& it : items) {
            if (it->getName() == item) {
                if (it->use()) {
                    logger.debug("Used item: " + item);
                    return true;
                }
                break;
            }
        }
        logger.debug("Item not found or cannot be used: " + item);
        return false;
    }

    bool addItem(std::shared_ptr<Item> item) {
        logger.debug("Add item " + item->getName() + " to inventory");
        if (!hasItem(item->getName())) {
            items.push_back(item);
            return true;
        }
        for (auto& i : items) {
            if (i->getName() == item->getName()) {
                i->add(*item);

                if (item->getCount() > 0)
                    items.push_back(item);

                return true;
            }
        }
        return false;
    }

    size_t getSize() const { return items.size(); }

    void removeItem(size_t index) {
        items.erase(items.begin() + index);
        logger.debug("Remove item from inventory at index " + std::to_string(index));
    }

    std::shared_ptr<Item> getItem(size_t index) {
        logger.debug("Get item from inventory at index " + std::to_string(index));
        return items[index];
    }

    void display() {
        logger.debug("Display inventory");
        std::cout << "\033[1,34m" << "[~] Inventory:" << "\033[0m" << std::endl;
        for (auto& item : items) {
            item->display();
        }
    }

    void save(std::ofstream& file) {
        logger.debug("Save inventory");
        size_t itemCount = items.size();
        file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
        logger.debug("Saving " + std::to_string(itemCount) + " items");
        for (const auto& item : items) {
			logger.debug("Saving item " + item->getName());
            item->save(file);
        }
    }

    void load(std::ifstream& file) {
        logger.debug("Load inventory");
        size_t itemCount;
        file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
        items.clear();
        for (size_t i = 0; i < itemCount; ++i) {
            auto item = std::make_shared<Item>();
            item->load(file);
            items.push_back(item);
        }
    }
};