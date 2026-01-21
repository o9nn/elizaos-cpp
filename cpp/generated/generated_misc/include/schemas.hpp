#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_SCHEMAS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_SCHEMAS_H
#include "core.h"
#include "zod.h"
#include "./types.js.h"

typedef z::infer<tokenPerformanceSchema> TokenPerformance;

typedef z::infer<recommenderMetricsSchema> RecommenderMetrics;

typedef z::infer<positionSchema> Position;

typedef z::infer<tokenRecommendationSchema> TokenRecommendation;

typedef z::infer<recommendationSchema> MessageRecommendation;


extern object TransactionType;
extern std::any tokenPerformanceSchema;
extern std::any transactionSchema;
extern std::any recommenderMetricsSchema;
extern std::any positionSchema;
extern std::any tokenRecommendationSchema;
TokenPerformance transformTokenPerformance(std::any dbToken, std::string chain = std::string("unknown"));

std::shared_ptr<Transaction> transformTransaction(std::any dbTx, std::string positionId = std::string("unknown"), std::string chain = std::string("unknown"));

Position transformPosition(std::any dbPos);

extern std::any recommendationSchema;
TokenRecommendation transformTokenRecommendation(std::any dbRec);

#endif
