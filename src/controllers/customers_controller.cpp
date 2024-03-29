#include <iostream>
#include <chrono>
#include <format>
#include <drogon/HttpController.h>

#include <src/models/transaction.cpp>
#include <src/validators/transaction_body_validator.cpp>
#include <src/services/checking_account_service.cpp>

class CustomersController : public drogon::HttpController<CustomersController, false>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CustomersController::newTransaction, "/clientes/{id}/transacoes", drogon::Post);
    ADD_METHOD_TO(CustomersController::getStatement, "/clientes/{id}/extrato", drogon::Get);
    METHOD_LIST_END

    CustomersController(const CheckingAccountService &checkingAccountService) : _checkingAccountService(checkingAccountService)
    {
    }

    void newTransaction(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback, const std::int32_t &id) const
    {
        static auto transactionBodyValidator = TransactionBodyValidator();
        auto body = req->jsonObject();
        auto validationResult = transactionBodyValidator.validate(body);
        if (validationResult.hasSuccess())
        {
            auto transaction = this->jsonToTransaction(body);
            auto result = this->_checkingAccountService.registerTransaction(id, transaction);
            if (result.isSuccess())
            {
                auto balance = result.getData();
                auto balanceJson = this->balanceToJson(balance);
                auto response = drogon::HttpResponse::newHttpJsonResponse(balanceJson);
                response->setStatusCode(drogon::HttpStatusCode::k200OK);
                callback(response);
            }
            else
            {
                auto response = drogon::HttpResponse::newHttpResponse();
                switch (result.getFailCode())
                {
                case TransactionErrors::BALANCE_NOT_FOUND:
                    response->setStatusCode(drogon::HttpStatusCode::k404NotFound);
                    response->setBody(result.getFailMessage());
                    break;

                case TransactionErrors::INSUFFICIENT_FUNDS:
                    response->setStatusCode(drogon::HttpStatusCode::k422UnprocessableEntity);
                    response->setBody(result.getFailMessage());
                    break;

                default:
                    response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
                    break;
                }
                callback(response);
            }
        }
        else
        {
            auto response = drogon::HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::HttpStatusCode::k422UnprocessableEntity);
            response->setBody(validationResult.getErrorMessage());
            callback(response);
        }
    }

    void getStatement(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback, const std::int32_t &id) const
    {
        auto accountStatementOption = this->_checkingAccountService.getAccountStatement(id);
        if (accountStatementOption.has_value())
        {
            auto accountStatementJson = this->accountStatementToJson(accountStatementOption.value());
            auto response = drogon::HttpResponse::newHttpJsonResponse(accountStatementJson);
            response->setStatusCode(drogon::HttpStatusCode::k200OK);
            callback(response);
        }
        else
        {
            auto response = drogon::HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::HttpStatusCode::k404NotFound);
            callback(response);
        }
    }

private:
    CheckingAccountService _checkingAccountService;

    Json::Value validationResultToJson(const ValidationResult &validationResult) const
    {
        Json::Value json;
        json["error"]["message"] = validationResult.getErrorMessage();
        return json;
    }

    Json::Value accountStatementToJson(const AccountStatement &accountStatement) const
    {
        const auto now = std::chrono::utc_clock::now();
        Json::Value accountStatementJson;
        accountStatementJson["saldo"]["total"] = accountStatement.balance.amount;
        accountStatementJson["saldo"]["data_extrato"] = std::format("{0:%F}T{0:%T}Z", now);
        accountStatementJson["saldo"]["limite"] = accountStatement.balance.limit;
        for (auto &transaction : accountStatement.latestTransactions)
        {
            std::chrono::microseconds microseconds(transaction.createdAt);
            std::chrono::time_point<std::chrono::utc_clock> dt(microseconds);
            Json::Value transactionJson;
            transactionJson["valor"] = transaction.amount;
            transactionJson["tipo"] = transaction.type;
            transactionJson["descricao"] = transaction.description;
            transactionJson["realizada_em"] = std::format("{0:%F}T{0:%T}Z", dt);
            accountStatementJson["ultimas_transacoes"].append(transactionJson);
        }

        return accountStatementJson;
    }

    Transaction jsonToTransaction(const std::shared_ptr<Json::Value> &jsonPtr) const
    {
        const auto jsonBody = (*jsonPtr);
        const auto now = std::chrono::utc_clock::now();
        const auto nowInMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
        auto transaction = Transaction(
            static_cast<std::int32_t>(jsonBody["valor"].asInt()),
            static_cast<std::string>(jsonBody["descricao"].asString()),
            static_cast<std::string>(jsonBody["tipo"].asString()),
            nowInMicroSeconds.count()
        );

        return transaction;
    }

    Json::Value balanceToJson(const Balance &balance) const
    {
        Json::Value balanceJson;
        balanceJson["limite"] = balance.limit;
        balanceJson["saldo"] = balance.amount;

        return balanceJson;
    }
};
