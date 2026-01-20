#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_UTILS_ENTITY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_UTILS_ENTITY_H
#include "core.h"
#include "@elizaos/core.h"

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
            auto errorText = std::string("Unable to fetch entity information. Please try again.");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), errorText}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("Entity not found")}
                    }}
                }); });
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("result"), object{
                    object::pair{std::string("text"), errorText}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("walletCreated"), false}, 
                        object::pair{std::string("error"), true}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("actionName")}, 
                        object::pair{std::string("error"), std::string("Entity not found")}
                    }}, 
                    object::pair{std::string("error"), std::make_shared<Error>(std::string("Entity not found"))}
                }}
            };
        }
        auto walletEntityId = entity["metadata"]["author_id"];
        if (!walletEntityId) {
            auto errorText = std::string("Wallet entity ID not found in entity metadata.");
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("result"), object{
                    object::pair{std::string("text"), errorText}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("walletCreated"), false}, 
                        object::pair{std::string("error"), true}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("error"), std::string("Wallet entity ID not found")}
                    }}
                }}
            };
        }
        auto walletEntity = std::async([=]() { runtime->getEntityById(walletEntityId); });
        if (!walletEntity) {
            auto errorText = std::string("Wallet entity not found.");
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("result"), object{
                    object::pair{std::string("text"), errorText}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("walletCreated"), false}, 
                        object::pair{std::string("error"), true}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("error"), std::string("Wallet entity not found")}
                    }}
                }}
            };
        }
        auto walletAddress = as<string>(walletEntity->metadata->walletAddress);
        if (!walletAddress) {
            auto errorText = std::string("Unable to fetch user's wallet information. Please create a wallet first.");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), errorText}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("Wallet not found")}
                    }}
                }); });
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("result"), object{
                    object::pair{std::string("text"), errorText}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("walletCreated"), false}, 
                        object::pair{std::string("error"), true}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("actionName")}, 
                        object::pair{std::string("error"), std::string("Wallet not found")}
                    }}, 
                    object::pair{std::string("error"), std::make_shared<Error>(std::string("Wallet not found"))}
                }}
            };
        }
        if (!walletAddress) {
            auto errorText = std::string("Wallet address not found in entity metadata.");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), errorText}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("Wallet address not found")}
                    }}
                }); });
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("result"), object{
                    object::pair{std::string("text"), errorText}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("walletCreated"), false}, 
                        object::pair{std::string("error"), true}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("actionName")}, 
                        object::pair{std::string("error"), std::string("Wallet address not found")}
                    }}, 
                    object::pair{std::string("error"), std::make_shared<Error>(std::string("Wallet address not found"))}
                }}
            };
        }
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("walletAddress"), std::string("walletAddress")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("walletAddress"), std::string("walletAddress")}, 
                object::pair{std::string("walletEntityId"), std::string("walletEntityId")}, 
                object::pair{std::string("accountName"), walletEntityId}
            }}
        };
    }
    catch (const any& error)
    {
        logger->error(std::string("Error getting entity wallet address:"), (is<Error>(error)) ? error->message : String(error));
        auto errorText = std::string("Failed to retrieve wallet information.");
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), errorText}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("error"), std::string("Wallet retrieval failed")}
                }}
            }); });
        }
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("result"), object{
                object::pair{std::string("text"), errorText}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("walletCreated"), false}, 
                    object::pair{std::string("error"), true}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("actionName")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? error->message : String(error)}
                }}, 
                object::pair{std::string("error"), (is<Error>(error)) ? error : std::make_shared<Error>(String(error))}
            }}
        };
    }
};


#endif
