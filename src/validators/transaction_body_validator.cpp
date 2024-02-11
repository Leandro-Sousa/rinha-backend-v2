#include <set>
#include <memory>
#include <json/json.h>

#include "validation_result.cpp"

class TransactionBodyValidator
{
    public:
        ValidationResult validate(const std::shared_ptr<Json::Value>& jsonPtr)
        {
            if(jsonPtr == nullptr)
            {
                return ValidationResult::error("Body is empty");
            }

            auto json = (*jsonPtr);

            if(json["valor"].empty())
            {
                return ValidationResult::error("valor must be informed");
            }

            if(!json["valor"].isInt())
            {
                return ValidationResult::error("valor must be integer");
            }

            if(json["valor"].asInt() <= 0)
            {
                return ValidationResult::error("valor must be greater than zero");
            }

            if(json["tipo"].empty())
            {
                return ValidationResult::error("tipo must be informed");
            }

            if(!json["tipo"].isString())
            {
                return ValidationResult::error("tipo must be string");
            }

            static auto validTypes = std::set<std::string> {"c", "d"};

            if(!validTypes.contains(json["tipo"].asString()))
            {
                return ValidationResult::error("tipo must be 'c' or 'd'");
            }

            if(json["descricao"].empty())
            {
                return ValidationResult::error("descricao must be informed");
            }

            if(!json["descricao"].isString())
            {
                return ValidationResult::error("descricao must be string");
            }

            auto description = json["descricao"].asString();

            if(description.length() == 0 || description.length() > 10)
            {
                return ValidationResult::error("descricao must have length between 1 and 10");
            }

            return ValidationResult::success();
        }
};
