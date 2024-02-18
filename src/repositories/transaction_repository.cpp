#pragma once

#include <coroutine>
#include <cstdint>
#include <vector>
#include <Poco/Data/Session.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/Data/RecordSet.h>

#include <src/common/result.cpp>
#include <src/models/balance.cpp>
#include <src/models/transaction.cpp>

class TransactionRepository
{
public:
    TransactionRepository(const std::shared_ptr<Poco::Data::SessionPool> &sessionPool) : _sessionPool(sessionPool)
    {
    }

    void insert(std::int32_t customerId, const Transaction &transaction) const
    {
        static const std::string command = "INSERT INTO transactions(customer_id, \"type\", amount, description, created_at) VALUES(?, ?, ?, ?, ?);";
        auto session = this->_sessionPool->get();
        session << command, 
                   Poco::Data::Keywords::use(customerId), 
                   Poco::Data::Keywords::use(const_cast<std::string&>(transaction.type)), 
                   Poco::Data::Keywords::use(const_cast<std::int32_t&>(transaction.amount)),
                   Poco::Data::Keywords::use(const_cast<std::string&>(transaction.description)),
                   Poco::Data::Keywords::use(const_cast<std::int64_t&>(transaction.createdAt)),
                   Poco::Data::Keywords::now;
    }

    TransactionList listLatestByCustomerId(std::int32_t customerId) const
    {
        static const std::string command = "SELECT \"type\", amount, description, created_at FROM transactions WHERE customer_id = ? ORDER BY created_at DESC LIMIT 10;";
        auto session = this->_sessionPool->get();
        auto statement = (session << command, 
                                     Poco::Data::Keywords::use(customerId),
                                     Poco::Data::Keywords::now);

        Poco::Data::RecordSet rs(statement);
        auto transactions = std::vector<Transaction>();
        for(auto &row : rs)
        {
            transactions.emplace_back(
                row["amount"].convert<std::int32_t>(),
                row["description"].convert<std::string>(),
                row["type"].convert<std::string>(),
                row["created_at"].convert<std::int64_t>()
            );
        }

        return transactions;
    }

private:
    const std::shared_ptr<Poco::Data::SessionPool> _sessionPool;
};
