#pragma once
#include "User.h"
#include "Resource.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <string>

template <typename TUser, typename TResource>
class AccessControlSystem {
private:
    std::vector<std::unique_ptr<TUser>> users;
    std::vector<std::unique_ptr<TResource>> resources;

public:
    AccessControlSystem() = default;
    ~AccessControlSystem() = default;

    void addUser(std::unique_ptr<TUser> user) {
        user->setId(users.size() + 1);
        users.push_back(std::move(user));
    }

    void addResource(std::unique_ptr<TResource> resource) {
        resources.push_back(std::move(resource));
    }

    bool checkAccess(const TUser& user, const TResource& resource) const {
        return user.getAccessLevel() >= resource.getAccessLevel();
    }

    void displayUsers() const {
        std::cout << "\n------------------Users------------------" << std::endl;
        for (const auto& user : users) {
            user->display();
        }
        std::cout << "-----------------------------------------\n" << std::endl;
    }

    void displayResources() const {
        std::cout << "\n----------------Resources----------------" << std::endl;
        for (const auto& resource : resources) {
            std::cout << "Resource Name: " << resource->getResourceName() << ", Access Level: " << resource->getAccessLevel() << std::endl;
        }
        std::cout << "-----------------------------------------\n" << std::endl;
    }

    void save(const std::string& filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            throw std::runtime_error("Failed to open file for saving");
        }

        size_t userCount = users.size();
        out.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));
        for (const auto& user : users) {
            int userType = user->getType();
            out.write(reinterpret_cast<const char*>(&userType), sizeof(userType));
            user->save(out);
        }

        size_t resourceCount = resources.size();
        out.write(reinterpret_cast<const char*>(&resourceCount), sizeof(resourceCount));
        for (const auto& resource : resources) {
            resource->save(out);
        }
    }

    void load(const std::string& filename) {
        std::ifstream in(filename, std::ios::binary);
        if (!in) {
            throw std::runtime_error("Failed to open file for loading");
        }

        size_t userCount;
        in.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));
        users.clear();
        for (size_t i = 0; i < userCount; ++i) {
            int userType;
            in.read(reinterpret_cast<char*>(&userType), sizeof(userType));
            std::unique_ptr<TUser> user;
            switch (userType) {
            case 0:
                user = std::make_unique<Student>("", 0, "");
                break;
            case 1:
                user = std::make_unique<Teacher>("", 0, "");
                break;
            case 2:
                user = std::make_unique<Administrator>("", 0);
                break;
            default:
                throw std::runtime_error("Unknown user type");
            }
            user->load(in);
            addUser(std::move(user));
        }

        size_t resourceCount;
        in.read(reinterpret_cast<char*>(&resourceCount), sizeof(resourceCount));
        resources.clear();
        for (size_t i = 0; i < resourceCount; ++i) {
            auto resource = std::make_unique<TResource>("", 0);
            resource->load(in);
            addResource(std::move(resource));
        }
    }

    TUser* findUserById(int id) const {
        for (const auto& user : users) {
            if (user->getId() == id) {
                return user.get();
            }
        }
        return nullptr;
    }

    TUser* findUserByName(const std::string& name) const {
        for (const auto& user : users) {
            if (user->getName() == name) {
                return user.get();
            }
        }
        return nullptr;
    }

    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(), [](const std::unique_ptr<TUser>& a, const std::unique_ptr<TUser>& b) {
            return a->getAccessLevel() < b->getAccessLevel();
            });
    }

    void sortUsersByName() {
        std::sort(users.begin(), users.end(), [](const std::unique_ptr<TUser>& a, const std::unique_ptr<TUser>& b) {
            return a->getName() < b->getName();
            });
    }
};