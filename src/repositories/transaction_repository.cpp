#pragma once

#include <coroutine>
#include <cstdint>
#include <vector>
#include <drogon/orm/DbClient.h>

#include <src/common/result.cpp>
#include <src/models/balance.cpp>
#include <src/models/transaction.cpp>

class TransactionRepository
{
public:
    TransactionRepository(const drogon::orm::DbClientPtr &dbClientPtr) : _dbClientPtr(dbClientPtr)
    {
    }

    void insert(std::int32_t customerId, const Transaction &transaction) const
    {
        static const std::string command = "INSERT INTO transactions(customer_id, \"type\", amount, description, created_at) VALUES($1, $2, $3, $4, $5);";
        this->_dbClientPtr->execSqlAsyncFuture(command, customerId, transaction.type, transaction.amount, transaction.description, transaction.createdAt).get();
    }

    TransactionList listLatestByCustomerId(std::int32_t customerId) const
    {
        static const std::string command = "SELECT \"type\", amount, description, created_at FROM transactions WHERE customer_id = $1 ORDER BY created_at DESC LIMIT 10;";
        const auto result = this->_dbClientPtr->execSqlAsyncFuture(command, customerId).get();

        auto transactions = std::vector<Transaction>();
        for(const auto &row : result)
        {
            transactions.emplace_back(
                row["amount"].as<std::int32_t>(),
                row["description"].as<std::string>(),
                row["type"].as<std::string>(),
                row["created_at"].as<std::int64_t>()
            );
        }

        return transactions;
    }

private:
    drogon::orm::DbClientPtr _dbClientPtr;
};
