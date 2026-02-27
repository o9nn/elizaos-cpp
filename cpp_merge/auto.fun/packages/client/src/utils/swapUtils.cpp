#include "swapUtils.hpp"

double convertToBasisPoints(double feePercent)
{
    if (feePercent >= 1) {
        return feePercent;
    }
    return Math->floor(feePercent * 10000);
};


double calculateAmountOutSell(double reserveLamport, double amount, double _tokenDecimals, double platformSellFee, double reserveToken)
{
    if (reserveLamport < 0) throw any(std::make_shared<Error>(std:("reserveLamport must be non-negative")));
    if (amount < 0) throw any(std::make_shared<Error>(std:("amount must be non-negative")));
    if (platformSellFee < 0) throw any(std::make_shared<Error>(std:("platformSellFee must be non-negative")));
    if (reserveToken < 0) throw any(std::make_shared<Error>(std:("reserveToken must be non-negative")));
    auto feeBasisPoints = convertToBasisPoints(platformSellFee);
    auto amountBN = std::make_shared<BN>(amount);
    auto adjustedAmount = amountBN->mul(std::make_shared<BN>(10000 - feeBasisPoints))->div(std::make_shared<BN>(10000));
    auto numerator = ((std::make_shared<BN>(reserveLamport->toString())))->mul(adjustedAmount);
    auto denominator = ((std::make_shared<BN>(reserveToken->toString())))->add(adjustedAmount);
    if (denominator->isZero()) throw any(std::make_shared<Error>(std:("Division by zero")));
    return numerator->div(denominator)->toNumber();
};


double calculateAmountOutBuy(double reserveToken, double amount, double _solDecimals, double reserveLamport, double platformBuyFee)
{
    auto feeBasisPoints = std::make_shared<BN>(convertToBasisPoints(platformBuyFee));
    auto amountBN = std::make_shared<BN>(amount);
    auto adjustedAmount = amountBN->mul(std::make_shared<BN>(10000))->sub(feeBasisPoints)->div(std::make_shared<BN>(10000));
    auto reserveTokenBN = std::make_shared<BN>(reserveToken->toString());
    auto numerator = (as<any>(reserveTokenBN))["mul"](adjustedAmount);
    auto denominator = ((std::make_shared<BN>(reserveLamport->toString())))->add(adjustedAmount);
    auto out = numerator["div"](denominator)["toNumber"]();
    return out;
};


std::function<std::shared_ptr<Promise<std::shared_ptr<...>>>(any, double, double, double, string, string, string, double, double, any, any, any, object)> launchAndSwapTx = [=](auto creator, auto decimals, auto tokenSupply, auto virtualLamportReserves, auto name, auto symbol, auto uri, auto swapAmount, auto slippageBps = 100, auto connection = undefined, auto program = undefined, auto mintKeypair = undefined, auto configAccount = undefined) mutable
{
    auto deadline = Math->floor(Date->now() / 1000) + 120;
    auto initBondingCurvePercentage = configAccount["initBondingCurve"];
    auto initBondingCurveAmount = (tokenSupply * initBondingCurvePercentage) / 100;
    auto numerator = virtualLamportReserves * swapAmount;
    auto denominator = initBondingCurveAmount + swapAmount;
    auto expectedOutput = Math->floor(numerator / denominator);
    auto minOutput = Math->floor((expectedOutput * (10000 - slippageBps)) / 10000);
    auto tx = std::async([=]() { program->methods->launchAndSwap(decimals, std::make_shared<BN>(tokenSupply), std::make_shared<BN>(virtualLamportReserves), name, symbol, uri, std::make_shared<BN>(swapAmount), std::make_shared<BN>(minOutput), std::make_shared<BN>(deadline))->accounts(object{
        object::pair{std:("teamWallet"), configAccount["teamWallet"]}, 
        object::pair{std:("creator"), creator}, 
        object::pair{std:("token"), mintKeypair->publicKey}
    })->transaction(); });
    tx->feePayer = creator;
    tx->recentBlockhash = (std::async([=]() { connection->getLatestBlockhash(); }))->blockhash;
    return tx;
};
double FEE_BASIS_POINTS = 10000;
std::function<std::shared_ptr<Promise<object>>(any, double, double, double, double)> getSwapAmount = [=](auto program, auto amount, auto style, auto reserveToken, auto reserveLamport) mutable
{
    auto configAccount = std::async([=]() { getConfigAccount(program); });
    auto feePercent = (style == 1) ? Number(configAccount->platformSellFee) : Number(configAccount->platformBuyFee);
    auto adjustedAmount = Math->floor((amount * (FEE_BASIS_POINTS - feePercent)) / FEE_BASIS_POINTS);
    any estimatedOutput;
    if (style == 0) {
        estimatedOutput = calculateAmountOutBuy(reserveToken, adjustedAmount, 9, reserveLamport, feePercent);
    } else {
        estimatedOutput = calculateAmountOutSell(reserveLamport, adjustedAmount, 6, feePercent, reserveToken);
    }
    return object{
        object::pair{std:("estimatedOutput"), estimatedOutput}, 
        object::pair{std:("priceImpact"), std:("0")}
    };
};
std::function<std::shared_ptr<Promise<object>>(string, double, double, double)> getSwapAmountJupiter = [=](auto tokenMintAddress, auto amount, auto style, auto slippageBps = 100) mutable
{
    try
    {
        if (amount == 0) return object{
            object::pair{std:("estimatedOutput"), 0}, 
            object::pair{std:("priceImpact"), std:("0")}
        };
        auto SOL_MINT_ADDRESS = std:("So11111111111111111111111111111111111111112");
        auto inputMint = (style == 0) ? any(SOL_MINT_ADDRESS) (tokenMintAddress);
        auto outputMint = (style == 0) ? any(tokenMintAddress) (SOL_MINT_ADDRESS);
        auto feePercent = 1;
        auto feeBps = feePercent * 100;
        auto quoteUrl = std:("https://lite-api.jup.ag/swap/v1/quote?inputMint=") + inputMint + std:("&outputMint=") + outputMint + std:("&amount=") + amount + std:("&slippageBps=") + slippageBps + std:("&restrictIntermediateTokens=true&platformFeeBps=") + feeBps + string_empty;
        auto quoteRes = std::async([=]() { fetch(quoteUrl); });
        if (!quoteRes->ok) {
            auto errorMsg = std::async([=]() { quoteRes->text(); });
            throw any(std::make_shared<Error>(std:("Failed to fetch quote from Jupiter: ") + errorMsg + string_empty));
        }
        auto quoteResponse = as<object>((std::async([=]() { quoteRes->json(); })));
        auto estimatedOutput = quoteResponse["outAmount"];
        return object{
            object::pair{std:("estimatedOutput"), Number(estimatedOutput)}, 
            object::pair{std:("priceImpact"), OR((quoteResponse["priceImpact"]), (std:("0")))}
        };
    }
    catch (const any& error)
    {
        console->error(std:("Error fetching swap amount from Jupiter:"), error);
        return object{
            object::pair{std:("estimatedOutput"), 0}, 
            object::pair{std:("priceImpact"), std:("0")}
        };
    }
};
std::function<std::shared_ptr<Promise<any>>(any, any, double, double, double, any, double, double, any)> swapIx = [=](auto user, auto token, auto amount, auto style, auto slippageBps = 100, auto program = undefined, auto reserveToken = undefined, auto reserveLamport = undefined, auto configAccount = undefined) mutable
{
    auto estimatedOutputResult = std::async([=]() { getSwapAmount(program, amount, style, reserveToken, reserveLamport); });
    auto estimatedOutput = estimatedOutputResult["estimatedOutput"];
    auto minOutput = std::make_shared<BN>(Math->floor((estimatedOutput * (10000 - slippageBps)) / 10000));
    auto deadline = Math->floor(Date->now() / 1000) + 120;
    auto tx = std::async([=]() { program->methods->swap(std::make_shared<BN>(amount), style, minOutput, std::make_shared<BN>(deadline))->accounts(object{
        object::pair{std:("teamWallet"), configAccount->teamWallet}, 
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("tokenMint"), token}
    })->instruction(); });
    return tx;
};
std::function<std::shared_ptr<Promise<any>>(any, any, double, double, double, any, boolean)> getJupiterSwapIx = [=](auto user, auto _token, auto amount, auto style, auto slippageBps = 100, auto _connection = undefined, auto isToken2022 = false) mutable
{
    auto SOL_MINT_ADDRESS = std:("So11111111111111111111111111111111111111112");
    auto tokenMintAddress = _token->toBase58();
    auto inputMint = (style == 0) ? any(SOL_MINT_ADDRESS) (tokenMintAddress);
    auto outputMint = (style == 0) ? any(tokenMintAddress) (SOL_MINT_ADDRESS);
    auto feeAccount = undefined;
    auto feePercent = 10;
    auto feeBps = Math->round(feePercent * 10);
    if (!isToken2022) {
        console->log(std:("Using Jupiter platform fee."));
        auto platformFeeWallet = env["platformFeeTokenAccount"];
        if (platformFeeWallet) {
            feeAccount = std::make_shared<PublicKey>(platformFeeWallet);
            console->log(std:("Platform fee account:"), feeAccount["toBase58"]());
        } else {
            console->warn(std:("Platform fee wallet address not found in env—skipping Jupiter platform fee."));
        }
    }
    auto quoteUrl = std:("https://lite-api.jup.ag/swap/v1/quote?inputMint=") + inputMint + std:("&outputMint=") + outputMint + string_empty + std:("&amount=") + amount + std:("&slippageBps=") + slippageBps + std:("&restrictIntermediateTokens=true") + string_empty + (!isToken2022) ? any(std:("&platformFeeBps=") + feeBps + string_empty) (string_empty) + string_empty;
    auto quoteRes = std::async([=]() { fetch(quoteUrl); });
    if (!quoteRes->ok) throw any(std::make_shared<Error>(std::async([=]() { quoteRes->text(); })));
    auto quoteResponse = std::async([=]() { quoteRes->json(); });
    auto body = object{
        object::pair{std:("quoteResponse"), std:("quoteResponse")}, 
        object::pair{std:("userPublicKey"), user->toBase58()}, 
        object::pair{std:("asLegacyTransaction"), true}, 
        object::pair{std:("dynamicComputeUnitLimit"), true}, 
        object::pair{std:("dynamicSlippage"), true}
    };
    if (feeAccount) {
        body["feeAccount"] = feeAccount;
    }
    auto swapRes = std::async([=]() { fetch(std:("https://lite-api.jup.ag/swap/v1/swap"), object{
        object::pair{std:("method"), std:("POST")}, 
        object::pair{std:("headers"), object{
            object::pair{std:("Content-Type"), std:("application/json")}
        }}, 
        object::pair{std:("body"), JSON->stringify(body)}
    }); });
    if (!swapRes->ok) throw any(std::make_shared<Error>(std::async([=]() { swapRes->text(); })));
    auto swapJson = std::async([=]() { swapRes->json(); });
    if (!swapJson["swapTransaction"]) {
        throw any(std::make_shared<Error>(std:("Jupiter swap transaction is missing in the response.")));
    }
    auto txBuffer = Buffer::from(swapJson["swapTransaction"], std:("base64"));
    return Transaction->from(txBuffer)->instructions;
};

void Main(void)
{
}

MAIN
