#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_SCHEMAS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_SCHEMAS_H
#include "core.hpp"
#include "zod.hpp"

typedef z::infer<TokenSchema> Token;

typedef z::infer<TokenArraySchema> TokenArray;

typedef z::infer<TokenRequestSchema> TokenRequest;

typedef z::infer<TweetSchema> Tweet;

typedef z::infer<TweetArraySchema> TweetArray;

typedef z::infer<SentimentSchema> Sentiment;

typedef z::infer<SentimentArraySchema> SentimentArray;

typedef z::infer<WalletSchema> Wallet;

typedef z::infer<BuySignalSchema> BuySignal;

typedef z::infer<StatisticsSchema> Statistics;


extern any TokenSchema;
extern any TokenArraySchema;
extern any TokenRequestSchema;
extern any TweetSchema;
extern any TweetArraySchema;
extern any SentimentSchema;
extern any SentimentArraySchema;
extern any WalletSchema;
extern any BuySignalSchema;
extern any StatisticsSchema;
#endif
