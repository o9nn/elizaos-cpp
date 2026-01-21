#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-env/generation.h"

boolean canGenerateEnvVar(string varName, string type, string description)
{
    auto lowerName = varName->toLowerCase();
    auto lowerDesc = OR((description->toLowerCase()), (string_empty));
    if (OR((OR((type == std::string("private_key")), (lowerName->includes(std::string("private_key"))))), (lowerDesc->includes(std::string("private key"))))) {
        return true;
    }
    if (OR((OR((type == std::string("secret")), (lowerName->includes(std::string("secret"))))), (lowerName->includes(std::string("key"))))) {
        if (OR((lowerName->includes(std::string("api_key"))), (lowerDesc->includes(std::string("api key"))))) {
            return false;
        }
        return true;
    }
    if (type == std::string("config")) {
        if (OR((lowerName->includes(std::string("port"))), (lowerName->includes(std::string("database_name"))))) {
            return true;
        }
    }
    if (OR((lowerName->includes(std::string("uuid"))), (lowerName->includes(std::string("id"))))) {
        return true;
    }
    return false;
};


any generateScript(string varName, string type, string pluginName, string description)
{
    auto lowerName = varName->toLowerCase();
    auto lowerDesc = OR((description->toLowerCase()), (string_empty));
    auto script = nullptr;
    auto dependencies = array<string>();
    if (OR((type == std::string("private_key")), (lowerName->includes(std::string("private_key"))))) {
        if (OR((lowerDesc->includes(std::string("ed25519"))), (lowerName->includes(std::string("ed25519"))))) {
            script = generationTemplates["private_key"]["ed25519"];
        } else {
            script = generationTemplates["private_key"]["rsa"];
        }
        dependencies = generationDependencies["private_key"];
    } else if (OR((lowerName->includes(std::string("uuid"))), (lowerName->includes(std::string("id"))))) {
        script = generationTemplates["secret"]["uuid"];
        dependencies = generationDependencies["secret"]["uuid"];
    } else if (AND((lowerName->includes(std::string("jwt"))), (lowerName->includes(std::string("secret"))))) {
        script = generationTemplates["secret"]["jwt_secret"];
        dependencies = generationDependencies["secret"]["jwt_secret"];
    } else if (OR((type == std::string("secret")), (lowerName->includes(std::string("secret"))))) {
        if (OR((lowerDesc->includes(std::string("base64"))), (lowerName->includes(std::string("base64"))))) {
            script = generationTemplates["secret"]["base64_32"];
        } else {
            script = generationTemplates["secret"]["hex_32"];
        }
        dependencies = generationDependencies["secret"]["hex_32"];
    } else if (lowerName->includes(std::string("port"))) {
        script = generationTemplates["config"]["port"];
        dependencies = generationDependencies["config"]["port"];
    } else if (AND((lowerName->includes(std::string("database"))), (lowerName->includes(std::string("name"))))) {
        script = generationTemplates["config"]["database_name"];
        dependencies = generationDependencies["config"]["database_name"];
    }
    if (!script) {
        logger->warn(std::string("No generation script available for ") + varName + std::string(" of type ") + type + string_empty);
        return nullptr;
    }
    return object{
        object::pair{std::string("variableName"), varName}, 
        object::pair{std::string("pluginName"), std::string("pluginName")}, 
        object::pair{std::string("script"), script->trim()}, 
        object::pair{std::string("dependencies"), std::string("dependencies")}, 
        object::pair{std::string("attempts"), 0}, 
        object::pair{std::string("status"), std::string("pending")}, 
        object::pair{std::string("createdAt"), Date->now()}
    };
};


string getGenerationDescription(string varName, string type)
{
    auto lowerName = varName->toLowerCase();
    if (OR((type == std::string("private_key")), (lowerName->includes(std::string("private_key"))))) {
        if (lowerName->includes(std::string("ed25519"))) {
            return std::string("Generate a new Ed25519 private key for cryptographic operations");
        } else {
            return std::string("Generate a new RSA private key for cryptographic operations");
        }
    } else if (lowerName->includes(std::string("uuid"))) {
        return std::string("Generate a new UUID (Universally Unique Identifier)");
    } else if (AND((lowerName->includes(std::string("jwt"))), (lowerName->includes(std::string("secret"))))) {
        return std::string("Generate a secure secret for JWT token signing");
    } else if (OR((type == std::string("secret")), (lowerName->includes(std::string("secret"))))) {
        return std::string("Generate a cryptographically secure random secret");
    } else if (lowerName->includes(std::string("port"))) {
        return std::string("Generate a random port number for the application");
    } else if (AND((lowerName->includes(std::string("database"))), (lowerName->includes(std::string("name"))))) {
        return std::string("Generate a unique database name");
    }
    return std::string("Generate a value for this environment variable");
};


object generationTemplates = object{
    object::pair{std::string("private_key"), object{
        object::pair{std::string("rsa"), std::string("\
const crypto = require('crypto');\
const { generateKeyPairSync } = crypto;\
\
try {\
  const { privateKey } = generateKeyPairSync('rsa', {\
    modulusLength: 2048,\
    privateKeyEncoding: { \
      type: 'pkcs8', \
      format: 'pem' \
    }\
  });\
  \
  console.log(privateKey);\
} catch (error) {\
  console.error('Error generating RSA key:', error.message);\
  process.exit(1);\
}\
    ")}, 
        object::pair{std::string("ed25519"), std::string("\
const crypto = require('crypto');\
const { generateKeyPairSync } = crypto;\
\
try {\
  const { privateKey } = generateKeyPairSync('ed25519', {\
    privateKeyEncoding: { \
      type: 'pkcs8', \
      format: 'pem' \
    }\
  });\
  \
  console.log(privateKey);\
} catch (error) {\
  console.error('Error generating Ed25519 key:', error.message);\
  process.exit(1);\
}\
    ")}
    }}, 
    object::pair{std::string("secret"), object{
        object::pair{std::string("uuid"), std::string("\
const { v4: uuidv4 } = require('uuid');\
\
try {\
  console.log(uuidv4());\
} catch (error) {\
  console.error('Error generating UUID:', error.message);\
  process.exit(1);\
}\
    ")}, 
        object::pair{std::string("hex_32"), std::string("\
const crypto = require('crypto');\
\
try {\
  console.log(crypto.randomBytes(32).toString('hex'));\
} catch (error) {\
  console.error('Error generating hex secret:', error.message);\
  process.exit(1);\
}\
    ")}, 
        object::pair{std::string("base64_32"), std::string("\
const crypto = require('crypto');\
\
try {\
  console.log(crypto.randomBytes(32).toString('base64'));\
} catch (error) {\
  console.error('Error generating base64 secret:', error.message);\
  process.exit(1);\
}\
    ")}, 
        object::pair{std::string("jwt_secret"), std::string("\
const crypto = require('crypto');\
\
try {\
  // Generate a 256-bit (32-byte) secret for JWT signing\
  const secret = crypto.randomBytes(32).toString('base64url');\
  console.log(secret);\
} catch (error) {\
  console.error('Error generating JWT secret:', error.message);\
  process.exit(1);\
}\
    ")}
    }}, 
    object::pair{std::string("config"), object{
        object::pair{std::string("port"), std::string("\
// Generate a random port number between 3000-9999\
const port = Math.floor(Math.random() * (9999 - 3000 + 1)) + 3000;\
console.log(port.toString());\
    ")}, 
        object::pair{std::string("database_name"), std::string("\
const crypto = require('crypto');\
\
try {\
  // Generate a database name with timestamp and random suffix\
  const timestamp = Date.now();\
  const randomSuffix = crypto.randomBytes(4).toString('hex');\
  const dbName = `app_db_${timestamp}_${randomSuffix}`;\
  console.log(dbName);\
} catch (error) {\
  console.error('Error generating database name:', error.message);\
  process.exit(1);\
}\
    ")}
    }}
};
object generationDependencies = object{
    object::pair{std::string("private_key"), array<any>()}, 
    object::pair{std::string("secret"), object{
        object::pair{std::string("uuid"), array<string>{ std::string("uuid") }}, 
        object::pair{std::string("hex_32"), array<any>()}, 
        object::pair{std::string("base64_32"), array<any>()}, 
        object::pair{std::string("jwt_secret"), array<any>()}
    }}, 
    object::pair{std::string("config"), object{
        object::pair{std::string("port"), array<any>()}, 
        object::pair{std::string("database_name"), array<any>()}
    }}
};

void Main(void)
{
}

MAIN
