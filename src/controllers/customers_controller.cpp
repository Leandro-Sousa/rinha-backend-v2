#include <iostream>
#include <drogon/HttpController.h>
#include <Poco/DateTime.h>
#include <Poco/Timestamp.h>

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
        drogon::HttpResponsePtr response = nullptr;
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
                response = drogon::HttpResponse::newHttpJsonResponse(balanceJson);
                response->setStatusCode(drogon::HttpStatusCode::k200OK);
            }
            else
            {
                response = drogon::HttpResponse::newHttpResponse();
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
            }
        }
        else
        {
            response = drogon::HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::HttpStatusCode::k422UnprocessableEntity);
            response->setBody(validationResult.getErrorMessage());
        }

        callback(response);
    }

    void getStatement(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback, const std::int32_t &id) const
    {
        drogon::HttpResponsePtr response;
        auto accountStatementOption = this->_checkingAccountService.getAccountStatement(id);
        if (accountStatementOption.has_value())
        {
            auto accountStatementJson = this->accountStatementToJson(accountStatementOption.value());
            response = drogon::HttpResponse::newHttpJsonResponse(accountStatementJson);
            response->setStatusCode(drogon::HttpStatusCode::k200OK);
        }
        else
        {
            response = drogon::HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::HttpStatusCode::k404NotFound);
        }
        callback(response);
    }

private:
    CheckingAccountService _checkingAccountService;

    Json::Value validationResultToJson(const ValidationResult &validationResult) const
    {
        Json::Value json;
        json["error"]["message"] = validationResult.getErrorMessage();
        return json;
    }

    Json::Value accountStatementToJson(const AccountStatementPtr &accountStatement) const
    {
        static const std::string dateTimeFormat = "%Y-%m-%dT%H:%M:%S.%iZ";
        auto now = Poco::Timestamp();
        Json::Value accountStatementJson;
        accountStatementJson["saldo"]["total"] = accountStatement->balance.amount;
        accountStatementJson["saldo"]["data_extrato"] = Poco::DateTimeFormatter::format(now, dateTimeFormat);
        accountStatementJson["saldo"]["limite"] = accountStatement->balance.limit;
        for (auto &transaction : (*accountStatement->latestTransactions))
        {
            Json::Value transactionJson;
            transactionJson["valor"] = transaction.amount;
            transactionJson["tipo"] = transaction.type;
            transactionJson["descricao"] = transaction.description;
            transactionJson["realizada_em"] = Poco::DateTimeFormatter::format(transaction.createdAt, dateTimeFormat);
            accountStatementJson["ultimas_transacoes"].append(transactionJson);
        }

        return accountStatementJson;
    }

    Transaction jsonToTransaction(const std::shared_ptr<Json::Value>& jsonPtr) const
    {
        auto jsonBody = (*jsonPtr);
        auto transaction = Transaction {
            .amount = jsonBody["valor"].asInt(),
            .description = jsonBody["descricao"].asString(),
            .type = jsonBody["tipo"].asString(),
            .createdAt = Poco::Timestamp()
        };

        return transaction;
    }

    Json::Value balanceToJson(const Balance& balance) const
    {
        Json::Value balanceJson;
        balanceJson["limite"] = balance.limit;
        balanceJson["saldo"] = balance.amount;

        return balanceJson;
    }
};
