#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-coinmarketcap/src/actions/getPrice/validation.h"

any isGetPriceContent(std::shared_ptr<GetPriceContent> content)
{
    return (AND((type_of(content->symbol) == std::string("string")), (type_of(content->currency) == std::string("string"))));
};


any GetPriceSchema = z->object(object{
    object::pair{std::string("symbol"), z->string()}, 
    object::pair{std::string("currency"), z->string()->default(std::string("USD"))}
});

void Main(void)
{
}

MAIN
