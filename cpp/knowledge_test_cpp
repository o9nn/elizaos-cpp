#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/__tests__/knowledge.test.h"

std::function<any(double)> createTestUUID = [=](auto num) mutable
{
    return std::string("00000000-0000-0000-0000-") + num->toString()->padStart(12, std::string("0")) + string_empty;
};

void Main(void)
{
    describe(std::string("Knowledge adapter"), [=]() mutable
    {
        shared testAgentId = createTestUUID(1);
        shared testRoomId = createTestUUID(2);
        describe(std::string("KnowledgeItem to Memory conversion"), [=]() mutable
        {
            it(std::string("should convert KnowledgeItem to Memory correctly"), [=]() mutable
            {
                auto knowledgeItem = object{
                    object::pair{std::string("id"), createTestUUID(100)}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("This is knowledge about AI")}, 
                        object::pair{std::string("source"), std::string("documentation")}
                    }}
                };
                auto memory = knowledgeItemToMemory(knowledgeItem, testAgentId, testRoomId);
                expect(memory->id)->toBe(knowledgeItem["id"]);
                expect(memory->entityId)->toBe(testAgentId);
                expect(memory->agentId)->toBe(testAgentId);
                expect(memory->content)->toEqual(knowledgeItem["content"]);
                expect(memory->roomId)->toBe(testRoomId);
                expect(memory->unique)->toBe(false);
                expect(memory->createdAt)->toBeDefined();
            }
            );
            it(std::string("should convert Memory to KnowledgeItem correctly"), [=]() mutable
            {
                auto memory = object{
                    object::pair{std::string("id"), createTestUUID(101)}, 
                    object::pair{std::string("entityId"), testAgentId}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Memory content")}, 
                        object::pair{std::string("action"), std::string("remember")}
                    }}, 
                    object::pair{std::string("roomId"), testRoomId}, 
                    object::pair{std::string("createdAt"), Date->now()}
                };
                auto knowledgeItem = memoryToKnowledgeItem(memory);
                expect(knowledgeItem["id"])->toBe(memory->id);
                expect(knowledgeItem["content"])->toEqual(memory->content);
            }
            );
        }
        );
        describe(std::string("RAGKnowledgeItem to Memory conversion"), [=]() mutable
        {
            it(std::string("should convert RAGKnowledgeItem to Memory correctly"), [=]() mutable
            {
                auto ragKnowledge = object{
                    object::pair{std::string("id"), createTestUUID(200)}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("RAG knowledge content")}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("source"), std::string("pdf")}, 
                            object::pair{std::string("type"), std::string("document")}, 
                            object::pair{std::string("isShared"), true}
                        }}
                    }}, 
                    object::pair{std::string("embedding"), std::make_shared<Float32Array>(array<double>{ 0.1, 0.2, 0.3 })}, 
                    object::pair{std::string("createdAt"), 1234567890}, 
                    object::pair{std::string("similarity"), 0.95}
                };
                auto memory = ragKnowledgeToMemory(ragKnowledge, testRoomId);
                expect(memory->id)->toBe(ragKnowledge->id);
                expect(memory->entityId)->toBe(testAgentId);
                expect(memory->agentId)->toBe(testAgentId);
                expect(memory->content->text)->toBe(std::string("RAG knowledge content"));
                expect(memory->content->source)->toBe(std::string("pdf"));
                expect(memory->content->type)->toBe(std::string("document"));
                expect(memory->content->isShared)->toBe(true);
                expect(memory->embedding)->toEqual(array<double>{ 0.1, 0.2, 0.3 });
                expect(memory->roomId)->toBe(testRoomId);
                expect(memory->createdAt)->toBe(1234567890);
                expect(memory->similarity)->toBe(0.95);
            }
            );
            it(std::string("should convert Memory to RAGKnowledgeItem correctly"), [=]() mutable
            {
                auto memory = object{
                    object::pair{std::string("id"), createTestUUID(201)}, 
                    object::pair{std::string("entityId"), testAgentId}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Memory text content")}, 
                        object::pair{std::string("source"), std::string("chat")}, 
                        object::pair{std::string("category"), std::string("conversation")}
                    }}, 
                    object::pair{std::string("embedding"), array<double>{ 0.4, 0.5, 0.6 }}, 
                    object::pair{std::string("roomId"), testRoomId}, 
                    object::pair{std::string("createdAt"), 9876543210ll}, 
                    object::pair{std::string("similarity"), 0.88}
                };
                auto ragKnowledge = memoryToRagKnowledge(memory);
                expect(ragKnowledge->id)->toBe(memory->id);
                expect(ragKnowledge->agentId)->toBe(testAgentId);
                expect(ragKnowledge->content["text"])->toBe(std::string("Memory text content"));
                expect(ragKnowledge->content["metadata"]["source"])->toBe(std::string("chat"));
                expect(ragKnowledge->content["metadata"]["category"])->toBe(std::string("conversation"));
                expect(ragKnowledge->content["metadata"]["type"])->toBe(std::string("knowledge"));
                expect(Array->from(ragKnowledge->embedding))->toEqual(array<double>{ 0.4, 0.5, 0.6 });
                expect(ragKnowledge->createdAt)->toBe(9876543210ll);
                expect(ragKnowledge->similarity)->toBe(0.88);
            }
            );
        }
        );
        describe(std::string("Knowledge scope utilities"), [=]() mutable
        {
            it(std::string("should identify shared knowledge correctly"), [=]() mutable
            {
                auto sharedKnowledge = object{
                    object::pair{std::string("id"), createTestUUID(300)}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Shared knowledge")}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("isShared"), true}
                        }}
                    }}
                };
                auto privateKnowledge = object{
                    object::pair{std::string("id"), createTestUUID(301)}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Private knowledge")}, 
                        object::pair{std::string("shared"), false}
                    }}
                };
                expect(isSharedKnowledge(sharedKnowledge))->toBe(true);
                expect(isSharedKnowledge(privateKnowledge))->toBe(false);
            }
            );
            it(std::string("should create knowledge metadata correctly"), [=]() mutable
            {
                auto metadata = createKnowledgeMetadata(std::string("test-source"), true);
                expect(metadata["type"])->toBe(std::string("knowledge"));
                expect(metadata["source"])->toBe(std::string("test-source"));
                expect(metadata["isShared"])->toBe(true);
                expect(metadata["timestamp"])->toBeDefined();
            }
            );
            it(std::string("should filter knowledge by scope correctly"), [=]() mutable
            {
                auto items = array<std::shared_ptr<RAGKnowledgeItem>>{ object{
                    object::pair{std::string("id"), createTestUUID(400)}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Shared item")}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("isShared"), true}
                        }}
                    }}
                }, object{
                    object::pair{std::string("id"), createTestUUID(401)}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Private item")}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("isShared"), false}
                        }}
                    }}
                } };
                auto sharedOnly = filterKnowledgeByScope(items, true, false);
                auto privateOnly = filterKnowledgeByScope(items, false, true);
                auto all = filterKnowledgeByScope(items, true, true);
                expect(sharedOnly)->toHaveLength(1);
                expect(const_(sharedOnly)[0]->content["text"])->toBe(std::string("Shared item"));
                expect(privateOnly)->toHaveLength(1);
                expect(const_(privateOnly)[0]->content["text"])->toBe(std::string("Private item"));
                expect(all)->toHaveLength(2);
            }
            );
        }
        );
        describe(std::string("Constants and table names"), [=]() mutable
        {
            it(std::string("should define knowledge table names"), [=]() mutable
            {
                expect(KNOWLEDGE_TABLE_NAMES["DOCUMENTS"])->toBe(std::string("documents"));
                expect(KNOWLEDGE_TABLE_NAMES["KNOWLEDGE"])->toBe(std::string("knowledge"));
                expect(KNOWLEDGE_TABLE_NAMES["RAG_KNOWLEDGE"])->toBe(std::string("rag_knowledge"));
                expect(KNOWLEDGE_TABLE_NAMES["MEMORIES"])->toBe(std::string("memories"));
                expect(KNOWLEDGE_TABLE_NAMES["FRAGMENTS"])->toBe(std::string("fragments"));
            }
            );
        }
        );
    }
    );
}

MAIN
