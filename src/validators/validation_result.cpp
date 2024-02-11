#pragma once

#include <string>
#include <optional>

class ValidationResult
{
	public:
		bool hasSuccess() const
		{
			return !this->_errorMessage.has_value();
		}

		bool hasError() const
		{
			return this->_errorMessage.has_value();
		}

		std::string getErrorMessage() const
		{
			return this->_errorMessage.value_or("");
		}

		static ValidationResult success() 
		{ 
			static ValidationResult success {};

			return success; 
		}

		static ValidationResult error(const std::string& error_message) 
		{ 
			return ValidationResult(error_message);
		}

	private:
		std::optional<std::string> _errorMessage;

		ValidationResult(): _errorMessage(std::nullopt)
		{
		}

		ValidationResult(const std::string& errorMessage): _errorMessage(errorMessage)
		{
		}

        ValidationResult(const ValidationResult& validationResult): _errorMessage(validationResult._errorMessage)
		{
		}
};
