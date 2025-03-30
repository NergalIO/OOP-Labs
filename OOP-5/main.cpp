#include <iostream>
#include <vector>
#include <string>


template <typename T>
class Queue {
private:
    std::vector<T> items;
    size_t frontIndex = 0;

public:
    void push(const T& item) {
        items.push_back(item);
        std::cout << "Added: " << item << " to queue.\n";
    }

    T pop() {
        T removedItem = items[frontIndex];
        frontIndex++;

        if (frontIndex > items.size() / 2) {
            items.erase(items.begin(), items.begin() + frontIndex);
            frontIndex = 0;
        }
        return removedItem;
    }

    T front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty! No front element.");
        }
        return items[frontIndex];
    }

    bool empty() const {
        return frontIndex >= items.size();
    }
};


int main()
{
    std::cout << "\n[~] Initializing queue:" << std::endl;
    Queue<int> intQueue;

    std::cout << "\n[~] Adding items to queue:\n" << std::endl;
    intQueue.push(1);
    intQueue.push(2);
    intQueue.push(3);

    std::cout << "\n[~] Getting items from queue:\n" << std::endl;
    std::cout << "\t- " << intQueue.pop() << std::endl;
    std::cout << "\t- " << intQueue.pop() << std::endl;
    std::cout << "\t- " << intQueue.pop() << std::endl;

    std::cout << std::endl;

    return 0;
}