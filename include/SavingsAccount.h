#pragma once

#include "Account.h"

class SavingsAccount : public Account {
public:
    SavingsAccount(const std::string& number,
                   const std::string& ownerName,
                   double startBalance,
                   double annualRatePercent);

    std::string getType() const override;
    std::string getDisplayType() const override;
    std::string getSpecialInfo() const override;

    void addMonthlyInterest();

private:
    double annualRatePercent_;
};
