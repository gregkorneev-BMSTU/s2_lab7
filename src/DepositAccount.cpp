#include "DepositAccount.h"

#include <sstream>

DepositAccount::DepositAccount(const std::string& number,
                               const std::string& ownerName,
                               double startBalance,
                               int termMonths)
    : Account(number, ownerName, startBalance), termMonths_(termMonths), locked_(true) {}

std::string DepositAccount::getType() const {
    return "DepositAccount";
}

std::string DepositAccount::getDisplayType() const {
    return "Депозитный счет";
}

std::string DepositAccount::getSpecialInfo() const {
    std::ostringstream out;
    out << "Срок: " << termMonths_ << " мес., статус: "
        << (locked_ ? "заблокирован" : "разблокирован");
    return out.str();
}

void DepositAccount::unlock() {
    locked_ = false;
}

bool DepositAccount::canWithdraw(double amount) const {
    if (locked_) {
        return false;
    }
    return Account::canWithdraw(amount);
}
