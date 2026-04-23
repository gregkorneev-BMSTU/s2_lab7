#pragma once

#include <string>

#include "MyContainer.h"

class Account;

class Client {
public:
    Client(int id, const std::string& fullName);

    int getId() const;
    const std::string& getFullName() const;

    // Агрегация: клиент хранит ссылки на уже существующие счета.
    void addAccount(Account* account);
    bool removeAccount(Account* account);
    const MyContainer<Account*>& getAccounts() const;

    double totalBalance() const;

private:
    int id_;
    std::string fullName_;
    MyContainer<Account*> accounts_;
};
