#pragma once

#include <string>

class Department {
public:
    Department(const std::string& name, const std::string& address);

    const std::string& getName() const;
    const std::string& getAddress() const;
    bool operator==(const Department& other) const;

private:
    std::string name_;
    std::string address_;
};
