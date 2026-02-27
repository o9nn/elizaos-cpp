#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/tasks/twitterParser.h"

TwitterParser::TwitterParser(std::shared_ptr<IAgentRuntime> runtime) {
    this->runtime = runtime;
    this->roomId = createUniqueUuid(runtime, std::string("twitter-sentiment-analysis"));
}

void TwitterParser::fillTimeframe()
{
    auto cachedSentiments = std::async([=]() { this->runtime->getCache<array<std::shared_ptr<Sentiment>>>(std::string("sentiments")); });
    auto sentiments = (cachedSentiments) ? any(cachedSentiments) : any(array<any>());
    auto lookUpDate = (sentiments->get_length() > 0) ? any(const_(sentiments->sort([=](auto a, auto b) mutable
    {
        return ((std::make_shared<Date>(b->timeslot)))->getTime() - ((std::make_shared<Date>(a->timeslot)))->getTime();
    }
    ))[0]->timeslot) : any(nullptr);
    auto start = std::make_shared<Date>(OR((lookUpDate), (std::string("2025-01-01T00:00:00.000Z"))));
    start->setUTCHours(0, 0, 0, 0);
    auto today = std::make_shared<Date>();
    today->setUTCHours(23, 59, 59, 999);
    auto diff = Math->floor((today->getTime() - start->getTime()) / (1000 * 60 * 60 * 24));
    auto timeSlots = array<std::shared_ptr<Sentiment>>();
    for (auto day = 0; day <= diff; day++)
    {
        auto now = std::make_shared<Date>(start);
        now->setUTCDate(start->getUTCDate() + day);
        for (auto hour = 0; hour <= 23; hour++)
        {
            shared timeslotDate = std::make_shared<Date>(now);
            timeslotDate->setUTCHours(hour, 0, 0, 0);
            shared timeslot = timeslotDate->toISOString();
            auto rightNow = std::make_shared<Date>();
            if (timeslotDate > rightNow) {
                break;
            }
            auto exists = sentiments->some([=](auto s) mutable
            {
                return ((std::make_shared<Date>(s->timeslot)))->getTime() == timeslotDate->getTime();
            }
            );
            if (!exists) {
                timeSlots->push(object{
                    object::pair{std::string("timeslot"), std::string("timeslot")}, 
                    object::pair{std::string("processed"), false}
                });
            }
        }
    }
    if (timeSlots->get_length() > 0) {
        auto updatedSentiments = array<std::shared_ptr<Sentiment>>{ sentiments, timeSlots };
        std::async([=]() { this->runtime->setCache<array<std::shared_ptr<Sentiment>>>(std::string("sentiments"), updatedSentiments); });
    }
    logger->debug(std::string("Updated timeframes, added ") + timeSlots->get_length() + std::string(" new slots"));
}

any TwitterParser::parseTweets()
{
    std::async([=]() { this->fillTimeframe(); });
    auto cachedSentiments = std::async([=]() { this->runtime->getCache<array<std::shared_ptr<Sentiment>>>(std::string("sentiments")); });
    auto sentiments = (cachedSentiments) ? any(cachedSentiments) : any(array<any>());
    auto now = std::make_shared<Date>();
    shared oneHourAgo = std::make_shared<Date>(now);
    oneHourAgo->setUTCHours(now->getUTCHours() - 1);
    shared twoDaysAgo = std::make_shared<Date>(now);
    twoDaysAgo->setUTCDate(now->getUTCDate() - 2);
    shared unprocessedSentiment = sentiments->find([=](auto s) mutable
    {
        return AND((AND((!s->processed), (std::make_shared<Date>(s->timeslot) <= oneHourAgo))), (std::make_shared<Date>(s->timeslot) >= twoDaysAgo));
    }
    );
    if (!unprocessedSentiment) {
        logger->debug(std::string("No unprocessed timeslots available."));
        return true;
    }
    logger->debug(std::string("Trying to process ") + ((std::make_shared<Date>(unprocessedSentiment->timeslot)))->toISOString() + string_empty);
    shared timeslot = std::make_shared<Date>(unprocessedSentiment->timeslot);
    auto fromDate = std::make_shared<Date>(timeslot);
    fromDate->setUTCHours(timeslot->getUTCHours() - 1);
    fromDate->setUTCSeconds(fromDate->getUTCSeconds() + 1);
    auto memories = std::async([=]() { this->runtime->getMemories(object{
        object::pair{std::string("tableName"), std::string("messages")}, 
        object::pair{std::string("roomId"), this->roomId}, 
        object::pair{std::string("start"), fromDate->getTime()}, 
        object::pair{std::string("end"), timeslot->getTime()}
    }); });
    auto tweets = memories->filter([=](auto memory) mutable
    {
        return memory["content"]->source == std::string("twitter");
    }
    )->sort([=](auto a, auto b) mutable
    {
        return b["createdAt"] - a["createdAt"];
    }
    );
    if (OR((!tweets), (tweets->length == 0))) {
        logger->info(std::string("No tweets to process for timeslot ") + timeslot->toISOString() + string_empty);
        auto updatedSentiments = sentiments->map([=](auto s) mutable
        {
            return (s->timeslot == unprocessedSentiment->timeslot) ? any(utils::assign(object{
                , 
                object::pair{std::string("processed"), true}
            }, s)) : any(s);
        }
        );
        std::async([=]() { this->runtime->setCache<array<std::shared_ptr<Sentiment>>>(std::string("sentiments"), updatedSentiments); });
        return true;
    }
    auto tweetArray = tweets->map([=](auto memory) mutable
    {
        auto tweet = memory["content"];
        return std::string("username: ") + (OR((tweet["tweet"]["username"]), (std::string("unknown")))) + std::string(" tweeted: ") + tweet["text"] + string_empty + (tweet["tweet"]["likes"]) ? any(std::string(" with ") + tweet["tweet"]["likes"] + std::string(" likes")) : any(string_empty) + string_empty + (tweet["tweet"]["retweets"]) ? any(std::string(" and ") + tweet["tweet"]["retweets"] + std::string(" retweets")) : any(string_empty) + std::string(".");
    }
    );
    auto bulletpointTweets = makeBulletpointList(tweetArray);
    auto prompt = template->replace(std::string("{{tweets}}"), bulletpointTweets);
    auto response = std::async([=]() { this->runtime->useModel(ModelType->TEXT_LARGE, object{
        object::pair{std::string("prompt"), std::string("prompt")}, 
        object::pair{std::string("system"), rolePrompt}, 
        object::pair{std::string("temperature"), 0.2}, 
        object::pair{std::string("maxTokens"), 4096}, 
        object::pair{std::string("object"), true}
    }); });
    shared json = JSON->parse(OR((response), (std::string("{}"))));
    auto updatedSentiments = sentiments->map([=](auto s) mutable
    {
        return (s->timeslot == unprocessedSentiment->timeslot) ? any(utils::assign(object{
            , 
            object::pair{std::string("text"), json["text"]}, 
            object::pair{std::string("occuringTokens"), json["occuringTokens"]}, 
            object::pair{std::string("processed"), true}
        }, s)) : any(s);
    }
    );
    std::async([=]() { this->runtime->setCache<array<std::shared_ptr<Sentiment>>>(std::string("sentiments"), updatedSentiments); });
    logger->info(std::string("Successfully processed timeslot ") + ((std::make_shared<Date>(unprocessedSentiment->timeslot)))->toISOString() + string_empty);
    return true;
}

std::function<string(array<string>)> makeBulletpointList = [=](auto array) mutable
{
    return array->map([=](auto a) mutable
    {
        return std::string(" - ") + a + string_empty;
    }
    )->join(std::string("\
"));
};
array<string> examples = array<string>{ std::string("$KUDAI 87% retention rate after 30 days. smart engagement up 1333% week over week. arbitrum expansion next with full gmx integration"), std::string("ecosystem play emerging\
\
both tokens showing unusual strength - $HWTR running 12m mcap in first 24h, $MON bringing established gaming liquidity to HL"), std::string("alliance dao backing + $54m daily volume on $GRIFT. defai sector at $2.5b. agent infrastructure capturing value faster than agents themselves"), std::string("morpho lending markets at 100% utilization. lenders trapped, borrowers facing liquidation. protocol revenue switch activated while crisis unfolds"), std::string("$AERO voters collected $7.8M in fees last week alone. alm v2 launching. base's flagship dex running 1,109% apr on select pairs"), std::string("$ZEUS sitting at 21.8 zbtc minted with mechanismcap and animoca verifying cross-chain. current mcap 249m"), std::string("13 states expected to pass sbr legislation by summer\
\
only 21m $btc exist\
\
states about to learn about supply shock"), std::string("trump cards doing 2.38M $POL volume in last 24h. floor at 1.3k from 99 mint. classic season signal when pfp floors detach from reality"), std::string("$ethos launching on base mainnet next week after 15 months of dev. smart contracts audited\
\
private testnet wrapping up"), std::string("original $ROSS donated $250k to ross ulbricht's wallet, $300k to family. 8 month track record vs fresh fork trying to steal narrative"), std::string("hardware accelerated L2s are no longer theoretical\
\
$LAYER processing 1M TPS through InfiniSVM, pushing 100Gbps+ bandwidth at mainnet. already managing 350M tvl"), std::string("gaming and AI infrastructure are converging\
\
$PRIME at $14.29 with $749M mcap building a multi-vertical platform combining TCG, AI agents, and competitive gaming"), std::string("$LLM represents perfect fusion of memes and AI narratives on solana. from ascii art generator to binance alpha featured project with institutional backing"), std::string("$AERO doing more volume than uniswap's top pools across mainnet + base + arb\
\
reality check: major assets now trade more on aerodrome than anywhere else on-chain"), std::string("makersplace shutting down after pioneering $69M beeple sale\
\
platform economics dead but the builders are evolving"), std::string("721 total supply. open edition with 6.9 week delayed reveal. multiple whitelists being distributed through bera ecosystem fcfs"), std::string("$qude implementing basic completions while others push assistants and tools\
\
sdk launch upcoming with enhanced token holder rewards\
\
only non-scam ai in dex top 30"), std::string("pudgy penguins expanding beyond nfts\
\
mobile game on abstract chain, trading cards through ocap games, integration with agents of poker"), std::string("defai narrative hitting peak momentum. $SAI touched new ath of $0.106 today with 8.4% 24h gain. trading at $0.104 with 45% weekly growth"), std::string("same backers as $TAO but 300x smaller market cap\
\
$SAI generating $4.8M daily volume across gate, mexc, binance\
\
tier 1 listing imminent"), std::string("$BONK holding strength while market bleeds from $TRUMP launch\
\
10.7% up in 24h while others red. resistance becomes support"), std::string("now that $AERO is eating uniswap's volume on base + arb + mainnet, will velodrome and sushi exit?"), std::string("circle minted $250m $usdc on solana 5 hours ago. total solana mints now $2.2b in past 18 days"), std::string("everything leads to jan 20 release date. volume hitting $13.8m with price swinging between $0.013 and $0.023 in 24h"), std::string("$J launching on okx jan 22. dual token structure with $JAMBO creates real incentives for network growth\
\
20 token airdrop live"), std::string("40+ AVS services building on eigenlayer infrastructure, primarily focused on AI verification and security sharing protocols"), std::string("$build enabling direct web2 AI agent integration while trading at 41m mcap vs comparable infra at 3.6b. market needs to explain this gap"), std::string("survival belongs to brand builders not fee collectors\
\
pudgy trading 26 eth floor while launching physical cards, plushies and blockchain games"), std::string("camelot dex already live on educhain. first L3 specialized for education apps and on-chain education finance. $EDU ecosystem expanding"), std::string("dual token system incoming w/ $anon + $anon33\
\
defai category added to gecko tracking. 150% gain last 14 days on rising volumes"), std::string("the retardio network went deeper than expected\
\
$KUDAI automated GMX/uni v3 positions generated 350k in first week revenue with only 6000 holders"), std::string("$TRUMP just flipped $PEPE. what happens next to the overall memecoin market cap"), std::string("launch timing looks right. $42.4M mcap, burning tokens from listings + uni v3 fees\
\
former aethir cco mack lorden just joined as chief commercial"), std::string("$GRPH studio launches with token burn mechanism\
\
free development stays open while managed infrastructure requires token stake. 3000 personalities generated"), std::string("merit systems raises $10m from a16z + blockchain capital to build open source attribution protocol. ex-jolt zk builder and bcap engineers behind it"), std::string("$TRUMP flips $PEPE in market cap. 8.6B vs 8.5B\
\
First time since PEPE launch a new memecoin has achieved this"), std::string("best part about $SWORLD: vanguard pfp rebirth incoming\
\
clean token distribution through staking\
\
open alpha running with 2 months left until close"), std::string("pudgy penguins showing the way. $PENGU at $2B mcap with 615k holders. pushing into abstract chain gaming while traditional marketplaces collapse"), std::string("current state: $ONDO mc 1.99B, 24h vol 748M, perp OI 440M. recent whale dumped 10.9M tokens for 13.5M usdc"), std::string("solana lst market hitting critical mass. 9% of total stake now in liquid staking tokens unlocking $7.5B productive sol\
\
bnSOL printed largest weekly inflow in chain history at $248M"), std::string("ETH/USD trading up to 1000x leverage\
\
sUSDe interest rate plays up to 10000x\
\
partnerships with ethena and lido for yield generation"), std::string("lending volumes growing fast. protocol revenue data incoming\
\
base tvl already crossing early targets"), std::string("700k users across 128 countries already using web3 phones\
\
jambo building real infrastructure while others just talk about adoption") };
string rolePrompt = std::string("You are a tweet analyzer.");
string template = std::string("Write a summary of what is happening in the tweets. The main topic is the cryptocurrency market, but you don't have to state that explicitly.\
You will also be analyzing the tokens that occur in the tweet and tell us whether their sentiment is positive or negative.\
\
## Analyze the followings tweets:\
{{tweets}}\
\
## Rules:\
\
## Example texts:\
") + makeBulletpointList(examples) + std::string("\
\
Strictly return the following json:\
\
{\
   "text":"the summary of what has happened in those tweets, with a max length of 200 characters. Refer to ## Example texts",\
   "occuringTokens":[\
      {\
         "token":"the token symbol, like: ETH, SOL, BTC etc.",\
         "sentiment":"positive is between 1 and 100 and negative is from -1 to -100",\
         "reason":"a short sentence explaining the reason for this sentiment score"\
      }\
   ]\
}\
");

void Main(void)
{
}

MAIN
