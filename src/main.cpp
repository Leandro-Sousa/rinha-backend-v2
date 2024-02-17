#include <iostream>
#include <memory>
#include <drogon/drogon.h>

#include <src/common/file_utils.cpp>
#include <src/repositories/balance_repository.cpp>
#include <src/repositories/transaction_repository.cpp>
#include <src/services/checking_account_service.cpp>
#include <src/controllers/customers_controller.cpp>

int main(int argc, char *argv[])
{
	const auto connectionString = std::string(std::getenv("DATABASE_CONNECTION_STRING"));
	const auto dbClientPtr = drogon::orm::DbClient::newPgClient(connectionString, 16);

	BalanceRepository balanceRepository(dbClientPtr);
	TransactionRepository transactionRepository(dbClientPtr);
	CheckingAccountService checkingAccountService(balanceRepository, transactionRepository);

	auto port = std::stoi(std::getenv("APP_PORT"));
	
	drogon::app()
		.setLogLevel(trantor::Logger::kInfo)
		.addListener("0.0.0.0", port)
		.setThreadNum(6)
		.registerController(std::make_shared<CustomersController>(checkingAccountService))
		.run();
}
