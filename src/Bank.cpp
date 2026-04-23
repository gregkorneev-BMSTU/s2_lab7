#include "Bank.h"

#include "Account.h"
#include "Client.h"

Bank::Bank(const std::string& name) : name_(name) {}

const std::string& Bank::getName() const {
    return name_;
}

void Bank::addDepartment(const std::string& name, const std::string& address) {
    departments_.emplace_back(name, address);
}

bool Bank::removeDepartment(const std::string& name) {
    for (std::size_t i = 0; i < departments_.size(); ++i) {
        if (departments_[i].getName() == name) {
            return departments_.removeAt(i);
        }
    }
    return false;
}

MyContainer<Department>& Bank::getDepartments() {
    return departments_;
}

const MyContainer<Department>& Bank::getDepartments() const {
    return departments_;
}

void Bank::registerClient(Client& client) {
    clients_.push_back(&client);
}

bool Bank::transfer(Account& from, Account& to, double amount) {
    if (&from == &to || amount <= 0) {
        return false;
    }

    double fee = commissionCalculator_.calculateTransferFee(from, to, amount);

    if (!from.canDebit(amount + fee)) {
        return false;
    }

    if (!from.transferOut(amount, to.getNumber())) {
        return false;
    }

    if (!from.chargeFee(fee, "Комиссия за перевод")) {
        return false;
    }

    to.transferIn(amount, from.getNumber());
    return true;
}
