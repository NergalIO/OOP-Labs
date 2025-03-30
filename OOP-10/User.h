#pragma once
#include <string>
#include <iostream>
#include <fstream>

class User {
protected:
    size_t id;
    std::string name;
    int accessLevel;

public:
    User(std::string name, int accessLevel)
        : id(0), name(name), accessLevel(accessLevel) {
    }

    virtual ~User() = default;

    void setId(size_t id) { this->id = id; }

    size_t getId() const { return id; }
    std::string getName() const { return name; }
    int getAccessLevel() const { return accessLevel; }

    virtual void display() const = 0;
    virtual int getType() const = 0;
    virtual void displayInfo() const = 0;

    virtual void save(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&id), sizeof(id));
        size_t nameSize = name.size();
        out.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
        out.write(name.c_str(), nameSize);
        out.write(reinterpret_cast<const char*>(&accessLevel), sizeof(accessLevel));
    }

    virtual void load(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&id), sizeof(id));
        size_t nameSize;
        in.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        name.resize(nameSize);
        in.read(&name[0], nameSize);
        in.read(reinterpret_cast<char*>(&accessLevel), sizeof(accessLevel));
    }
};

class Student : public User {
private:
    std::string group;

public:
    Student(std::string name, int accessLevel, std::string group)
        : User(name, accessLevel), group(group) {
    }

    void display() const override {
        std::cout << "\n--------Student information--------" << std::endl;
        std::cout << "ID\t\t: " << getId() << std::endl;
        std::cout << "Name\t\t: " << getName() << std::endl;
        std::cout << "Group\t\t: " << group << std::endl;
        std::cout << "Access level\t: " << getAccessLevel() << std::endl;
        std::cout << "-----------------------------------\n" << std::endl;
    }

    void displayInfo() const override {
        display();
    }

    int getType() const override {
        return 0;
    }

    void save(std::ofstream& out) const override {
        User::save(out);
        size_t groupSize = group.size();
        out.write(reinterpret_cast<const char*>(&groupSize), sizeof(groupSize));
        out.write(group.c_str(), groupSize);
    }

    void load(std::ifstream& in) override {
        User::load(in);
        size_t groupSize;
        in.read(reinterpret_cast<char*>(&groupSize), sizeof(groupSize));
        group.resize(groupSize);
        in.read(&group[0], groupSize);
    }
};

class Teacher : public User {
private:
    std::string department;

public:
    Teacher(std::string name, int accessLevel, std::string department)
        : User(name, accessLevel), department(department) {
    }

    void display() const override {
        std::cout << "\n--------Teacher information--------" << std::endl;
        std::cout << "ID\t\t: " << getId() << std::endl;
        std::cout << "Name\t\t: " << getName() << std::endl;
        std::cout << "Department\t: " << department << std::endl;
        std::cout << "Access level\t: " << getAccessLevel() << std::endl;
        std::cout << "-----------------------------------\n" << std::endl;
    }

    void displayInfo() const override {
        display();
    }

    int getType() const override {
        return 1;
    }

    void save(std::ofstream& out) const override {
        User::save(out);
        size_t departmentSize = department.size();
        out.write(reinterpret_cast<const char*>(&departmentSize), sizeof(departmentSize));
        out.write(department.c_str(), departmentSize);
    }

    void load(std::ifstream& in) override {
        User::load(in);
        size_t departmentSize;
        in.read(reinterpret_cast<char*>(&departmentSize), sizeof(departmentSize));
        department.resize(departmentSize);
        in.read(&department[0], departmentSize);
    }
};

class Administrator : public User {
public:
    Administrator(std::string name, int accessLevel)
        : User(name, accessLevel) {
    }

    void display() const override {
        std::cout << "\n-----Administrator information-----" << std::endl;
        std::cout << "ID\t\t: " << getId() << std::endl;
        std::cout << "Name\t\t: " << getName() << std::endl;
        std::cout << "Access level\t: " << getAccessLevel() << std::endl;
        std::cout << "-----------------------------------\n" << std::endl;
    }

    void displayInfo() const override {
        display();
    }

    int getType() const override {
        return 2;
    }
};