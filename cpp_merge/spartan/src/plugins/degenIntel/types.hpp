#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TYPES_H
#include "core.hpp"
// External dependency removed

typedef any TChain;

typedef any TDataProvider;

typedef object Job;

class IToken;
class ISentimentToken;
class ISentiment;
class IData;
class ITransactionHistory;
class Sentiment;
class TwitterContent;

class IToken : public object, public std::enable_shared_from_this<IToken> {
public:
    using std::enable_shared_from_this<IToken>::shared_from_this;
    TDataProvider provider;

    TChain chain;

    string address;

    double decimals;

    double liquidity;

    double marketcap;

    string logoURI;

    string name;

    string symbol;

    double volume24hUSD;

    double rank;

    double price;

    double price24hChangePercent;

    std::shared_ptr<Date> last_updated;
};

class ISentimentToken : public object, public std::enable_shared_from_this<ISentimentToken> {
public:
    using std::enable_shared_from_this<ISentimentToken>::shared_from_this;
    string token;

    double sentiment;

    string reason;
};

class ISentiment : public object, public std::enable_shared_from_this<ISentiment> {
public:
    using std::enable_shared_from_this<ISentiment>::shared_from_this;
    std::shared_ptr<Date> timeslot;

    boolean processed;

    string text;

    array<std::shared_ptr<ISentimentToken>> occuringTokens;
};

class IData : public object, public std::enable_shared_from_this<IData> {
public:
    using std::enable_shared_from_this<IData>::shared_from_this;
    any data;

    string key;
};

class ITransactionHistory : public object, public std::enable_shared_from_this<ITransactionHistory> {
public:
    using std::enable_shared_from_this<ITransactionHistory>::shared_from_this;
    string txHash;

    std::shared_ptr<Date> blockTime;

    any data;
};

class Sentiment : public object, public std::enable_shared_from_this<Sentiment> {
public:
    using std::enable_shared_from_this<Sentiment>::shared_from_this;
    string timeslot;

    boolean processed;

    string text;

    array<object> occuringTokens;
};

class TwitterContent : public Content, public std::enable_shared_from_this<TwitterContent> {
public:
    using std::enable_shared_from_this<TwitterContent>::shared_from_this;
    string text;

    string source;

    string url;

    object tweet;
};

#endif
