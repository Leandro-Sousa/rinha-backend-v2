#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

class Transaction
{
public:
    std::int32_t amount;
    std::string description;
    std::string type;
    std::int64_t createdAt;

    Transaction(std::int32_t amount, const std::string& description, const std::string& type, std::int64_t createdAt)
    : amount(amount), description(std::move(description)), type(std::move(type)), createdAt(createdAt)
    {
    }
};

using TransactionList = std::vector<Transaction>;
