#pragma once

class Account;

class CommissionCalculator {
public:
    double calculateTransferFee(const Account& from, const Account& to, double amount) const;
};
