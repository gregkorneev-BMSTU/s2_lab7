cmake_minimum_required(VERSION 3.16)
project(project_bank LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(project_bank
    src/main.cpp
    src/Transaction.cpp
    src/Account.cpp
    src/SavingsAccount.cpp
    src/CreditAccount.cpp
    src/DepositAccount.cpp
    src/CommissionCalculator.cpp
    src/Department.cpp
    src/Client.cpp
    src/Card.cpp
    src/Bank.cpp
)

target_include_directories(project_bank PRIVATE include)
