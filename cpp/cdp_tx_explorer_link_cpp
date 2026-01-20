#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-cdp/actions/cdp-tx-explorer-link.h"

std::shared_ptr<Action> cdpTxExplorerLink = object{
    object::pair{std::string("name"), std::string("GET_TX_EXPLORER_LINK")}, 
    object::pair{std::string("similes"), array<string>{ std::string("TX_LINK"), std::string("TRANSACTION_LINK"), std::string("ETHERSCAN_LINK"), std::string("BASESCAN_LINK"), std::string("POLYGONSCAN_LINK"), std::string("ARBISCAN_LINK"), std::string("EXPLORER_LINK"), std::string("VIEW_TRANSACTION"), std::string("TX_HASH_LINK") }}, 
    object::pair{std::string("description"), std::string("Returns a blockchain explorer link (Etherscan, Basescan, Polygonscan, Arbiscan, etc.) for a given transaction hash and network. Use this to generate clickable links to view transaction details on the appropriate blockchain explorer.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("txhash"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Transaction hash (0x-prefixed hex string, 66 characters total)")}, 
            object::pair{std::string("required"), true}
        }}, 
        object::pair{std::string("network"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Blockchain network: 'base', 'ethereum', 'polygon', 'arbitrum', 'optimism', 'scroll', 'base-sepolia', or 'ethereum-sepolia'")}, 
            object::pair{std::string("required"), true}
        }}
    }}, 
    object::pair{std::string("validate"), [=](auto _runtime, auto message) mutable
    {
        try
        {
            return true;
        }
        catch (const any& error)
        {
            logger->error(std::string("[GET_TX_EXPLORER_LINK] Error validating action:"), (is<Error>(error)) ? error->message : String(error));
            return false;
        }
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std::string("[GET_TX_EXPLORER_LINK] Generating transaction explorer link"));
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto txhash = params["txhash"]["trim"]();
            auto network = params["network"]["trim"]()["toLowerCase"]();
            auto inputParams = object{
                object::pair{std::string("txhash"), OR((txhash), (undefined))}, 
                object::pair{std::string("network"), OR((network), (undefined))}
            };
            if (!txhash) {
                auto errorMsg = std::string("Missing required parameter 'txhash'. Please provide a transaction hash.");
                logger->error(std::string("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), std::string(" ") + errorMsg + string_empty}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("missing_required_parameter")}, 
                    object::pair{std::string("input"), inputParams}
                });
                callback(object{
                    object::pair{std::string("text"), errorResult->text}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("missing_required_parameter")}, 
                        object::pair{std::string("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            if (!network) {
                auto errorMsg = std::string("Missing required parameter 'network'. Please specify the blockchain network (e.g., 'base', 'ethereum', 'polygon').");
                logger->error(std::string("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), std::string(" ") + errorMsg + string_empty}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("missing_required_parameter")}, 
                    object::pair{std::string("input"), inputParams}
                });
                callback(object{
                    object::pair{std::string("text"), errorResult->text}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("missing_required_parameter")}, 
                        object::pair{std::string("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            if (!(new RegExp(std::string("^0x[a-fA-F0-9]{64}")))->test(txhash)) {
                auto errorMsg = std::string("Invalid transaction hash format: ") + txhash + std::string(". Expected 0x-prefixed hex string with 64 characters (66 total).");
                logger->error(std::string("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), std::string(" ") + errorMsg + string_empty}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("invalid_txhash_format")}, 
                    object::pair{std::string("input"), inputParams}
                });
                callback(object{
                    object::pair{std::string("text"), errorResult->text}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("invalid_txhash_format")}, 
                        object::pair{std::string("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            auto supportedNetworks = array<string>{ std::string("base"), std::string("ethereum"), std::string("polygon"), std::string("arbitrum"), std::string("optimism"), std::string("scroll"), std::string("base-sepolia"), std::string("ethereum-sepolia") };
            if (!supportedNetworks->includes(network)) {
                auto errorMsg = std::string("Unsupported network: ") + network + std::string(". Supported networks: ") + supportedNetworks->join(std::string(", ")) + string_empty;
                logger->error(std::string("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), std::string(" ") + errorMsg + string_empty}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("unsupported_network")}, 
                    object::pair{std::string("input"), inputParams}
                });
                callback(object{
                    object::pair{std::string("text"), errorResult->text}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("unsupported_network")}, 
                        object::pair{std::string("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            auto explorerLink = getTxExplorerUrl(network, txhash);
            if (!explorerLink) {
                auto errorMsg = std::string("Could not generate explorer link for network: ") + network + string_empty;
                logger->error(std::string("[GET_TX_EXPLORER_LINK] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), std::string(" ") + errorMsg + string_empty}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("link_generation_failed")}, 
                    object::pair{std::string("input"), inputParams}
                });
                callback(object{
                    object::pair{std::string("text"), errorResult->text}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("link_generation_failed")}, 
                        object::pair{std::string("details"), errorMsg}
                    }}
                });
                return errorResult;
            }
            logger->info(std::string("[GET_TX_EXPLORER_LINK] Generated explorer link: ") + explorerLink + string_empty);
            auto networkName = network["charAt"](0)["toUpperCase"]() + network["slice"](1)["replace"](std::string("-"), std::string(" "));
            auto text = std::string(" **Transaction Explorer Link**\
\
") + std::string("Network: ") + networkName + std::string("\
") + std::string("Transaction Hash: `") + txhash + std::string("`\
") + std::string("Link: ") + explorerLink + string_empty;
            auto data = object{
                object::pair{std::string("txhash"), std::string("txhash")}, 
                object::pair{std::string("network"), std::string("network")}, 
                object::pair{std::string("explorerLink"), std::string("explorerLink")}, 
                object::pair{std::string("explorerUrl"), explorerLink}
            };
            callback(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("content"), data}
            });
            return as<any>(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), std::string("data")}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("explorerLink"), std::string("explorerLink")}, 
                    object::pair{std::string("txhash"), std::string("txhash")}, 
                    object::pair{std::string("network"), std::string("network")}
                }}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto errorMessage = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_TX_EXPLORER_LINK] Action failed:"), errorMessage);
            auto errorText = std::string(" Failed to generate transaction explorer link: ") + errorMessage + string_empty;
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), errorText}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), errorMessage}, 
                object::pair{std::string("input"), object{}}
            });
            callback(object{
                object::pair{std::string("text"), errorText}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("error"), std::string("action_failed")}, 
                    object::pair{std::string("details"), errorMessage}
                }}
            });
            return errorResult;
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("get link for transaction 0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef on base")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string(" Generating transaction explorer link...")}, 
            object::pair{std::string("action"), std::string("GET_TX_EXPLORER_LINK")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("show me the etherscan link for tx 0xabcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string(" Generating transaction explorer link...")}, 
            object::pair{std::string("action"), std::string("GET_TX_EXPLORER_LINK")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("link to basescan for transaction hash 0x9876543210fedcba9876543210fedcba9876543210fedcba9876543210fedcba")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string(" Generating transaction explorer link...")}, 
            object::pair{std::string("action"), std::string("GET_TX_EXPLORER_LINK")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
