#pragma once

#include <cstdint>
#include <drogon/orm/DbClient.h>

#include <src/common/result.cpp>
#include <src/models/balance.cpp>

class BalanceRepository
{
public:
    BalanceRepository(const drogon::orm::DbClientPtr &dbClientPtr) : _dbClientPtr(dbClientPtr)
    {
    }

    std::shared_ptr<drogon::orm::Transaction> newTransaction() const
    {
        return this->_dbClientPtr->newTransaction();
    }

    std::optional<Balance> getByCustomerId(std::int32_t customerId) const
    {
        static const std::string command = "SELECT \"limit\", amount FROM balances WHERE customer_id = $1;";
        const auto result = this->_dbClientPtr->execSqlSync(command, customerId);
        if(!result.empty())
        {
            const auto row = result[0];
            return Balance(row["limit"].as<std::int32_t>(), row["amount"].as<std::int32_t>());
        }
     
        return std::nullopt;
    }

    std::optional<Balance> getByCustomerIdForUpdate(std::int32_t customerId, std::shared_ptr<drogon::orm::Transaction> &databaseTransaction) const
    {
        static const std::string command = "SELECT \"limit\", amount FROM balances WHERE customer_id = $1 FOR UPDATE;";
        const auto result = databaseTransaction->execSqlAsyncFuture(command, customerId).get();
        if(!result.empty())
        {
            const auto row = result[0];
            return Balance(row["limit"].as<std::int32_t>(), row["amount"].as<std::int32_t>());
        }
     
        return std::nullopt;
    }

    bool update(std::int32_t customerId, std::int32_t currentBalance, std::int32_t newBalance) const
    {
        static const std::string command = "UPDATE balances SET amount = $1 WHERE customer_id = $2 AND amount = $3;";
        const auto result = this->_dbClientPtr->execSqlAsyncFuture(command, newBalance, customerId, currentBalance).get();
        return (result.affectedRows() == 1);
    }

    std::future<drogon::orm::Result> update(std::int32_t customerId, std::int32_t newBalance, std::shared_ptr<drogon::orm::Transaction> &databaseTransaction) const
    {
        static const std::string command = "UPDATE balances SET amount = $1 WHERE customer_id = $2;";
        return databaseTransaction->execSqlAsyncFuture(command, newBalance, customerId);
    }

private:
    drogon::orm::DbClientPtr _dbClientPtr;
};
