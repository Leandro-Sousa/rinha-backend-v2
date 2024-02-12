#pragma once

#include <cstdint>
#include <Poco/Data/Session.h>
#include <Poco/Data/SessionPool.h>

#include <src/common/result.cpp>
#include <src/models/balance.cpp>

class BalanceRepository
{
public:
    BalanceRepository(const std::shared_ptr<Poco::Data::SessionPool> &sessionPool) : _sessionPool(sessionPool)
    {
    }

    std::optional<Balance> getByCustomerId(std::int32_t id) const
    {
        auto session = this->_sessionPool->get();
        Poco::Data::Statement statement(session);
        Balance balance;
        statement << "SELECT \"limit\", amount FROM balances WHERE customer_id = ?;",
            Poco::Data::Keywords::into(balance.limit),
            Poco::Data::Keywords::into(balance.amount),
            Poco::Data::Keywords::use(id);

        auto rowCount = statement.execute();

        if (rowCount == 1)
        {
            return balance;
        }

        return std::nullopt;
    }

    bool update(std::int32_t customerId, std::int32_t currentBalance, std::int32_t newBalance) const
    {
        auto session = this->_sessionPool->get();
        return update(customerId, currentBalance, newBalance, session);
    }

    bool update(std::int32_t customerId, std::int32_t currentBalance, std::int32_t newBalance, Poco::Data::Session &session) const
    {
        Poco::Data::Statement statement(session);
        statement << "UPDATE balances SET amount = ? WHERE customer_id = ? AND amount = ?;",
            Poco::Data::Keywords::use(newBalance),
            Poco::Data::Keywords::use(customerId),
            Poco::Data::Keywords::use(currentBalance);

        auto rowCount = statement.execute();

        return (rowCount == 1);
    }

private:
    std::shared_ptr<Poco::Data::SessionPool> _sessionPool;
};
