#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_SCHEMAS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_SCHEMAS_H
#include "core.hpp"
#include <string>
#include "zod.hpp"
#include "./types.js.h"

typedef z::infer<tokenPerformanceSchema> TokenPerformance;

typedef z::infer<recommenderMetricsSchema> RecommenderMetrics;

typedef z::infer<positionSchema> Position;

typedef z::infer<tokenRecommendationSchema> TokenRecommendation;

typedef z::infer<recommendationSchema> MessageRecommendation;


extern object TransactionType;
extern any tokenPerformanceSchema;
extern any transactionSchema;
extern any recommenderMetricsSchema;
extern any positionSchema;
extern any tokenRecommendationSchema;
TokenPerformance transformTokenPerformance(any dbToken, string chain = std::string("unknown"));

std::shared_ptr<Transaction> transformTransaction(any dbTx, string positionId = std::string("unknown"), string chain = std::string("unknown"));

Position transformPosition(any dbPos);

extern any recommendationSchema;
TokenRecommendation transformTokenRecommendation(any dbRec);

#endif
