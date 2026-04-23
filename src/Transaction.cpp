#include "Transaction.h"

#include <sstream>

int Transaction::nextId_ = 1;

Transaction::Transaction(TransactionType type, double amount, const std::string& comment)
    : id_(nextId_), type_(type), amount_(amount), comment_(comment) {
    nextId_++;
}

int Transaction::getId() const {
    return id_;
}

TransactionType Transaction::getType() const {
    return type_;
}

double Transaction::getAmount() const {
    return amount_;
}

const std::string& Transaction::getComment() const {
    return comment_;
}

std::string Transaction::toString() const {
    std::ostringstream out;
    out << "#" << id_ << " " << typeToString(type_) << ": " << amount_ << " RUB";
    if (!comment_.empty()) {
        out << " (" << comment_ << ")";
    }
    return out.str();
}

std::string Transaction::typeToString(TransactionType type) {
    if (type == TransactionType::Deposit) {
        return "Пополнение";
    }
    if (type == TransactionType::Withdrawal) {
        return "Снятие";
    }
    if (type == TransactionType::TransferIn) {
        return "Входящий перевод";
    }
    if (type == TransactionType::TransferOut) {
        return "Исходящий перевод";
    }
    if (type == TransactionType::Fee) {
        return "Комиссия";
    }
    return "Операция";
}
