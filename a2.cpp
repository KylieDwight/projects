#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

class Employee {
public:
    std::string id;
    std::vector<std::string> data;

    Employee(std::string id, std::vector<std::string> data) : id(id), data(data) {}

    void print() const {
        std::cout << "Employee ID: " << id << ", Data: ";
        for (const auto& item : data) {
            if (item.empty()) {
                std::cout << "-, ";
            } else {
                std::cout << item << ", ";
            }
        }
        std::cout << std::endl;
    }
};

class HashTable {
    static const int initialCapacity = 10;
    int capacity;
    int size;
    std::vector<std::string> keys;
    std::vector<Employee*> values;

    int hash(const std::string& key, int attempt) {
        int h1 = std::hash<std::string>{}(key) % capacity;
        int h2 = 1; // Set h2 to 1
        return (h1 + attempt * h2) % capacity;
    }

    bool isFull() {
        return size >= capacity * 0.75;
    }

    void doubleSize() {
        capacity *= 2;
        std::vector<std::string> newKeys(capacity);
        std::vector<Employee*> newValues(capacity, nullptr);

        for (int i = 0; i < keys.size(); ++i) {
            if (values[i] != nullptr) {
                int attempt = 0;
                std::string key = keys[i];
                while (true) {
                    int index = hash(key, attempt);
                    if (newValues[index] == nullptr) {
                        newKeys[index] = key;
                        newValues[index] = values[i];
                        break;
                    }
                    attempt++;
                }
            }
        }

        keys.swap(newKeys);
        values.swap(newValues);
    }

public:
    HashTable() : capacity(initialCapacity), size(0), keys(initialCapacity), values(initialCapacity, nullptr) {}

    ~HashTable() {
        for (Employee* emp : values) {
            delete emp;
        }
    }

    void insert(const std::string& key, Employee* value) {
        if (isFull()) {
            doubleSize();
        }

        int attempt = 0;
        while (true) {
            int index = hash(key, attempt);
            if (values[index] == nullptr || keys[index] == key) {
                keys[index] = key;
                values[index] = value;
                size++;
                break;
            }
            attempt++;
        }
    }

    Employee* get(const std::string& key) {
        int attempt = 0;
        while (true) {
            int index = hash(key, attempt);
            if (keys[index] == key) {
                return values[index];
            }
            if (values[index] == nullptr) {
                return nullptr;
            }
            attempt++;
        }
    }

    void remove(const std::string& key) {
        int attempt = 0;
        while (true) {
            int index = hash(key, attempt);
            if (keys[index] == key) {
                delete values[index];  // Free memory
                values[index] = nullptr; // Mark as deleted
                size--;
                break;
            }
            if (values[index] == nullptr) {
                break;
            }
            attempt++;
        }
    }

    void printTable() {
        for (int i = 0; i < capacity; ++i) {
            if (values[i] == nullptr) {
                std::cout << "-, ";
            } else {
                std::cout << keys[i] << ": ";
                values[i]->print();
            }
        }
        std::cout << std::endl;
    }
};

void readCSVAndPopulateHashTable(const std::string& filename, HashTable& table) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id, item;
        if (!std::getline(ss, id, ',')) {
            std::cerr << "Failed to read ID from line: " << line << std::endl;
            continue; // Skip this line or handle error as needed
        }

        std::vector<std::string> data;
        while (std::getline(ss, item, ',')) {
            data.push_back(item);
        }

        // Assuming Employee constructor and HashTable::insert are correctly defined elsewhere
        Employee* employee = new Employee(id, data);
        table.insert(id, employee);

    }
}

int main() {
    HashTable table;

    // Reading data from "us-contacts.csv"
    readCSVAndPopulateHashTable("us-contacts.csv", table);

    // Print the hash table
    table.printTable();

    // Demonstrate retrieval: Let's pick an ID from the provided data, e.g., "314" for James Butt
    std::string employeeIdToFind = "333"; 
    Employee* employee = table.get(employeeIdToFind);
    if (employee != nullptr) {
        std::cout << "Retrieved: ";
        employee->print();
    } else {
        std::cout << "Employee not found." << std::endl;
    }

    // Demonstrate removal of the same employee
    std::cout << "Removing employee ID " << employeeIdToFind << "..." << std::endl;
    table.remove(employeeIdToFind);

    // Attempt to retrieve the removed employee
    employee = table.get(employeeIdToFind);
    if (employee == nullptr) {
        std::cout << "Employee successfully removed." << std::endl;
    } else {
        std::cout << "Employee still present (removal failed): ";
        employee->print();
    }



    return 0;
}