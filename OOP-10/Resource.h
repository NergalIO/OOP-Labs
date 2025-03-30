#pragma once
#include <string>
#include <iostream>
#include <fstream>

class Resource {
private:
    std::string resourceName;
    int accessLevel;

public:
    Resource(const std::string& resourceName, int accessLevel)
        : resourceName(resourceName), accessLevel(accessLevel) {
    }

    std::string getResourceName() const { return resourceName; }
    int getAccessLevel() const { return accessLevel; }

    void save(std::ofstream& out) const {
        size_t nameSize = resourceName.size();
        out.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
        out.write(resourceName.c_str(), nameSize);
        out.write(reinterpret_cast<const char*>(&accessLevel), sizeof(accessLevel));
    }

    void load(std::ifstream& in) {
        size_t nameSize;
        in.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        resourceName.resize(nameSize);
        in.read(&resourceName[0], nameSize);
        in.read(reinterpret_cast<char*>(&accessLevel), sizeof(accessLevel));
    }
};