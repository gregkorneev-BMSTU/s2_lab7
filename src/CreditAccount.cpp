#include "CreditAccount.h"

#include <iomanip>
#include <sstream>

CreditAccount::CreditAccount(const std::string& number,
                             const std::string& ownerName,
                             double startBalance,
                             double creditLimit)
    : Account(number, ownerName, startBalance), creditLimit_(creditLimit) {}

std::string CreditAccount::getType() const {
    return "CreditAccount";
}

std::string CreditAccount::getDisplayType() const {
    return "Кредитный счет";
}

std::string CreditAccount::getSpecialInfo() const {
    std::ostringstream out;
    out << "Кредитный лимит: " << std::fixed << std::setprecision(2)
        << creditLimit_ << " RUB";
    return out.str();
}

double CreditAccount::getCreditLimit() const {
    return creditLimit_;
}

bool CreditAccount::canWithdraw(double amount) const {
    return amount >= 0 && (balance_ + creditLimit_) >= amount;
}
