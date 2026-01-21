#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-todo/services/dbCompatibility.h"

DatabaseCompatibilityService::DatabaseCompatibilityService() {
    this->detectDatabaseType();
}

void DatabaseCompatibilityService::detectDatabaseType()
{
    auto dbUrl = OR((OR((process->env->DATABASE_URL), (process->env->DB_URL))), (string_empty));
    if (OR((dbUrl->includes(std::string("postgres"))), (dbUrl->includes(std::string("pg://"))))) {
        this->databaseType = std::string("postgres");
    } else if (OR((dbUrl->includes(std::string("sqlite"))), (dbUrl->includes(std::string(".db"))))) {
        this->databaseType = std::string("sqlite");
    } else {
        this->databaseType = std::string("sqlite");
        logger->warn(std::string("Database type not detected, defaulting to sqlite"));
    }
    logger->info(std::string("Database compatibility mode: ") + this->databaseType + string_empty);
}

boolean DatabaseCompatibilityService::parseBoolean(std::any value)
{
    if (this->databaseType == std::string("sqlite")) {
        return OR((OR((value == 1), (value == std::string("1")))), (value == true));
    }
    return value == true;
}

std::any DatabaseCompatibilityService::parseDate(std::any value)
{
    if (!value) return nullptr;
    try
    {
        if (AND((this->databaseType == std::string("sqlite")), (type_of(value) == std::string("number")))) {
            return std::make_shared<Date>(value * 1000);
        }
        return std::make_shared<Date>(value);
    }
    catch (const std::any& error)
    {
        logger->error(std::string("Failed to parse date:"), error);
        return nullptr;
    }
}

std::any DatabaseCompatibilityService::parseJson(std::any value)
{
    if (!value) return nullptr;
    if (AND((this->databaseType == std::string("sqlite")), (type_of(value) == std::string("string")))) {
        try
        {
            return JSON->parse(value);
        }
        catch (const std::any& error)
        {
            logger->error(std::string("Failed to parse JSON:"), error);
            return nullptr;
        }
    }
    return value;
}

array<string> DatabaseCompatibilityService::parseArray(std::any value)
{
    if (!value) return array<any>();
    if (AND((this->databaseType == std::string("sqlite")), (type_of(value) == std::string("string")))) {
        try
        {
            return JSON->parse(value);
        }
        catch (const std::any& error)
        {
            logger->error(std::string("Failed to parse array:"), error);
            return array<any>();
        }
    }
    return (Array->isArray(value)) ? value : array<any>();
}

std::string DatabaseCompatibilityService::buildCaseInsensitiveSearch(std::string column, std::string value)
{
    if (this->databaseType == std::string("postgres")) {
        return string_empty + column + std::string(" ILIKE '%") + value + std::string("%'");
    }
    return string_empty + column + std::string(" LIKE '%") + value + std::string("%'");
}

std::shared_ptr<DatabaseCompatibilityService> dbCompat = std::make_shared<DatabaseCompatibilityService>();

void Main(void)
{
}

MAIN
