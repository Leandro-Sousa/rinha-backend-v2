#pragma once

#include <memory>
#include <vector>

#include "balance.cpp"
#include "transaction.cpp"

class AccountStatement
{
public:
    Balance balance;
    TransactionList latestTransactions;

    AccountStatement(const Balance &balance, const TransactionList &latestTransactions)
    : balance(std::move(balance)), latestTransactions(std::move(latestTransactions))
    {

    }
};
