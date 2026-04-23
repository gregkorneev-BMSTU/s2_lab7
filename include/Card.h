#pragma once

#include <string>

class Account;

class Card {
public:
    Card(const std::string& cardNumber, Account& linkedAccount);

    const std::string& getCardNumber() const;

    bool pay(double amount, const std::string& shopName);
    bool cashWithdraw(double amount);

private:
    std::string cardNumber_;

    // Ассоциация: карта знает счет, но не владеет им.
    Account* account_;
};
