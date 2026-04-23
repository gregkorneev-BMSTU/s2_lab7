#include "CommissionCalculator.h"

#include "Account.h"

double CommissionCalculator::calculateTransferFee(const Account& from,
                                                  const Account& to,
                                                  double amount) const {
    if (amount <= 0) {
        return 0;
    }

    // Простое учебное правило.
    double rate = 0.01;
    if (from.getType() == to.getType()) {
        rate = 0.005;
    }

    return amount * rate;
}
