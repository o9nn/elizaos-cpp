#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/partners.h"

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
    if (req->method != std::string("GET")) {
        return res->status(405)->json(object{
            object::pair{std::string("error"), std::string("Method not allowed")}
        });
    }
    try
    {
        auto allPartners = std::async([=]() { getAllPartners(); });
        auto formattedPartners = allPartners->map([=](auto partner) mutable
        {
            return (object{
                object::pair{std::string("address"), partner->owner}, 
                object::pair{std::string("holdings"), partner->amount}, 
                object::pair{std::string("trustScore"), partner->trustScore}
            });
        }
        );
        console->log(std::string("Returning ") + formattedPartners->get_length() + std::string(" mock partners for trust scoreboard"));
        res->status(200)->json(object{
            object::pair{std::string("partners"), formattedPartners}
        });
    }
    catch (const any& error)
    {
        console->error(std::string("API Error:"), error);
        res->status(500)->json(object{
            object::pair{std::string("error"), std::string("Failed to fetch partner accounts")}, 
            object::pair{std::string("details"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
        });
    }
};


array<std::shared_ptr<Partner>> mockPartners = array<std::shared_ptr<Partner>>{ object{
    object::pair{std::string("owner"), std::string("7xKXtg2CW87d97TXJSDpbD5jBkheTqA83TZRuJosgAsU")}, 
    object::pair{std::string("displayAddress"), std::string("7xKX...gAsU")}, 
    object::pair{std::string("amount"), 2500000}, 
    object::pair{std::string("trustScore"), 95.5}
}, object{
    object::pair{std::string("owner"), std::string("9WzDXwBbmkg8ZTbNMqUxvQRAyrZzDsGYdLVL9zYtAWWM")}, 
    object::pair{std::string("displayAddress"), std::string("9WzD...AWWM")}, 
    object::pair{std::string("amount"), 1800000}, 
    object::pair{std::string("trustScore"), 87.2}
}, object{
    object::pair{std::string("owner"), std::string("CuieVDEDtLo7FypA9SbLM9saXFdb1dsshEkyErMqkRQq")}, 
    object::pair{std::string("displayAddress"), std::string("Cuie...kRQq")}, 
    object::pair{std::string("amount"), 1500000}, 
    object::pair{std::string("trustScore"), 82.7}
}, object{
    object::pair{std::string("owner"), std::string("8sLbNZoA1cfnvMJLPfp98ZLAnFSYCFApfJKMbiXNLwxj")}, 
    object::pair{std::string("displayAddress"), std::string("8sLb...Lwxj")}, 
    object::pair{std::string("amount"), 1200000}, 
    object::pair{std::string("trustScore"), 79.3}
}, object{
    object::pair{std::string("owner"), std::string("B1aLAAe4vW8nSQCetXnYqJfRxzTjnbooczwkUJAr7yMS")}, 
    object::pair{std::string("displayAddress"), std::string("B1aL...7yMS")}, 
    object::pair{std::string("amount"), 950000}, 
    object::pair{std::string("trustScore"), 75.8}
}, object{
    object::pair{std::string("owner"), std::string("DjVE6JNiYqPL2QXyCUEh73933aNBRXS4vqB2AF2H2H6x")}, 
    object::pair{std::string("displayAddress"), std::string("DjVE...H2H6x")}, 
    object::pair{std::string("amount"), 800000}, 
    object::pair{std::string("trustScore"), 71.4}
}, object{
    object::pair{std::string("owner"), std::string("F1Y2pYf4N2LvZgBGqVjYa3d4JcR4YqNtZx1mQzLk8RjZ")}, 
    object::pair{std::string("displayAddress"), std::string("F1Y2...8RjZ")}, 
    object::pair{std::string("amount"), 650000}, 
    object::pair{std::string("trustScore"), 68.9}
}, object{
    object::pair{std::string("owner"), std::string("H7ySQ9b3Rx4BfGvE6Qw2K8LpJ9sA5rR4WnZx7mK2VgLt")}, 
    object::pair{std::string("displayAddress"), std::string("H7yS...VgLt")}, 
    object::pair{std::string("amount"), 550000}, 
    object::pair{std::string("trustScore"), 65.1}
} };
std::function<double(double, double)> calculateTrustScore = [=](auto amount, auto minAmount = 100000) mutable
{
    auto rawScore = (amount == 0) ? any(0) : any(Math->min(100, (amount / minAmount) * 10));
    return Number(rawScore->toFixed(1));
};

void Main(void)
{
}

MAIN
