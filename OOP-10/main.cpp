#include "User.h"
#include "Resource.h"
#include "AccessControlSystem.h"
#include <iostream>
#include <vector>
#include <memory>

int main() {
    std::vector<std::unique_ptr<User>> users;

    users.push_back(std::make_unique<Student>("Student1", 3, "Group-1"));
    users.push_back(std::make_unique<Teacher>("Teacher1", 2, "Department-1"));
    users.push_back(std::make_unique<Administrator>("Administrator1", 3));

    for (const auto& user : users) {
        user->displayInfo();
    }

    AccessControlSystem<User, Resource> acs;

    for (auto& user : users) {
		acs.addUser(std::move(user));
	}

    acs.addResource(std::make_unique<Resource>("Resource1", 0));
    acs.addResource(std::make_unique<Resource>("Resource2", 1));
    acs.addResource(std::make_unique<Resource>("Resource3", 2));

    acs.save("save.bin");

	AccessControlSystem<User, Resource> loadedAcs;

    loadedAcs.load("save.bin");

    std::cout << "\033[34m[~] Searching user by ID<2>: \033[0m\n";
    auto user = loadedAcs.findUserById(2);
	if (user) {
		std::cout << "\033[32m[+] User found: \033[0m\n";
        user->displayInfo();
    }
    else {
        std::cout << "\033[31m[-] User not found: \033[0m\n";
    }

    std::cout << "\033[34m[~] Searching user by Name<Student1>: \033[0m\n";
    user = loadedAcs.findUserByName("Student1");
    if (user) {
        std::cout << "\033[32m[+] User found: \033[0m\n";
        user->displayInfo();
    }
    else {
        std::cout << "\033[31m[-] User not found: \033[0m\n";
    }

	std::cout << "\033[34m[~] Sorting users by AccessLevel: \033[0m\n";
	loadedAcs.sortUsersByAccessLevel();
    loadedAcs.displayUsers();

    return 0;
}