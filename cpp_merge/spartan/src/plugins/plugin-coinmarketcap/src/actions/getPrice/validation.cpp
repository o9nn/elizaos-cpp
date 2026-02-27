#include "validation.hpp"

any isGetPriceContent(std::shared_ptr<GetPriceContent> content)
{
    return (AND((type_of(content->symbol) == std:("string")), (type_of(content->currency) == std:("string"))));
};


any GetPriceSchema = z->object(object{
    object::pair{std:("symbol"), z->string()}, 
    object::pair{std:("currency"), z->string()->default(std:("USD"))}
});

void Main(void)
{
}

MAIN
