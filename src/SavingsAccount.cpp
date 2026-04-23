#include "SavingsAccount.h"

#include <iomanip>
#include <sstream>

SavingsAccount::SavingsAccount(const std::string& number,
                               const std::string& ownerName,
                               double startBalance,
                               double annualRatePercent)
    : Account(number, ownerName, startBalance), annualRatePercent_(annualRatePercent) {}

std::string SavingsAccount::getType() const {
    return "SavingsAccount";
}

std::string SavingsAccount::getDisplayType() const {
    return "Сберегательный счет";
}

std::string SavingsAccount::getSpecialInfo() const {
    std::ostringstream out;
    out << "Ставка: " << std::fixed << std::setprecision(2)
        << annualRatePercent_ << "% годовых";
    return out.str();
}

void SavingsAccount::addMonthlyInterest() {
    double monthly = balance_ * (annualRatePercent_ / 100.0) / 12.0;
    if (monthly > 0) {
        deposit(monthly, "Месячные проценты");
    }
}
