#pragma once

#include <cstdint>
#include <memory>

class Balance
{
public:
    std::int32_t limit;
    std::int32_t amount;

    Balance(std::int32_t limit, std::int32_t amount) : limit(limit), amount(amount)
    {
    }
};
