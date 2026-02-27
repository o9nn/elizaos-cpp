#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-cdp/actions/cdp-tx-explorer-link.h"

std::shared_ptr<Action> cdpTxExplorerLink = object{
    object::pair{std:("name"), std:("GET_TX_EXPLORER_LINK")}, 
    object::pair{std:("similes"), array<string>{ std:("TX_LINK"), std:("TRANSACTION_LINK"), std:("ETHERSCAN_LINK"), std:("BASESCAN_LINK"), std:("POLYGONSCAN_LINK"), std:("ARBISCAN_LINK"), std:("EXPLORER_LINK"), std:("VIEW_TRANSACTION"), std:("TX_HASH_LINK") }}, 
    object::pair{std:("description"), std:("Returns a blockchain explorer link (Etherscan, Basescan, Polygonscan, Arbiscan, etc.) for a given transaction hash and network. Use this to generate clickable links to view transaction details on the appropriate blockchain explorer.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("txhash"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Transaction hash (0x-prefixed hex string, 66 characters total)")}, 
            object::pair{std:("required"), true}
        }}, 
        object::pair{std:("network"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Blockchain network: 'base', 'ethereum', 'polygon', 'arbitrum', 'optimism', 'scroll', 'base-sepolia', or 'ethereum-sepolia'")}, 
            object::pair{std:("required"), true}
        }}
    }}, 
    object::pair{std:("validate"), [=](auto _runtime, auto message) mutable
    {
        try
        {
            return true;
        }
        catch (const any& error)
        {
            logger->error(std:("[GET_TX_EXPLORER_LINK] Error validating action:"), (is<Error>(error)) ? error->message : String(error));
            return false;
        }
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std:("[GET_TX_EXPLORER_LINK] Generating transaction explorer link"));
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto txhash = params["txhash"]["trim"]();
            auto network = params["network"]["trim"]()["toLowerCase"]();
            auto inputParams = object{
                object::pair{std:("txhash"), OR((txhash), (undefined))}, 
                object::pair{std:("network"), OR((network), (undefined))}
            };
            if (!txhash) {
                auto errorMsg = std:("Missing required parameter 'txhash'. Please provide a transaction hash.");
                logger->error(std:("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), std:(" ") + errorMsg + string_empty}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("missing_required_parameter")}, 
                    object::pair{std:("input"), inputParams}
                });
                callback(object{
                    object::pair{std:("text"), errorResult->text}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("missing_required_parameter")}, 
                        object::pair{std:("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            if (!network) {
                auto errorMsg = std:("Missing required parameter 'network'. Please specify the blockchain network (e.g., 'base', 'ethereum', 'polygon').");
                logger->error(std:("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), std:(" ") + errorMsg + string_empty}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("missing_required_parameter")}, 
                    object::pair{std:("input"), inputParams}
                });
                callback(object{
                    object::pair{std:("text"), errorResult->text}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("missing_required_parameter")}, 
                        object::pair{std:("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            if (!(new RegExp(std:("^0x[a-fA-F0-9]{64}")))->test(txhash)) {
                auto errorMsg = std:("Invalid transaction hash format: ") + txhash + std:(". Expected 0x-prefixed hex string with 64 characters (66 total).");
                logger->error(std:("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), std:(" ") + errorMsg + string_empty}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("invalid_txhash_format")}, 
                    object::pair{std:("input"), inputParams}
                });
                callback(object{
                    object::pair{std:("text"), errorResult->text}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("invalid_txhash_format")}, 
                        object::pair{std:("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            auto supportedNetworks = array<string>{ std:("base"), std:("ethereum"), std:("polygon"), std:("arbitrum"), std:("optimism"), std:("scroll"), std:("base-sepolia"), std:("ethereum-sepolia") };
            if (!supportedNetworks->includes(network)) {
                auto errorMsg = std:("Unsupported network: ") + network + std:(". Supported networks: ") + supportedNetworks->join(std:(", ")) + string_empty;
                logger->error(std:("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), std:(" ") + errorMsg + string_empty}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("unsupported_network")}, 
                    object::pair{std:("input"), inputParams}
                });
                callback(object{
                    object::pair{std:("text"), errorResult->text}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("unsupported_network")}, 
                        object::pair{std:("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            auto explorerLink = getTxExplorerUrl(network, txhash);
            if (!explorerLink) {
                auto errorMsg = std:("Could not generate explorer link for network: ") + network + string_empty;
                logger->error(std:("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), std:(" ") + errorMsg + string_empty}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("link_generation_failed")}, 
                    object::pair{std:("input"), inputParams}
                });
                callback(object{
                    object::pair{std:("text"), errorResult->text}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("link_generation_failed")}, 
                        object::pair{std:("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            logger->info(std:("[GET_TX_EXPLORER_LINK] Generated explorer link: ") + explorerLink + string_empty);
            auto networkName = network["charAt"](0)["toUpperCase"]() + network["slice"](1)["replace"](std:("-"), std:(" "));
            auto text = std:(" **Transaction Explorer Link**\
\
") + std:("Network: ") + networkName + std:("\
") + std:("Transaction Hash: "") + txhash + std:(""\
") + std:("Link: ") + explorerLink + string_empty;
            auto data = object{
                object::pair{std:("txhash"), std:("txhash")}, 
                object::pair{std:("network"), std:("network")}, 
                object::pair{std:("explorerLink"), std:("explorerLink")}, 
                object::pair{std:("explorerUrl"), explorerLink}
            };
            callback(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("content"), data}
            });
            return as<any>(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), std:("data")}, 
                object::pair{std:("values"), object{
                    object::pair{std:("explorerLink"), std:("explorerLink")}, 
                    object::pair{std:("txhash"), std:("txhash")}, 
                    object::pair{std:("network"), std:("network")}
                }}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto errorMessage = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_TX_EXPLORER_LINK] Action failed:"), errorMessage);
            auto errorText = std:(" Failed to generate transaction explorer link: ") + errorMessage + string_empty;
            auto errorResult = as<any>(object{
                object::pair{std:("text"), errorText}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), errorMessage}, 
                object::pair{std:("input"), object{}}
            });
            callback(object{
                object::pair{std:("text"), errorText}, 
                object::pair{std:("content"), object{
                    object::pair{std:("error"), std:("action_failed")}, 
                    object::pair{std:("details"), errorMessage}
                }}
            });
            return errorResult;
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("get link for transaction 0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef on base")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:(" Generating transaction explorer link...")}, 
            object::pair{std:("action"), std:("GET_TX_EXPLORER_LINK")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("show me the etherscan link for tx 0xabcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:(" Generating transaction explorer link...")}, 
            object::pair{std:("action"), std:("GET_TX_EXPLORER_LINK")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("link to basescan for transaction hash 0x9876543210fedcba9876543210fedcba9876543210fedcba9876543210fedcba")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:(" Generating transaction explorer link...")}, 
            object::pair{std:("action"), std:("GET_TX_EXPLORER_LINK")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
