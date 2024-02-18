cmake_minimum_required(VERSION 3.15)

project(rinha-backend VERSION 1.0 
                      DESCRIPTION "A simple project for rinha de backends competition." 
                      LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra")

include_directories(include ${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} 
               src/common/result.cpp
               src/models/account_statement.cpp
               src/models/balance.cpp
               src/models/transaction.cpp               
               src/validators/validation_result.cpp
               src/validators/transaction_body_validator.cpp
               src/repositories/balance_repository.cpp
               src/repositories/transaction_repository.cpp
               src/services/checking_account_service.cpp
               src/controllers/customers_controller.cpp
               src/main.cpp)

find_package(PostgreSQL REQUIRED)
find_package(Drogon REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon INTERFACE PostgreSQL::PostgreSQL)
