#include "dbCompatibility.hpp"

DatabaseCompatibilityService::DatabaseCompatibilityService() {
    this->detectDatabaseType();
}

void DatabaseCompatibilityService::detectDatabaseType()
{
    auto dbUrl = OR((OR((process->env->DATABASE_URL), (process->env->DB_URL))), (string_empty));
    if (OR((dbUrl->includes(std:("postgres"))), (dbUrl->includes(std:("pg://"))))) {
        this->databaseType = std:("postgres");
    } else if (OR((dbUrl->includes(std:("sqlite"))), (dbUrl->includes(std:(".db"))))) {
        this->databaseType = std:("sqlite");
    } else {
        this->databaseType = std:("sqlite");
        logger->warn(std:("Database type not detected, defaulting to sqlite"));
    }
    logger->info(std:("Database compatibility mode: ") + this->databaseType + string_empty);
}

boolean DatabaseCompatibilityService::parseBoolean(any value)
{
    if (this->databaseType == std:("sqlite")) {
        return OR((OR((value == 1), (value == std:("1")))), (value == true));
    }
    return value == true;
}

any DatabaseCompatibilityService::parseDate(any value)
{
    if (!value) return nullptr;
    try
    {
        if (AND((this->databaseType == std:("sqlite")), (type_of(value) == std:("number")))) {
            return std::make_shared<Date>(value * 1000);
        }
        return std::make_shared<Date>(value);
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to parse date:"), error);
        return nullptr;
    }
}

any DatabaseCompatibilityService::parseJson(any value)
{
    if (!value) return nullptr;
    if (AND((this->databaseType == std:("sqlite")), (type_of(value) == std:("string")))) {
        try
        {
            return JSON->parse(value);
        }
        catch (const any& error)
        {
            logger->error(std:("Failed to parse JSON:"), error);
            return nullptr;
        }
    }
    return value;
}

array<string> DatabaseCompatibilityService::parseArray(any value)
{
    if (!value) return array<any>();
    if (AND((this->databaseType == std:("sqlite")), (type_of(value) == std:("string")))) {
        try
        {
            return JSON->parse(value);
        }
        catch (const any& error)
        {
            logger->error(std:("Failed to parse array:"), error);
            return array<any>();
        }
    }
    return (Array->isArray(value)) ? value : array<any>();
}

string DatabaseCompatibilityService::buildCaseInsensitiveSearch(string column, string value)
{
    if (this->databaseType == std:("postgres")) {
        return string_empty + column + std:(" ILIKE '%") + value + std:("%'");
    }
    return string_empty + column + std:(" LIKE '%") + value + std:("%'");
}

std::shared_ptr<DatabaseCompatibilityService> dbCompat = std::make_shared<DatabaseCompatibilityService>();

void Main(void)
{
}

MAIN
