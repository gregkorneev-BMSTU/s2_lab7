#pragma once

#include <iosfwd>
#include <string>

#include "MyContainer.h"
#include "Transaction.h"

class Account {
public:
    Account(const std::string& number, const std::string& ownerName, double startBalance);
    virtual ~Account() = default;

    const std::string& getNumber() const;
    const std::string& getOwnerName() const;
    double getBalance() const;

    virtual std::string getType() const = 0;
    virtual std::string getDisplayType() const = 0;
    virtual std::string getSpecialInfo() const = 0;

    bool deposit(double amount, const std::string& comment = "Пополнение");
    bool withdraw(double amount, const std::string& comment = "Снятие");

    bool canDebit(double amount) const;
    bool chargeFee(double fee, const std::string& reason);

    bool transferOut(double amount, const std::string& toAccountNumber);
    void transferIn(double amount, const std::string& fromAccountNumber);

    const MyContainer<Transaction>& getHistory() const;

protected:
    virtual bool canWithdraw(double amount) const;
    void addTransaction(TransactionType type, double amount, const std::string& comment);

    std::string number_;
    std::string ownerName_;
    double balance_;

private:
    // Композиция: история операций живет вместе со счетом.
    MyContainer<Transaction> history_;
};
