#include "spartan/src/plugins/plugin-coinmarketcap/src/actions/getPrice/validation.h"

std::any isGetPriceContent(std::shared_ptr<GetPriceContent> content)
{
    return (AND((type_of(content->symbol) == std::string("string")), (type_of(content->currency) == std::string("string"))));
};


std::any GetPriceSchema = z->object(object{
    object::pair{std::string("symbol"), z->std::string()}, 
    object::pair{std::string("currency"), z->std::string()->default(std::string("USD"))}
});

void Main(void)
{
}

MAIN
