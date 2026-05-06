#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/integration/export-import-demo.test.h"

void Main(void)
{
    describe(std::string("Agent Export/Import Demo"), [=]() mutable
    {
        it(std::string("should demonstrate ZIP creation and validation"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto manifest = object{
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                object::pair{std::string("agentId"), std::string("demo-agent-123")}, 
                object::pair{std::string("agentName"), std::string("Demo Agent")}, 
                object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                object::pair{std::string("fileCount"), 3}, 
                object::pair{std::string("elizaVersion"), std::string("1.0.0")}
            };
            zip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
            auto agentData = array<object>{ object{
                object::pair{std::string("id"), std::string("demo-agent-123")}, 
                object::pair{std::string("name"), std::string("Demo Agent")}, 
                object::pair{std::string("enabled"), true}, 
                object::pair{std::string("bio"), array<string>{ std::string("A demonstration agent") }}, 
                object::pair{std::string("settings"), object{
                    object::pair{std::string("theme"), std::string("dark")}, 
                    object::pair{std::string("language"), std::string("en")}
                }}, 
                object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->toISOString()}, 
                object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->toISOString()}
            } };
            zip->addFile(std::string("database/agent.json"), Buffer::from(JSON->stringify(agentData, nullptr, 2)));
            auto memories = array<object>{ object{
                object::pair{std::string("id"), std::string("memory-1")}, 
                object::pair{std::string("agentId"), std::string("demo-agent-123")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Hello, I am a demo agent")}
                }}, 
                object::pair{std::string("embedding"), ((array(768)))->fill(0.1)}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("type"), std::string("greeting")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            }, object{
                object::pair{std::string("id"), std::string("memory-2")}, 
                object::pair{std::string("agentId"), std::string("demo-agent-123")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("I can help with various tasks")}
                }}, 
                object::pair{std::string("embedding"), ((array(768)))->fill(0.2)}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("type"), std::string("capability")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            } };
            zip->addFile(std::string("database/memories.json"), Buffer::from(JSON->stringify(memories, nullptr, 2)));
            auto emptyTables = array<string>{ std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("relationships"), std::string("tasks"), std::string("server_agents") };
            for (auto& table : emptyTables)
            {
                zip->addFile(std::string("database/") + table + std::string(".json"), Buffer::from(std::string("[]")));
            }
            zip->addFile(std::string("uploads/profile.jpg"), Buffer::from(std::string("fake-image-data")));
            zip->addFile(std::string("uploads/documents/guide.pdf"), Buffer::from(std::string("fake-pdf-data")));
            zip->addFile(std::string("uploads/audio/greeting.mp3"), Buffer::from(std::string("fake-audio-data")));
            auto zipBuffer = zip->toBuffer();
            auto verifyZip = std::make_shared<AdmZip>(zipBuffer);
            auto entries = verifyZip->getEntries();
            auto entryNames = entries->map([=](auto e) mutable
            {
                return e["entryName"];
            }
            );
            expect(zipBuffer->length)->toBeGreaterThan(0);
            expect(zipBuffer->length)->toBeLessThan(10 * 1024);
            expect(entryNames)->toContain(std::string("manifest.json"));
            expect(entryNames)->toContain(std::string("database/agent.json"));
            expect(entryNames)->toContain(std::string("database/memories.json"));
            expect(entryNames)->toContain(std::string("uploads/profile.jpg"));
            auto manifestEntry = verifyZip->getEntry(std::string("manifest.json"));
            auto manifestData = JSON->parse(verifyZip->readAsText(manifestEntry));
            expect(manifestData["agentId"])->toBe(std::string("demo-agent-123"));
            expect(manifestData["fileCount"])->toBe(3);
            auto memoriesEntry = verifyZip->getEntry(std::string("database/memories.json"));
            auto memoriesData = JSON->parse(verifyZip->readAsText(memoriesEntry));
            expect(memoriesData["length"])->toBe(2);
            expect(const_(memoriesData)[0]["embedding"]["length"])->toBe(768);
            console->log(std::string("✅ Export ZIP created successfully"));
            console->log(std::string("   - Total size: ") + (zipBuffer->length / 1024)->toFixed(2) + std::string("KB"));
            console->log(std::string("   - Total entries: ") + entries->length + string_empty);
            console->log(std::string("   - Database tables: 9"));
            console->log(std::string("   - Uploaded files: 3"));
        }
        );
        it(std::string("should handle large datasets efficiently"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto largeMemories = array<any>();
            for (auto i = 0; i < 5000; i++)
            {
                largeMemories->push(object{
                    object::pair{std::string("id"), std::string("memory-") + i + string_empty}, 
                    object::pair{std::string("agentId"), std::string("large-agent")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Memory content ") + i + string_empty}
                    }}, 
                    object::pair{std::string("embedding"), ((array(768)))->fill(Math->random())}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("index"), i}
                    }}, 
                    object::pair{std::string("createdAt"), Date->now() - i * 1000}
                });
            }
            auto startTime = Date->now();
            zip->addFile(std::string("database/memories.json"), Buffer::from(JSON->stringify(largeMemories, nullptr, 2)));
            auto compressionTime = Date->now() - startTime;
            auto zipBuffer = zip->toBuffer();
            auto originalSize = JSON->stringify(largeMemories)->get_length();
            auto compressedSize = zipBuffer->length;
            auto compressionRatio = ((1 - compressedSize / originalSize) * 100)->toFixed(2);
            console->log(std::string("✅ Large dataset handling:"));
            console->log(std::string("   - 5000 memories with embeddings"));
            console->log(std::string("   - Original size: ") + (originalSize / 1024 / 1024)->toFixed(2) + std::string("MB"));
            console->log(std::string("   - Compressed size: ") + (compressedSize / 1024 / 1024)->toFixed(2) + std::string("MB"));
            console->log(std::string("   - Compression ratio: ") + compressionRatio + std::string("%"));
            console->log(std::string("   - Compression time: ") + compressionTime + std::string("ms"));
            expect(compressedSize)->toBeLessThan(originalSize);
            expect(compressionTime)->toBeLessThan(5000);
        }
        );
    }
    );
}

MAIN
