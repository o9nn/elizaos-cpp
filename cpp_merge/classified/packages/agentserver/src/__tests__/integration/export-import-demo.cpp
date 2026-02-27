#include "export-import-demo.test.h"

void Main(void)
{
    describe(std:("Agent Export/Import Demo"), [=]() mutable
    {
        it(std:("should demonstrate ZIP creation and validation"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto manifest = object{
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                object::pair{std:("agentId"), std:("demo-agent-123")}, 
                object::pair{std:("agentName"), std:("Demo Agent")}, 
                object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                object::pair{std:("fileCount"), 3}, 
                object::pair{std:("elizaVersion"), std:("1.0.0")}
            };
            zip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
            auto agentData = array<object>{ object{
                object::pair{std:("id"), std:("demo-agent-123")}, 
                object::pair{std:("name"), std:("Demo Agent")}, 
                object::pair{std:("enabled"), true}, 
                object::pair{std:("bio"), array<string>{ std:("A demonstration agent") }}, 
                object::pair{std:("settings"), object{
                    object::pair{std:("theme"), std:("dark")}, 
                    object::pair{std:("language"), std:("en")}
                }}, 
                object::pair{std:("createdAt"), ((std::make_shared<Date>()))->toISOString()}, 
                object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->toISOString()}
            } };
            zip->addFile(std:("database/agent.json"), Buffer::from(JSON->stringify(agentData, nullptr, 2)));
            auto memories = array<object>{ object{
                object::pair{std:("id"), std:("memory-1")}, 
                object::pair{std:("agentId"), std:("demo-agent-123")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Hello, I am a demo agent")}
                }}, 
                object::pair{std:("embedding"), ((array(768)))->fill(0.1)}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("type"), std:("greeting")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            }, object{
                object::pair{std:("id"), std:("memory-2")}, 
                object::pair{std:("agentId"), std:("demo-agent-123")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("I can help with various tasks")}
                }}, 
                object::pair{std:("embedding"), ((array(768)))->fill(0.2)}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("type"), std:("capability")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            } };
            zip->addFile(std:("database/memories.json"), Buffer::from(JSON->stringify(memories, nullptr, 2)));
            auto emptyTables = array<string>{ std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("relationships"), std:("tasks"), std:("server_agents") };
            for (auto& table : emptyTables)
            {
                zip->addFile(std:("database/") + table + std:(".json"), Buffer::from(std:("[]")));
            }
            zip->addFile(std:("uploads/profile.jpg"), Buffer::from(std:("fake-image-data")));
            zip->addFile(std:("uploads/documents/guide.pdf"), Buffer::from(std:("fake-pdf-data")));
            zip->addFile(std:("uploads/audio/greeting.mp3"), Buffer::from(std:("fake-audio-data")));
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
            expect(entryNames)->toContain(std:("manifest.json"));
            expect(entryNames)->toContain(std:("database/agent.json"));
            expect(entryNames)->toContain(std:("database/memories.json"));
            expect(entryNames)->toContain(std:("uploads/profile.jpg"));
            auto manifestEntry = verifyZip->getEntry(std:("manifest.json"));
            auto manifestData = JSON->parse(verifyZip->readAsText(manifestEntry));
            expect(manifestData["agentId"])->toBe(std:("demo-agent-123"));
            expect(manifestData["fileCount"])->toBe(3);
            auto memoriesEntry = verifyZip->getEntry(std:("database/memories.json"));
            auto memoriesData = JSON->parse(verifyZip->readAsText(memoriesEntry));
            expect(memoriesData["length"])->toBe(2);
            expect(const_(memoriesData)[0]["embedding"]["length"])->toBe(768);
            console->log(std:("✅ Export ZIP created successfully"));
            console->log(std:("   - Total size: ") + (zipBuffer->length / 1024)->toFixed(2) + std:("KB"));
            console->log(std:("   - Total entries: ") + entries->length + string_empty);
            console->log(std:("   - Database tables: 9"));
            console->log(std:("   - Uploaded files: 3"));
        }
        );
        it(std:("should handle large datasets efficiently"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto largeMemories = array<any>();
            for (auto i = 0; i < 5000; i++)
            {
                largeMemories->push(object{
                    object::pair{std:("id"), std:("memory-") + i + string_empty}, 
                    object::pair{std:("agentId"), std:("large-agent")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Memory content ") + i + string_empty}
                    }}, 
                    object::pair{std:("embedding"), ((array(768)))->fill(Math->random())}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("index"), i}
                    }}, 
                    object::pair{std:("createdAt"), Date->now() - i * 1000}
                });
            }
            auto startTime = Date->now();
            zip->addFile(std:("database/memories.json"), Buffer::from(JSON->stringify(largeMemories, nullptr, 2)));
            auto compressionTime = Date->now() - startTime;
            auto zipBuffer = zip->toBuffer();
            auto originalSize = JSON->stringify(largeMemories)->get_length();
            auto compressedSize = zipBuffer->length;
            auto compressionRatio = ((1 - compressedSize / originalSize) * 100)->toFixed(2);
            console->log(std:("✅ Large dataset handling:"));
            console->log(std:("   - 5000 memories with embeddings"));
            console->log(std:("   - Original size: ") + (originalSize / 1024 / 1024)->toFixed(2) + std:("MB"));
            console->log(std:("   - Compressed size: ") + (compressedSize / 1024 / 1024)->toFixed(2) + std:("MB"));
            console->log(std:("   - Compression ratio: ") + compressionRatio + std:("%"));
            console->log(std:("   - Compression time: ") + compressionTime + std:("ms"));
            expect(compressedSize)->toBeLessThan(originalSize);
            expect(compressionTime)->toBeLessThan(5000);
        }
        );
    }
    );
}

MAIN
