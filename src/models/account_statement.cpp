#pragma once

#include <memory>
#include <vector>

#include "balance.cpp"
#include "transaction.cpp"

struct AccountStatement
{
    Balance balance;
    TransactionList latestTransactions;
};
