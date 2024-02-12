#pragma once

#include <cstdint>
#include <vector>
#include <Poco/Data/Session.h>
#include <Poco/Data/SessionPool.h>

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
        auto session = this->_sessionPool->get();
        insert(customerId, transaction, session);
    }

    void insert(std::int32_t customerId, const Transaction &transaction, Poco::Data::Session &session) const
    {
        Poco::Data::Statement statement(session);
        auto type = transaction.type;
        auto amount = transaction.amount;
        auto description = transaction.description;
        std::int64_t createdAt = transaction.createdAt.epochMicroseconds();
        statement << "INSERT INTO transactions VALUES(?, ?, ?, ?, ?);",
            Poco::Data::Keywords::use(customerId),
            Poco::Data::Keywords::use(type),
            Poco::Data::Keywords::use(amount),
            Poco::Data::Keywords::use(description),
            Poco::Data::Keywords::use(createdAt),
            Poco::Data::Keywords::now;
    }

    TransactionList getByCustomerId(std::int32_t customerId, int limit) const
    {
        auto result = std::vector<Transaction>();
        auto session = this->_sessionPool->get();
        Poco::Data::Statement statement(session);
        std::string type;
        std::int32_t amount;
        std::string description;
        std::int64_t createdAt;
        statement << "SELECT \"type\", amount, description, created_at FROM transactions WHERE customer_id = ? ORDER BY created_at DESC LIMIT ?;",
            Poco::Data::Keywords::into(type),
            Poco::Data::Keywords::into(amount),
            Poco::Data::Keywords::into(description),
            Poco::Data::Keywords::into(createdAt),
            Poco::Data::Keywords::use(customerId),
            Poco::Data::Keywords::use(limit),
            Poco::Data::Keywords::range(0, 1);

        while (!statement.done())
        {
            auto rowCount = statement.execute();

            if (rowCount > 0)
            {
                Transaction transaction{
                    .amount = amount,
                    .description = description,
                    .type = type,
                    .createdAt = createdAt};
                result.push_back(transaction);
            }
        }
        return result;
    }

private:
    std::shared_ptr<Poco::Data::SessionPool> _sessionPool;
};
