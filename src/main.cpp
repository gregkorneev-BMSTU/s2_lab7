#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "Account.h"
#include "Bank.h"
#include "Card.h"
#include "Client.h"
#include "CreditAccount.h"
#include "DepositAccount.h"
#include "MyContainer.h"
#include "SavingsAccount.h"

namespace {

const std::string RESET  = "\033[0m";
const std::string BOLD   = "\033[1m";
const std::string CYAN   = "\033[36m";
const std::string GREEN  = "\033[32m";
const std::string RED    = "\033[31m";
const std::string YELLOW = "\033[33m";

std::size_t utf8DisplayWidth(const std::string& text) {
    std::size_t width = 0;

    for (unsigned char ch : text) {
        if ((ch & 0xC0) != 0x80) {
            ++width;
        }
    }

    return width;
}

template<typename T>
std::string toText(const T& value) {
    std::ostringstream out;
    out << value;
    return out.str();
}

void printCell(const std::string& text, std::size_t width) {
    std::cout << text;

    std::size_t visibleWidth = utf8DisplayWidth(text);
    if (visibleWidth < width) {
        std::cout << std::string(width - visibleWidth, ' ');
    }
}

template<typename T>
void printCell(const T& value, std::size_t width) {
    printCell(toText(value), width);
}

std::string formatMoney(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value << " RUB";
    return out.str();
}

void printLine(char ch = '=', int width = 80) {
    for (int i = 0; i < width; i++) {
        std::cout << ch;
    }
    std::cout << '\n';
}

void printCentered(const std::string& text, int width = 80) {
    int padding = static_cast<int>((width - static_cast<int>(utf8DisplayWidth(text))) / 2);
    if (padding < 0) {
        padding = 0;
    }
    std::cout << std::string(padding, ' ') << text << '\n';
}

void printSection(const std::string& title) {
    std::cout << '\n' << CYAN << BOLD;
    printLine('=');
    printCentered(title);
    printLine('=');
    std::cout << RESET;
}

void printSubsection(const std::string& title) {
    std::cout << '\n' << YELLOW << BOLD << title << RESET << '\n';
    printLine('-');
}

std::string transactionTypeToText(TransactionType type) {
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

void printInfo(const std::string& text) {
    std::cout << YELLOW << "[INFO]" << RESET << " " << text << '\n';
}

void printAccountCard(const Account& account) {
    printLine('-');
    printCell("Тип:", 16);
    std::cout << account.getDisplayType() << '\n';
    printCell("Номер:", 16);
    std::cout << account.getNumber() << '\n';
    printCell("Владелец:", 16);
    std::cout << account.getOwnerName() << '\n';
    printCell("Баланс:", 16);
    std::cout << formatMoney(account.getBalance()) << '\n';
    printCell("Особенность:", 16);
    std::cout << account.getSpecialInfo() << '\n';
    printLine('-');
}

void showAccountPolymorphically(const Account& account) {
    std::cout << "Работа через базовый тип Account:\n";
    printAccountCard(account);
}

void useDerivedFeature(Account& account) {
    std::cout << "Использование dynamic_cast: ";

    if (SavingsAccount* savings = dynamic_cast<SavingsAccount*>(&account)) {
        savings->addMonthlyInterest();
        std::cout << "для SavingsAccount вызван addMonthlyInterest().\n";
        return;
    }

    if (CreditAccount* credit = dynamic_cast<CreditAccount*>(&account)) {
        std::cout << "для CreditAccount получен лимит через getCreditLimit(): "
                  << formatMoney(credit->getCreditLimit()) << ".\n";
        return;
    }

    if (DepositAccount* deposit = dynamic_cast<DepositAccount*>(&account)) {
        deposit->unlock();
        std::cout << "для DepositAccount вызван unlock().\n";
        return;
    }

    std::cout << "тип не распознан.\n";
}

void printDepartments(const Bank& bank) {
    const MyContainer<Department>& deps = bank.getDepartments();

    printCell("№", 5);
    printCell("Отделение", 18);
    printCell("Адрес", 50);
    std::cout << '\n';
    printLine('-');

    std::size_t index = 1;
    for (auto it = deps.begin(); it != deps.end(); ++it) {
        printCell(index, 5);
        printCell(it->getName(), 18);
        printCell(it->getAddress(), 50);
        std::cout << '\n';
        ++index;
    }
}

void printOperationResult(const std::string& icon,
                          const std::string& text,
                          bool success) {
    std::string fullText = icon + " " + text;
    printCell(fullText, 62);
    std::cout << ' ';

    if (success) {
        std::cout << GREEN << "[OK]" << RESET;
    } else {
        std::cout << RED << "[ERROR]" << RESET;
    }

    std::cout << '\n';
}

void printClientSummary(const Client& client) {
    printLine('-');
    printCell("Клиент:", 14);
    std::cout << client.getFullName() << '\n';
    printCell("ID:", 14);
    std::cout << client.getId() << '\n';
    printCell("Счетов:", 14);
    std::cout << client.getAccounts().size() << '\n';
    printCell("Итого:", 14);
    std::cout << formatMoney(client.totalBalance()) << '\n';
    printLine('-');
}

void printHistoryTable(const Account& account) {
    printSubsection("История счета " + account.getNumber());

    const MyContainer<Transaction>& history = account.getHistory();

    if (history.empty()) {
        std::cout << "История операций пуста.\n";
        return;
    }

    printCell("ID", 6);
    printCell("Тип", 25);
    printCell("Сумма", 18);
    printCell("Комментарий", 33);
    std::cout << '\n';
    printLine('-');

    for (auto it = history.begin(); it != history.end(); ++it) {
        printCell(it->getId(), 6);
        printCell(transactionTypeToText(it->getType()), 25);
        printCell(formatMoney(it->getAmount()), 18);
        printCell(it->getComment(), 33);
        std::cout << '\n';
    }
}

} // namespace

int main() {
    printSection("БАНКОВСКАЯ СИСТЕМА (ВАРИАНТ 15)");

    Bank bank("BMSTU Bank");
    bank.addDepartment("Центральное", "Москва, ул. Бауманская, 5");
    bank.addDepartment("Северное", "Москва, ул. Лобненская, 12");
    bank.addDepartment("Южное", "Москва, Варшавское шоссе, 18");

    Client alice(1, "Алиса Иванова");
    Client bob(2, "Боб Петров");

    bank.registerClient(alice);
    bank.registerClient(bob);

    SavingsAccount aliceSavings("ACC-1001", alice.getFullName(), 50000, 6);
    CreditAccount bobCredit("ACC-2001", bob.getFullName(), 15000, 30000);
    DepositAccount aliceDeposit("ACC-3001", alice.getFullName(), 120000, 12);

    alice.addAccount(&aliceSavings);
    alice.addAccount(&aliceDeposit);
    bob.addAccount(&bobCredit);

    Card aliceCard("2200 1000 0000 0001", aliceSavings);

    printSection("1. РАБОТА С КОЛЛЕКЦИЕЙ ОТДЕЛЕНИЙ");
    printInfo("В банке используется собственная коллекция MyContainer<Department>.");
    printInfo("Для лабораторной 7 внешний интерфейс контейнера сохранен, "
              "но внутри данные хранятся блоками.");
    printDepartments(bank);

    Department searchDepartment("Северное", "Москва, ул. Лобненская, 12");
    auto foundDepartment = std::find(bank.getDepartments().begin(),
                                     bank.getDepartments().end(),
                                     searchDepartment);
    if (foundDepartment != bank.getDepartments().end()) {
        printInfo("Через std::find найдено отделение: " + foundDepartment->getName());
    }

    std::sort(bank.getDepartments().begin(),
              bank.getDepartments().end(),
              [](const Department& left, const Department& right) {
                  return left.getName() < right.getName();
              });
    printInfo("Отделения отсортированы по названию через std::sort.");
    printDepartments(bank);

    printOperationResult("🏢", "Удаление отделения Южное из коллекции",
                         bank.removeDepartment("Южное"));
    printDepartments(bank);

    printSection("2. НАЧАЛЬНОЕ СОСТОЯНИЕ СЧЕТОВ");
    printAccountCard(aliceSavings);
    printAccountCard(bobCredit);
    printAccountCard(aliceDeposit);

    printSection("3. ВЫПОЛНЕНИЕ ОПЕРАЦИЙ");
    printOperationResult("💳", "Оплата картой 1500 RUB (магазин: Книжный)",
                         aliceCard.pay(1500, "Книжный"));

    printOperationResult("💰", "Пополнение Savings на 3000 RUB",
                         aliceSavings.deposit(3000, "Внесение наличных"));

    printOperationResult("💸", "Снятие с Credit 20000 RUB",
                         bobCredit.withdraw(20000, "Покупка техники"));

    printOperationResult("⛔", "Попытка снять с Deposit 1000 RUB до разблокировки",
                         aliceDeposit.withdraw(1000, "Попытка досрочного снятия"));

    aliceDeposit.unlock();

    printOperationResult("🔓", "Снятие с Deposit 1000 RUB после разблокировки",
                         aliceDeposit.withdraw(1000, "Снятие после разблокировки"));

    printOperationResult("🔄", "Перевод 5000 RUB со Savings на Credit",
                         bank.transfer(aliceSavings, bobCredit, 5000));

    aliceSavings.addMonthlyInterest();
    printInfo("На сберегательный счет начислены месячные проценты.");

    printSection("4. ИТОГОВОЕ СОСТОЯНИЕ СЧЕТОВ");
    printAccountCard(aliceSavings);
    printAccountCard(bobCredit);
    printAccountCard(aliceDeposit);

    printSection("5. СВОДКА ПО КЛИЕНТАМ");
    printClientSummary(alice);
    printClientSummary(bob);

    printSection("6. ДЕМОНСТРАЦИЯ ПОЛИМОРФИЗМА");
    MyContainer<std::unique_ptr<Account>> trainingAccounts;
    trainingAccounts.push_back(
        std::make_unique<SavingsAccount>("ACC-4001", "Учебный клиент", 20000, 4.5));
    trainingAccounts.push_back(
        std::make_unique<CreditAccount>("ACC-4002", "Учебный клиент", 10000, 25000));
    trainingAccounts.push_back(
        std::make_unique<DepositAccount>("ACC-4003", "Учебный клиент", 50000, 9));

    for (std::unique_ptr<Account>& account : trainingAccounts) {
        showAccountPolymorphically(*account);
        useDerivedFeature(*account);
        std::cout << "Состояние после специфичного действия:\n";
        printAccountCard(*account);
        printLine('.');
    }

    printSection("7. ИСТОРИЯ ОПЕРАЦИЙ");
    printHistoryTable(aliceSavings);
    printHistoryTable(bobCredit);
    printHistoryTable(aliceDeposit);

    printSection("8. КОЛЛЕКЦИЯ С ПРОСТЫМ ТИПОМ");
    MyContainer<int> dailyCodes;
    dailyCodes.push_back(30);
    dailyCodes.push_back(10);
    dailyCodes.push_back(20);
    dailyCodes.push_back(40);

    std::cout << "Коды до pop_back(): ";
    for (auto it = dailyCodes.begin(); it != dailyCodes.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';

    dailyCodes.pop_back();
    std::cout << "После pop_back() последний код удален.\n";

    dailyCodes.removeAt(1);
    std::sort(dailyCodes.begin(), dailyCodes.end());

    std::cout << "Коды после сортировки: ";
    for (auto it = dailyCodes.begin(); it != dailyCodes.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';

    auto foundCode = std::find(dailyCodes.begin(), dailyCodes.end(), 20);
    if (foundCode != dailyCodes.end()) {
        std::cout << "Код 20 найден через std::find и удален через erase().\n";
        dailyCodes.erase(foundCode);
    }

    MyContainer<int> copiedCodes = dailyCodes;
    std::cout << "Коды в копии контейнера: ";
    for (const int code : copiedCodes) {
        std::cout << code << ' ';
    }
    std::cout << '\n';

    printSection("9. ИТОГ ПО БАНКУ");
    double totalBankBalance = alice.totalBalance() + bob.totalBalance();
    std::cout << BOLD << "Общий баланс по всем клиентам: " << RESET
              << formatMoney(totalBankBalance) << '\n';

    printSection("РАБОТА ПРОГРАММЫ ЗАВЕРШЕНА");
    return 0;
}
