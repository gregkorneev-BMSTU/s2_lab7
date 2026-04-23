#include "Department.h"

Department::Department(const std::string& name, const std::string& address)
    : name_(name), address_(address) {}

const std::string& Department::getName() const {
    return name_;
}

const std::string& Department::getAddress() const {
    return address_;
}

bool Department::operator==(const Department& other) const {
    return name_ == other.name_ && address_ == other.address_;
}
