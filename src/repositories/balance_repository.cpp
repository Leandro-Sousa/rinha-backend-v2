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

    std::optional<Balance> getByCustomerId(std::int32_t customerId) const
    {
        static const std::string command = "SELECT \"limit\", amount FROM balances WHERE customer_id = ?;";
        auto session = this->_sessionPool->get();
        std::int32_t limit;
        std::int32_t amount;
        auto statement = (session << command, 
                                     Poco::Data::Keywords::into(limit), 
                                     Poco::Data::Keywords::into(amount), 
                                     Poco::Data::Keywords::use(customerId),
                                     Poco::Data::Keywords::range(0, 1));

        const auto rowCount = statement.execute();

        if(rowCount == 1)
        {
            return Balance(limit, amount);
        }
     
        return std::nullopt;
    }

    bool update(std::int32_t customerId, std::int32_t currentBalance, std::int32_t newBalance) const
    {
        static const std::string command = "UPDATE balances SET amount = ? WHERE customer_id = ? AND amount = ?;";
        auto session = this->_sessionPool->get();
        auto statement = (session << command, 
                                     Poco::Data::Keywords::use(newBalance), 
                                     Poco::Data::Keywords::use(customerId), 
                                     Poco::Data::Keywords::use(currentBalance));

        const auto rowCount = statement.execute();

        return (rowCount == 1);
    }

private:
    const std::shared_ptr<Poco::Data::SessionPool> _sessionPool;
};
