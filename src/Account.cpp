#include "Account.h"

Account::Account(const std::string& number, const std::string& ownerName, double startBalance)
    : number_(number), ownerName_(ownerName), balance_(startBalance) {
    if (startBalance > 0) {
        addTransaction(TransactionType::Deposit, startBalance, "Начальный баланс");
    }
}

const std::string& Account::getNumber() const {
    return number_;
}

const std::string& Account::getOwnerName() const {
    return ownerName_;
}

double Account::getBalance() const {
    return balance_;
}

bool Account::deposit(double amount, const std::string& comment) {
    if (amount <= 0) {
        return false;
    }
    balance_ += amount;
    addTransaction(TransactionType::Deposit, amount, comment);
    return true;
}

bool Account::withdraw(double amount, const std::string& comment) {
    if (amount <= 0 || !canWithdraw(amount)) {
        return false;
    }
    balance_ -= amount;
    addTransaction(TransactionType::Withdrawal, amount, comment);
    return true;
}

bool Account::canDebit(double amount) const {
    return canWithdraw(amount);
}

bool Account::chargeFee(double fee, const std::string& reason) {
    if (fee <= 0) {
        return true;
    }
    if (!canWithdraw(fee)) {
        return false;
    }
    balance_ -= fee;
    addTransaction(TransactionType::Fee, fee, reason);
    return true;
}

bool Account::transferOut(double amount, const std::string& toAccountNumber) {
    if (amount <= 0 || !canWithdraw(amount)) {
        return false;
    }
    balance_ -= amount;
    addTransaction(TransactionType::TransferOut, amount, "На счет " + toAccountNumber);
    return true;
}

void Account::transferIn(double amount, const std::string& fromAccountNumber) {
    if (amount <= 0) {
        return;
    }
    balance_ += amount;
    addTransaction(TransactionType::TransferIn, amount, "Со счета " + fromAccountNumber);
}

const MyContainer<Transaction>& Account::getHistory() const {
    return history_;
}

bool Account::canWithdraw(double amount) const {
    return amount >= 0 && balance_ >= amount;
}

void Account::addTransaction(TransactionType type, double amount, const std::string& comment) {
    history_.push_back(Transaction(type, amount, comment));
}
