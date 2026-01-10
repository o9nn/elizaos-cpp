#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-solana/src/actions/transfer.h"

boolean isTransferContent(std::shared_ptr<TransferContent> content)
{
    logger->log(std::string("Content for transfer"), content);
    if (OR((OR((!content->recipient), (type_of(content->recipient) != std::string("string")))), (!content->amount))) {
        return false;
    }
    if (content->tokenAddress == std::string("null")) {
        content->tokenAddress = nullptr;
    }
    return OR((type_of(content->amount) == std::string("string")), (type_of(content->amount) == std::string("number")));
};


string transferTemplate = std::string("Respond with a JSON markdown block containing only the extracted values. Use null for any values that cannot be determined.\
\
Example responses:\
For SPL tokens:\
```json\
{\
    "tokenAddress": "BieefG47jAHCGZBxi2q87RDuHyGZyYC3vAzxpyu8pump",\
    "recipient": "9jW8FPr6BSSsemWPV22UUCzSqkVdTp6HTyPqeqyuBbCa",\
    "amount": "1000"\
}\
```\
\
For SOL:\
```json\
{\
    "tokenAddress": null,\
    "recipient": "9jW8FPr6BSSsemWPV22UUCzSqkVdTp6HTyPqeqyuBbCa",\
    "amount": 1.5\
}\
```\
\
{{recentMessages}}\
\
Extract the following information about the requested transfer:\
- Token contract address (use null for SOL transfers)\
- Recipient wallet address\
- Amount to transfer\
");

void Main(void)
{
}

MAIN
