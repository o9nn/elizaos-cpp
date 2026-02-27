#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/__tests__/knowledge.test.h"

std::function<any(double)> createTestUUID = [=](auto num) mutable
{
    return std:("00000000-0000-0000-0000-") + num->toString()->padStart(12, std:("0")) + string_empty;
};

void Main(void)
{
    describe(std:("Knowledge adapter"), [=]() mutable
    {
        shared testAgentId = createTestUUID(1);
        shared testRoomId = createTestUUID(2);
        describe(std:("KnowledgeItem to Memory conversion"), [=]() mutable
        {
            it(std:("should convert KnowledgeItem to Memory correctly"), [=]() mutable
            {
                auto knowledgeItem = object{
                    object::pair{std:("id"), createTestUUID(100)}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("This is knowledge about AI")}, 
                        object::pair{std:("source"), std:("documentation")}
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
            it(std:("should convert Memory to KnowledgeItem correctly"), [=]() mutable
            {
                auto memory = object{
                    object::pair{std:("id"), createTestUUID(101)}, 
                    object::pair{std:("entityId"), testAgentId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Memory content")}, 
                        object::pair{std:("action"), std:("remember")}
                    }}, 
                    object::pair{std:("roomId"), testRoomId}, 
                    object::pair{std:("createdAt"), Date->now()}
                };
                auto knowledgeItem = memoryToKnowledgeItem(memory);
                expect(knowledgeItem["id"])->toBe(memory->id);
                expect(knowledgeItem["content"])->toEqual(memory->content);
            }
            );
        }
        );
        describe(std:("RAGKnowledgeItem to Memory conversion"), [=]() mutable
        {
            it(std:("should convert RAGKnowledgeItem to Memory correctly"), [=]() mutable
            {
                auto ragKnowledge = object{
                    object::pair{std:("id"), createTestUUID(200)}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("RAG knowledge content")}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("source"), std:("pdf")}, 
                            object::pair{std:("type"), std:("document")}, 
                            object::pair{std:("isShared"), true}
                        }}
                    }}, 
                    object::pair{std:("embedding"), std::make_shared<Float32Array>(array<double>{ 0.1, 0.2, 0.3 })}, 
                    object::pair{std:("createdAt"), 1234567890}, 
                    object::pair{std:("similarity"), 0.95}
                };
                auto memory = ragKnowledgeToMemory(ragKnowledge, testRoomId);
                expect(memory->id)->toBe(ragKnowledge->id);
                expect(memory->entityId)->toBe(testAgentId);
                expect(memory->agentId)->toBe(testAgentId);
                expect(memory->content->text)->toBe(std:("RAG knowledge content"));
                expect(memory->content->source)->toBe(std:("pdf"));
                expect(memory->content->type)->toBe(std:("document"));
                expect(memory->content->isShared)->toBe(true);
                expect(memory->embedding)->toEqual(array<double>{ 0.1, 0.2, 0.3 });
                expect(memory->roomId)->toBe(testRoomId);
                expect(memory->createdAt)->toBe(1234567890);
                expect(memory->similarity)->toBe(0.95);
            }
            );
            it(std:("should convert Memory to RAGKnowledgeItem correctly"), [=]() mutable
            {
                auto memory = object{
                    object::pair{std:("id"), createTestUUID(201)}, 
                    object::pair{std:("entityId"), testAgentId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Memory text content")}, 
                        object::pair{std:("source"), std:("chat")}, 
                        object::pair{std:("category"), std:("conversation")}
                    }}, 
                    object::pair{std:("embedding"), array<double>{ 0.4, 0.5, 0.6 }}, 
                    object::pair{std:("roomId"), testRoomId}, 
                    object::pair{std:("createdAt"), 9876543210ll}, 
                    object::pair{std:("similarity"), 0.88}
                };
                auto ragKnowledge = memoryToRagKnowledge(memory);
                expect(ragKnowledge->id)->toBe(memory->id);
                expect(ragKnowledge->agentId)->toBe(testAgentId);
                expect(ragKnowledge->content["text"])->toBe(std:("Memory text content"));
                expect(ragKnowledge->content["metadata"]["source"])->toBe(std:("chat"));
                expect(ragKnowledge->content["metadata"]["category"])->toBe(std:("conversation"));
                expect(ragKnowledge->content["metadata"]["type"])->toBe(std:("knowledge"));
                expect(Array->from(ragKnowledge->embedding))->toEqual(array<double>{ 0.4, 0.5, 0.6 });
                expect(ragKnowledge->createdAt)->toBe(9876543210ll);
                expect(ragKnowledge->similarity)->toBe(0.88);
            }
            );
        }
        );
        describe(std:("Knowledge scope utilities"), [=]() mutable
        {
            it(std:("should identify shared knowledge correctly"), [=]() mutable
            {
                auto sharedKnowledge = object{
                    object::pair{std:("id"), createTestUUID(300)}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Shared knowledge")}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("isShared"), true}
                        }}
                    }}
                };
                auto privateKnowledge = object{
                    object::pair{std:("id"), createTestUUID(301)}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Private knowledge")}, 
                        object::pair{std:("shared"), false}
                    }}
                };
                expect(isSharedKnowledge(sharedKnowledge))->toBe(true);
                expect(isSharedKnowledge(privateKnowledge))->toBe(false);
            }
            );
            it(std:("should create knowledge metadata correctly"), [=]() mutable
            {
                auto metadata = createKnowledgeMetadata(std:("test-source"), true);
                expect(metadata["type"])->toBe(std:("knowledge"));
                expect(metadata["source"])->toBe(std:("test-source"));
                expect(metadata["isShared"])->toBe(true);
                expect(metadata["timestamp"])->toBeDefined();
            }
            );
            it(std:("should filter knowledge by scope correctly"), [=]() mutable
            {
                auto items = array<std::shared_ptr<RAGKnowledgeItem>>{ object{
                    object::pair{std:("id"), createTestUUID(400)}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Shared item")}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("isShared"), true}
                        }}
                    }}
                }, object{
                    object::pair{std:("id"), createTestUUID(401)}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Private item")}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("isShared"), false}
                        }}
                    }}
                } };
                auto sharedOnly = filterKnowledgeByScope(items, true, false);
                auto privateOnly = filterKnowledgeByScope(items, false, true);
                auto all = filterKnowledgeByScope(items, true, true);
                expect(sharedOnly)->toHaveLength(1);
                expect(const_(sharedOnly)[0]->content["text"])->toBe(std:("Shared item"));
                expect(privateOnly)->toHaveLength(1);
                expect(const_(privateOnly)[0]->content["text"])->toBe(std:("Private item"));
                expect(all)->toHaveLength(2);
            }
            );
        }
        );
        describe(std:("Constants and table names"), [=]() mutable
        {
            it(std:("should define knowledge table names"), [=]() mutable
            {
                expect(KNOWLEDGE_TABLE_NAMES["DOCUMENTS"])->toBe(std:("documents"));
                expect(KNOWLEDGE_TABLE_NAMES["KNOWLEDGE"])->toBe(std:("knowledge"));
                expect(KNOWLEDGE_TABLE_NAMES["RAG_KNOWLEDGE"])->toBe(std:("rag_knowledge"));
                expect(KNOWLEDGE_TABLE_NAMES["MEMORIES"])->toBe(std:("memories"));
                expect(KNOWLEDGE_TABLE_NAMES["FRAGMENTS"])->toBe(std:("fragments"));
            }
            );
        }
        );
    }
    );
}

MAIN
