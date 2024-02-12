#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <Poco/Timestamp.h>

struct Transaction
{
    std::int32_t amount;
    std::string description;
    std::string type;
    Poco::Timestamp createdAt;
};

using TransactionList = std::vector<Transaction>;