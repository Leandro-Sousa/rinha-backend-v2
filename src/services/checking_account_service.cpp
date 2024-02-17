#pragma once
#include <iostream>

#include <src/common/result.cpp>
#include <src/models/account_statement.cpp>
#include <src/models/balance.cpp>
#include <src/models/transaction.cpp>
#include <src/repositories/balance_repository.cpp>
#include <src/repositories/transaction_repository.cpp>

enum TransactionErrors
{
    BALANCE_NOT_FOUND = 1,
    INSUFFICIENT_FUNDS = 2
};

class CheckingAccountService
{
public:
    CheckingAccountService(
        const BalanceRepository &balanceRepository,
        const TransactionRepository &transactionRepository) : _balanceRepository(balanceRepository), _transactionRepository(transactionRepository)
    {
    }

    const Result<Balance> registerTransaction(std::int32_t customerId, const Transaction &transaction) const
    {
        bool hasSuccess = false;
        Result<Balance> result;
        do
        {
            hasSuccess = this->tryRegisterTransaction(customerId, transaction, result);
        } while (!hasSuccess);
        return result;
    }

    // const Result<Balance> registerTransaction(std::int32_t customerId, const Transaction &transaction) const
    // {
    //     auto currentBalanceOption = this->_balanceRepository.getByCustomerIdForUpdate(customerId, dbTransaction);

    //     if (!currentBalanceOption.has_value())
    //     {
    //         dbTransaction->rollback();
    //         return Result<Balance>::fail("BALANCE_NOT_FOUND", TransactionErrors::BALANCE_NOT_FOUND);
    //     }

    //     auto currentBalance = currentBalanceOption.value();
    //     auto transactionAmount = this->getTransactionAmountWithSign(transaction);
    //     std::int32_t newBalance = (currentBalance.amount + transactionAmount);

    //     if ((newBalance + currentBalance.limit) < 0)
    //     {
    //         dbTransaction->rollback();
    //         return Result<Balance>::fail("INSUFFICIENT_FUNDS", TransactionErrors::INSUFFICIENT_FUNDS);
    //     }

    //     this->_balanceRepository.update(customerId, newBalance, dbTransaction).wait();
    //     this->_transactionRepository.insert(customerId, transaction, dbTransaction).wait();
        
    //     currentBalance.amount = newBalance;

    //     return Result<Balance>::success(currentBalance);
    // }

    std::optional<AccountStatement> getAccountStatement(std::int32_t customerId) const
    {
        const auto balanceOption = this->_balanceRepository.getByCustomerId(customerId);
        if (!balanceOption.has_value())
        {
            return std::nullopt;
        }
        const auto balance = balanceOption.value();
        const auto latestTransactions = this->_transactionRepository.listLatestByCustomerId(customerId);
        return AccountStatement(balance, latestTransactions);
    }

private:
    BalanceRepository _balanceRepository;
    TransactionRepository _transactionRepository;

    std::int32_t getTransactionAmountWithSign(const Transaction &transaction) const
    {
        return transaction.amount * (transaction.type == "d" ? -1 : 1);
    }

    bool tryRegisterTransaction(std::int32_t customerId, const Transaction &transaction, Result<Balance> &result) const
    {
        auto currentBalanceOption = this->_balanceRepository.getByCustomerId(customerId);

        if (!currentBalanceOption.has_value())
        {
            result = Result<Balance>::fail("BALANCE_NOT_FOUND", TransactionErrors::BALANCE_NOT_FOUND);
            return true;
        }

        auto currentBalance = currentBalanceOption.value();
        auto transactionAmount = this->getTransactionAmountWithSign(transaction);
        std::int32_t newBalance = (currentBalance.amount + transactionAmount);

        if ((newBalance + currentBalance.limit) < 0)
        {
            result = Result<Balance>::fail("INSUFFICIENT_FUNDS", TransactionErrors::INSUFFICIENT_FUNDS);
            return true;
        }

        const auto isBalanceUpdated = this->_balanceRepository.update(customerId, currentBalance.amount, newBalance);
        if(isBalanceUpdated)
        {
            this->_transactionRepository.insert(customerId, transaction);
            currentBalance.amount = newBalance;
            result = Result<Balance>::success(currentBalance);
            return true;
        }

        return false;
    }
};
