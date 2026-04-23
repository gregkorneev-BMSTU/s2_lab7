#pragma once

#include "Account.h"

class CreditAccount : public Account {
public:
    CreditAccount(const std::string& number,
                  const std::string& ownerName,
                  double startBalance,
                  double creditLimit);

    std::string getType() const override;
    std::string getDisplayType() const override;
    std::string getSpecialInfo() const override;
    double getCreditLimit() const;

protected:
    bool canWithdraw(double amount) const override;

private:
    double creditLimit_;
};
