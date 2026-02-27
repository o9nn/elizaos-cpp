#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_UTILS_ENTITY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_UTILS_ENTITY_H
#include "core.hpp"
// External dependency removed

typedef any EntityWalletResponse;

class EntityWalletResult;
class EntityWalletError;

class EntityWalletResult : public object, public std::enable_shared_from_this<EntityWalletResult> {
public:
    using std::enable_shared_from_this<EntityWalletResult>::shared_from_this;
    boolean success;

    string walletAddress;

    Record<string, any> metadata;
};

class EntityWalletError : public object, public std::enable_shared_from_this<EntityWalletError> {
public:
    using std::enable_shared_from_this<EntityWalletError>::shared_from_this;
    boolean success;

    std::shared_ptr<ActionResult> result;
};

template <typename RET>
RET getEntityWallet(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, string actionName, std::shared_ptr<HandlerCallback> callback = undefined);

template <typename RET>
RET getEntityWallet(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, string actionName, std::shared_ptr<HandlerCallback> callback)
{
    try
    {
        auto entityId = message->entityId;
        auto entity = as<any>((std::async([=]() { runtime->getEntityById(entityId); })));
        if (!entity) {
            auto errorText = std:("Unable to fetch entity information. Please try again.");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), errorText}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("Entity not found")}
                    }}
                }); });
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("result"), object{
                    object::pair{std:("text"), errorText}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("walletCreated"), false}, 
                        object::pair{std:("error"), true}
                    }}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("actionName")}, 
                        object::pair{std:("error"), std:("Entity not found")}
                    }}, 
                    object::pair{std:("error"), std::make_shared<Error>(std:("Entity not found"))}
                }}
            };
        }
        auto walletEntityId = entity["metadata"]["author_id"];
        if (!walletEntityId) {
            auto errorText = std:("Wallet entity ID not found in entity metadata.");
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("result"), object{
                    object::pair{std:("text"), errorText}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("walletCreated"), false}, 
                        object::pair{std:("error"), true}
                    }}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("error"), std:("Wallet entity ID not found")}
                    }}
                }}
            };
        }
        auto walletEntity = std::async([=]() { runtime->getEntityById(walletEntityId); });
        if (!walletEntity) {
            auto errorText = std:("Wallet entity not found.");
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("result"), object{
                    object::pair{std:("text"), errorText}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("walletCreated"), false}, 
                        object::pair{std:("error"), true}
                    }}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("error"), std:("Wallet entity not found")}
                    }}
                }}
            };
        }
        auto walletAddress = as<string>(walletEntity->metadata->walletAddress);
        if (!walletAddress) {
            auto errorText = std:("Unable to fetch user's wallet information. Please create a wallet first.");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), errorText}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("Wallet not found")}
                    }}
                }); });
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("result"), object{
                    object::pair{std:("text"), errorText}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("walletCreated"), false}, 
                        object::pair{std:("error"), true}
                    }}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("actionName")}, 
                        object::pair{std:("error"), std:("Wallet not found")}
                    }}, 
                    object::pair{std:("error"), std::make_shared<Error>(std:("Wallet not found"))}
                }}
            };
        }
        if (!walletAddress) {
            auto errorText = std:("Wallet address not found in entity metadata.");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), errorText}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("Wallet address not found")}
                    }}
                }); });
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("result"), object{
                    object::pair{std:("text"), errorText}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("walletCreated"), false}, 
                        object::pair{std:("error"), true}
                    }}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("actionName")}, 
                        object::pair{std:("error"), std:("Wallet address not found")}
                    }}, 
                    object::pair{std:("error"), std::make_shared<Error>(std:("Wallet address not found"))}
                }}
            };
        }
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("walletAddress"), std:("walletAddress")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("walletAddress"), std:("walletAddress")}, 
                object::pair{std:("walletEntityId"), std:("walletEntityId")}, 
                object::pair{std:("accountName"), walletEntityId}
            }}
        };
    }
    catch (const any& error)
    {
        logger->error(std:("Error getting entity wallet address:"), (is<Error>(error)) ? error->message : String(error));
        auto errorText = std:("Failed to retrieve wallet information.");
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), errorText}, 
                object::pair{std:("content"), object{
                    object::pair{std:("error"), std:("Wallet retrieval failed")}
                }}
            }); });
        }
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("result"), object{
                object::pair{std:("text"), errorText}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("values"), object{
                    object::pair{std:("walletCreated"), false}, 
                    object::pair{std:("error"), true}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("actionName")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}
                }}, 
                object::pair{std:("error"), (is<Error>(error)) ? error : std::make_shared<Error>(String(error))}
            }}
        };
    }
};


#endif
