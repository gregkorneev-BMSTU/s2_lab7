#pragma once

#include <string>

#include "CommissionCalculator.h"
#include "Department.h"
#include "MyContainer.h"

class Account;
class Client;

class Bank {
public:
    Bank(const std::string& name);

    const std::string& getName() const;

    // Композиция: отделения принадлежат банку.
    void addDepartment(const std::string& name, const std::string& address);
    bool removeDepartment(const std::string& name);
    MyContainer<Department>& getDepartments();
    const MyContainer<Department>& getDepartments() const;

    void registerClient(Client& client);

    // Делегирование: часть работы (комиссия) отдается CommissionCalculator.
    bool transfer(Account& from, Account& to, double amount);

private:
    std::string name_;
    CommissionCalculator commissionCalculator_;
    MyContainer<Department> departments_;
    MyContainer<Client*> clients_;
};
