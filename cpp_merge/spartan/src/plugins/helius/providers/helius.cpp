#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/helius/providers/helius.h"

HeliusWebSocket::HeliusWebSocket(string apiKey_, std::shared_ptr<IAgentRuntime> runtime_) : apiKey(apiKey_), runtime(runtime_)  {
}

std::shared_ptr<Promise<void>> HeliusWebSocket::connect()
{
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        try
        {
            this->ws = std::make_shared<WebSocket>(std:("wss://mainnet.helius-rpc.com/?api-key=") + this->apiKey + string_empty);
            this->ws["on"](std:("open"), [=]() mutable
            {
                console->log(std:("Helius WebSocket connected"));
                this->startPing();
                resolve();
            }
            );
            this->ws["on"](std:("message"), [=](auto data) mutable
            {
                try
                {
                    auto message = JSON->parse(data["toString"]());
                    console->log(std:("Received message:"), message);
                }
                catch (const any& e)
                {
                    console->error(std:("Failed to parse WebSocket message:"), e);
                }
            }
            );
            this->ws["on"](std:("error"), [=](auto error) mutable
            {
                console->error(std:("WebSocket error:"), error);
            }
            );
            this->ws["on"](std:("close"), [=]() mutable
            {
                console->log(std:("WebSocket closed"));
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
        throw any(std::make_shared<Error>(std:("WebSocket not connected")));
    }
    shared request = object{
        object::pair{std:("jsonrpc"), std:("2.0")}, 
        object::pair{std:("id"), Date->now()}, 
        object::pair{std:("method"), std:("accountSubscribe")}, 
        object::pair{std:("params"), array<string>{ walletAddress, object{
            object::pair{std:("encoding"), std:("jsonParsed")}, 
            object::pair{std:("commitment"), std:("confirmed")}
        } }}
    };
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        if (!this->ws) {
            reject(std::make_shared<Error>(std:("WebSocket not connected")));
            return;
        }
        this->ws["send"](JSON->stringify(request), [=](auto error) mutable
        {
            if (error) {
                reject(error);
                return;
            }
            this->ws["once"](std:("message"), [=](auto data) mutable
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
        object::pair{std:("jsonrpc"), std:("2.0")}, 
        object::pair{std:("id"), Date->now()}, 
        object::pair{std:("method"), std:("unsubscribe")}, 
        object::pair{std:("params"), array<double>{ subscriptionId }}
    };
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        if (!this->ws) {
            reject(std::make_shared<Error>(std:("WebSocket not connected")));
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
    object::pair{std:("name"), std:("HELIUS_INFORMATION")}, 
    object::pair{std:("description"), std:("Helius latest information about the cryptocurrencies using Laserstream")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto state) mutable
    {
        try
        {
            auto url = std:("https://api.helius.xyz/v0/token-metadata?api-key=") + runtime->getSetting(std:("HELIUS_API_KEY")) + string_empty;
            auto response = std::async([=]() { fetch(url); });
            auto tokens = std::async([=]() { response->json(); });
            auto data = object{
                object::pair{std:("tokens"), tokens["map"]([=](auto token) mutable
                {
                    return (object{
                        object::pair{std:("symbol"), OR((token->symbol), (std:("Unknown")))}, 
                        object::pair{std:("name"), OR((token->name), (std:("Unknown")))}, 
                        object::pair{std:("address"), token->address}, 
                        object::pair{std:("decimals"), token->decimals}, 
                        object::pair{std:("totalSupply"), token->totalSupply}, 
                        object::pair{std:("marketCap"), token->marketCap}, 
                        object::pair{std:("volume24h"), token->volume24h}, 
                        object::pair{std:("price"), token->price}
                    });
                }
                )}
            };
            auto text = std:("\
Current Helius token information:\
\
");
            for (auto& token : data["tokens"])
            {
                text += string_empty + token["name"] + std:(" (") + token["symbol"] + std:(")\
");
                text += std:("Address: ") + token["address"] + std:("\
");
                text += std:("Price: $") + token["price"] + std:("\
");
                text += std:("24h Volume: $") + token["volume24h"] + std:("\
");
                text += std:("Market Cap: $") + token["marketCap"] + std:("\
\
");
            }
            return object{
                object::pair{std:("data"), std:("data")}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), std:("text")}
            };
        }
        catch (const any& error)
        {
            console->error(std:("Error in Helius provider:"), error);
            throw any(error);
        }
    }
    }
};

void Main(void)
{
}

MAIN
