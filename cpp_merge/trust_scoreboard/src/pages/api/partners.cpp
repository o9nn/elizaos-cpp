#include "partners.hpp"

std::shared_ptr<Promise<array<std::shared_ptr<Partner>>>> getAllPartners()
{
    return mockPartners->sort([=](auto a, auto b) mutable
    {
        return b->amount - a->amount;
    }
    );
};


any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    if (req->method != std:("GET")) {
        return res->status(405)->json(object{
            object::pair{std:("error"), std:("Method not allowed")}
        });
    }
    try
    {
        auto allPartners = std::async([=]() { getAllPartners(); });
        auto formattedPartners = allPartners->map([=](auto partner) mutable
        {
            return (object{
                object::pair{std:("address"), partner->owner}, 
                object::pair{std:("holdings"), partner->amount}, 
                object::pair{std:("trustScore"), partner->trustScore}
            });
        }
        );
        console->log(std:("Returning ") + formattedPartners->get_length() + std:(" mock partners for trust scoreboard"));
        res->status(200)->json(object{
            object::pair{std:("partners"), formattedPartners}
        });
    }
    catch (const any& error)
    {
        console->error(std:("API Error:"), error);
        res->status(500)->json(object{
            object::pair{std:("error"), std:("Failed to fetch partner accounts")}, 
            object::pair{std:("details"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
        });
    }
};


array<std::shared_ptr<Partner>> mockPartners = array<std::shared_ptr<Partner>>{ object{
    object::pair{std:("owner"), std:("7xKXtg2CW87d97TXJSDpbD5jBkheTqA83TZRuJosgAsU")}, 
    object::pair{std:("displayAddress"), std:("7xKX/* spread: gAsU */")}, 
    object::pair{std:("amount"), 2500000}, 
    object::pair{std:("trustScore"), 95.5}
}, object{
    object::pair{std:("owner"), std:("9WzDXwBbmkg8ZTbNMqUxvQRAyrZzDsGYdLVL9zYtAWWM")}, 
    object::pair{std:("displayAddress"), std:("9WzD/* spread: AWWM */")}, 
    object::pair{std:("amount"), 1800000}, 
    object::pair{std:("trustScore"), 87.2}
}, object{
    object::pair{std:("owner"), std:("CuieVDEDtLo7FypA9SbLM9saXFdb1dsshEkyErMqkRQq")}, 
    object::pair{std:("displayAddress"), std:("Cuie/* spread: kRQq */")}, 
    object::pair{std:("amount"), 1500000}, 
    object::pair{std:("trustScore"), 82.7}
}, object{
    object::pair{std:("owner"), std:("8sLbNZoA1cfnvMJLPfp98ZLAnFSYCFApfJKMbiXNLwxj")}, 
    object::pair{std:("displayAddress"), std:("8sLb/* spread: Lwxj */")}, 
    object::pair{std:("amount"), 1200000}, 
    object::pair{std:("trustScore"), 79.3}
}, object{
    object::pair{std:("owner"), std:("B1aLAAe4vW8nSQCetXnYqJfRxzTjnbooczwkUJAr7yMS")}, 
    object::pair{std:("displayAddress"), std:("B1aL/* spread: 7yMS */")}, 
    object::pair{std:("amount"), 950000}, 
    object::pair{std:("trustScore"), 75.8}
}, object{
    object::pair{std:("owner"), std:("DjVE6JNiYqPL2QXyCUEh73933aNBRXS4vqB2AF2H2H6x")}, 
    object::pair{std:("displayAddress"), std:("DjVE/* spread: H2H6x */")}, 
    object::pair{std:("amount"), 800000}, 
    object::pair{std:("trustScore"), 71.4}
}, object{
    object::pair{std:("owner"), std:("F1Y2pYf4N2LvZgBGqVjYa3d4JcR4YqNtZx1mQzLk8RjZ")}, 
    object::pair{std:("displayAddress"), std:("F1Y2/* spread: 8RjZ */")}, 
    object::pair{std:("amount"), 650000}, 
    object::pair{std:("trustScore"), 68.9}
}, object{
    object::pair{std:("owner"), std:("H7ySQ9b3Rx4BfGvE6Qw2K8LpJ9sA5rR4WnZx7mK2VgLt")}, 
    object::pair{std:("displayAddress"), std:("H7yS/* spread: VgLt */")}, 
    object::pair{std:("amount"), 550000}, 
    object::pair{std:("trustScore"), 65.1}
} };
std::function<double(double, double)> calculateTrustScore = [=](auto amount, auto minAmount = 100000) mutable
{
    auto rawScore = (amount == 0) ? any(0) (Math->min(100, (amount / minAmount) * 10));
    return Number(rawScore->toFixed(1));
};

void Main(void)
{
}

MAIN
