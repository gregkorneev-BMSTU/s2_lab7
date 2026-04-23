#pragma once

#include <string>

enum class TransactionType {
    Deposit,
    Withdrawal,
    TransferIn,
    TransferOut,
    Fee
};

class Transaction {
public:
    Transaction(TransactionType type, double amount, const std::string& comment);

    int getId() const;
    TransactionType getType() const;
    double getAmount() const;
    const std::string& getComment() const;

    std::string toString() const;

private:
    static std::string typeToString(TransactionType type);

    static int nextId_;
    int id_;
    TransactionType type_;
    double amount_;
    std::string comment_;
};
