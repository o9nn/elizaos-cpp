#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/helius/providers/helius.h"

HeliusWebSocket::HeliusWebSocket(string apiKey_, std::shared_ptr<IAgentRuntime> runtime_) : apiKey(apiKey_), runtime(runtime_)  {
}

std::shared_ptr<Promise<void>> HeliusWebSocket::connect()
{
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        try
        {
            this->ws = std::make_shared<WebSocket>(std::string("wss://mainnet.helius-rpc.com/?api-key=") + this->apiKey + string_empty);
            this->ws["on"](std::string("open"), [=]() mutable
            {
                console->log(std::string("Helius WebSocket connected"));
                this->startPing();
                resolve();
            }
            );
            this->ws["on"](std::string("message"), [=](auto data) mutable
            {
                try
                {
                    auto message = JSON->parse(data["toString"]());
                    console->log(std::string("Received message:"), message);
                }
                catch (const any& e)
                {
                    console->error(std::string("Failed to parse WebSocket message:"), e);
                }
            }
            );
            this->ws["on"](std::string("error"), [=](auto error) mutable
            {
                console->error(std::string("WebSocket error:"), error);
            }
            );
            this->ws["on"](std::string("close"), [=]() mutable
            {
                console->log(std::string("WebSocket closed"));
                this->cleanup();
            }
            );
        }
        catch (const any& error)
        {
            reject(error);
        }
    }
    );
}

std::shared_ptr<Promise<any>> HeliusWebSocket::subscribeToWallet(string walletAddress)
{
    if (OR((!this->ws), (this->ws["readyState"] != WebSocket->OPEN))) {
        throw any(std::make_shared<Error>(std::string("WebSocket not connected")));
    }
    shared request = object{
        object::pair{std::string("jsonrpc"), std::string("2.0")}, 
        object::pair{std::string("id"), Date->now()}, 
        object::pair{std::string("method"), std::string("accountSubscribe")}, 
        object::pair{std::string("params"), array<string>{ walletAddress, object{
            object::pair{std::string("encoding"), std::string("jsonParsed")}, 
            object::pair{std::string("commitment"), std::string("confirmed")}
        } }}
    };
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        if (!this->ws) {
            reject(std::make_shared<Error>(std::string("WebSocket not connected")));
            return;
        }
        this->ws["send"](JSON->stringify(request), [=](auto error) mutable
        {
            if (error) {
                reject(error);
                return;
            }
            this->ws["once"](std::string("message"), [=](auto data) mutable
            {
                try
                {
                    auto response = JSON->parse(data["toString"]());
                    if (response["error"]) {
                        reject(std::make_shared<Error>(response["error"]["message"]));
                        return;
                    }
                    auto subscriptionId = response["result"];
                    this->subscriptions->set(walletAddress, subscriptionId);
                    resolve(subscriptionId);
                }
                catch (const any& e)
                {
                    reject(e);
                }
            }
            );
        }
        );
    }
    );
}

std::shared_ptr<Promise<boolean>> HeliusWebSocket::unsubscribeFromWallet(string walletAddress)
{
    auto subscriptionId = this->subscriptions->get(walletAddress);
    if (!subscriptionId) {
        return false;
    }
    shared request = object{
        object::pair{std::string("jsonrpc"), std::string("2.0")}, 
        object::pair{std::string("id"), Date->now()}, 
        object::pair{std::string("method"), std::string("unsubscribe")}, 
        object::pair{std::string("params"), array<double>{ subscriptionId }}
    };
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        if (!this->ws) {
            reject(std::make_shared<Error>(std::string("WebSocket not connected")));
            return;
        }
        this->ws["send"](JSON->stringify(request), [=](auto error) mutable
        {
            if (error) {
                reject(error);
                return;
            }
            this->subscriptions->delete(walletAddress);
            resolve(true);
        }
        );
    }
    );
}

void HeliusWebSocket::startPing()
{
    this->pingInterval = setInterval([=]() mutable
    {
        if (this->ws["readyState"] == WebSocket->OPEN) {
            this->ws["ping"]();
        }
    }
    , 30000);
}

void HeliusWebSocket::cleanup()
{
    if (this->pingInterval) {
        clearInterval(this->pingInterval);
        this->pingInterval = nullptr;
    }
    this->subscriptions->clear();
}

void HeliusWebSocket::disconnect()
{
    if (this->ws) {
        this->ws["close"]();
        this->cleanup();
    }
}

std::shared_ptr<Provider> heliusProvider = object{
    object::pair{std::string("name"), std::string("HELIUS_INFORMATION")}, 
    object::pair{std::string("description"), std::string("Helius latest information about the cryptocurrencies using Laserstream")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
    {
        try
        {
            auto url = std::string("https://api.helius.xyz/v0/token-metadata?api-key=") + runtime->getSetting(std::string("HELIUS_API_KEY")) + string_empty;
            auto response = std::async([=]() { fetch(url); });
            auto tokens = std::async([=]() { response->json(); });
            auto data = object{
                object::pair{std::string("tokens"), tokens["map"]([=](auto token) mutable
                {
                    return (object{
                        object::pair{std::string("symbol"), OR((token->symbol), (std::string("Unknown")))}, 
                        object::pair{std::string("name"), OR((token->name), (std::string("Unknown")))}, 
                        object::pair{std::string("address"), token->address}, 
                        object::pair{std::string("decimals"), token->decimals}, 
                        object::pair{std::string("totalSupply"), token->totalSupply}, 
                        object::pair{std::string("marketCap"), token->marketCap}, 
                        object::pair{std::string("volume24h"), token->volume24h}, 
                        object::pair{std::string("price"), token->price}
                    });
                }
                )}
            };
            auto text = std::string("\
Current Helius token information:\
\
");
            for (auto& token : data["tokens"])
            {
                text += string_empty + token["name"] + std::string(" (") + token["symbol"] + std::string(")\
");
                text += std::string("Address: ") + token["address"] + std::string("\
");
                text += std::string("Price: $") + token["price"] + std::string("\
");
                text += std::string("24h Volume: $") + token["volume24h"] + std::string("\
");
                text += std::string("Market Cap: $") + token["marketCap"] + std::string("\
\
");
            }
            return object{
                object::pair{std::string("data"), std::string("data")}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), std::string("text")}
            };
        }
        catch (const any& error)
        {
            console->error(std::string("Error in Helius provider:"), error);
            throw any(error);
        }
    }
    }
};

void Main(void)
{
}

MAIN
