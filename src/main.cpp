#include <iostream>
#include <memory>
#include <drogon/drogon.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>

#include <src/common/file_utils.cpp>
#include <src/repositories/balance_repository.cpp>
#include <src/repositories/transaction_repository.cpp>
#include <src/services/checking_account_service.cpp>
#include <src/controllers/customers_controller.cpp>

void createDatabase(const std::shared_ptr<Poco::Data::SessionPool> &sessionPool)
{
	const auto content = FileUtils::readAllAsText("./init.sql");
	auto session = sessionPool->get();
	session << content, Poco::Data::Keywords::now;
}

bool shouldCreateDatabase(int argc, char *argv[])
{
	if(argc > 1 && strcmp(argv[1], "--create-database") == 0)
	{
		return true;
	}

	return false;
}

int main(int argc, char *argv[])
{
	Poco::Data::SQLite::Connector::registerConnector();

	const auto sessionPool = std::make_shared<Poco::Data::SessionPool>(
		Poco::Data::SQLite::Connector::KEY, 
		std::getenv("DATABASE_PATH"),
		8,
		64
	);

	if(shouldCreateDatabase(argc, argv))
	{
		createDatabase(sessionPool);
	}

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
