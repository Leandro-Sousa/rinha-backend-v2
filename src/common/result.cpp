#pragma once

#include <cstdint>
#include <optional>
#include <string>

template<typename T>
class Result
{
    public:
        Result(const Result& result): _data(std::move(result._data)), _failMessage(result._failMessage), _failCode(result._failCode)
        {
        }

        static Result<T> success(const T& data)
        {
            return Result(data);
        }

        static Result<T> fail(const std::string& failMessage)
        {
            return Result(failMessage);
        }

        static Result<T> fail(const std::string& failMessage, std::int32_t failCode)
        {
            return Result(failMessage, failCode);
        }

        bool isSuccess() const
        {
            return !this->isFail();
        }

        bool isFail() const
        {
            return this->_failMessage.has_value();
        }

        constexpr T&& getData() 
        {
            return std::move(this->_data);
        }

        std::string getFailMessage() const
        {
            return this->_failMessage.value();
        }

        std::int32_t getFailCode() const
        {
            return this->_failCode.value_or(0);
        }

    private:
        T _data;
        std::optional<std::string> _failMessage;
        std::optional<std::int32_t> _failCode;

        Result(const T& data): _data(std::move(data)), _failMessage(std::nullopt), _failCode(std::nullopt)
        {
        }

        Result(const std::string &failMessage): _failMessage(failMessage), _failCode(std::nullopt)
        {
        }

        Result(const std::string &failMessage, std::int32_t failCode): _failMessage(failMessage), _failCode(failCode)
        {
        }
};
