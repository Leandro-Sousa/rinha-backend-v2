#include <iostream>
#include <memory>
#include <drogon/drogon.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/Data/SQLite/Connector.h>

#include <src/common/file_utils.cpp>
#include <src/repositories/balance_repository.cpp>
#include <src/repositories/transaction_repository.cpp>
#include <src/services/checking_account_service.cpp>
#include <src/controllers/customers_controller.cpp>

void initDatabase(const std::shared_ptr<Poco::Data::SessionPool> &sessionPool)
{
	try
	{
		auto databaseInitScriptPath = std::getenv("DATABASE_INIT_SCRIPT_PATH");
		auto content = FileUtils::readAllText(databaseInitScriptPath);
		auto session = sessionPool->get();
		session << content, Poco::Data::Keywords::now;
	}
	catch(const std::exception& e)
	{
		std::cout << "Error on try create database: " << e.what() << std::endl;
	}
}

int main(int argc, char *argv[])
{
	Poco::Data::SQLite::Connector::registerConnector();

	auto sessionPool = std::make_shared<Poco::Data::SessionPool>(
		Poco::Data::SQLite::Connector::KEY, 
		std::getenv("DATABASE_CONNECTION_STRING"), 
		16, 
		64
	);

	initDatabase(sessionPool);

	BalanceRepository balanceRepository(sessionPool);
	TransactionRepository transactionRepository(sessionPool);
	CheckingAccountService checkingAccountService(balanceRepository, transactionRepository);

	auto port = std::stoi(std::getenv("APP_PORT"));
	drogon::app()
		.setLogLevel(trantor::Logger::kInfo)
		.addListener("0.0.0.0", port)
		.setThreadNum(6)
		.registerController(std::make_shared<CustomersController>(checkingAccountService))
		.run();
}
