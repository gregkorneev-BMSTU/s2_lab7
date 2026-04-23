#include "Card.h"

#include "Account.h"

Card::Card(const std::string& cardNumber, Account& linkedAccount)
    : cardNumber_(cardNumber), account_(&linkedAccount) {}

const std::string& Card::getCardNumber() const {
    return cardNumber_;
}

bool Card::pay(double amount, const std::string& shopName) {
    if (account_ == nullptr) {
        return false;
    }
    return account_->withdraw(amount, "Оплата картой: " + shopName);
}

bool Card::cashWithdraw(double amount) {
    if (account_ == nullptr) {
        return false;
    }
    return account_->withdraw(amount, "Снятие наличных картой");
}
