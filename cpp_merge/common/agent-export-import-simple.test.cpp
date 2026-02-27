#include "agent-export-import-simple.test.h"

void Main(void)
{
    describe(std:("Agent Export/Import Services"), [=]() mutable
    {
        it(std:("should validate manifest structure"), [=]() mutable
        {
            auto manifest = object{
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                object::pair{std:("agentId"), std:("test-agent-id")}, 
                object::pair{std:("agentName"), std:("Test Agent")}, 
                object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                object::pair{std:("fileCount"), 0}, 
                object::pair{std:("elizaVersion"), std:("1.0.0")}
            };
            expect(manifest["version"])->toBe(std:("1.0.0"));
            expect(manifest["agentId"])->toBeDefined();
            expect(manifest["agentName"])->toBeDefined();
            expect(manifest["tables"])->toBeInstanceOf(Array);
            expect(manifest["tables"]->get_length())->toBe(9);
        }
        );
        it(std:("should create valid ZIP structure"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto manifest = object{
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                object::pair{std:("agentId"), std:("test-agent-id")}, 
                object::pair{std:("agentName"), std:("Test Agent")}, 
                object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                object::pair{std:("fileCount"), 0}, 
                object::pair{std:("elizaVersion"), std:("1.0.0")}
            };
            zip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
            auto tables = array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") };
            for (auto& table : tables)
            {
                zip->addFile(std:("database/") + table + std:(".json"), Buffer::from(std:("[]")));
            }
            auto entries = zip->getEntries();
            auto entryNames = entries->map([=](auto e) mutable
            {
                return e["entryName"];
            }
            );
            expect(entryNames)->toContain(std:("manifest.json"));
            expect(entryNames->filter([=](auto name) mutable
            {
                return name->startsWith(std:("database/"));
            }
            )->length)->toBe(9);
            auto manifestEntry = zip->getEntry(std:("manifest.json"));
            expect(manifestEntry)->toBeDefined();
            auto manifestData = JSON->parse(zip->readAsText(manifestEntry));
            expect(manifestData["version"])->toBe(std:("1.0.0"));
        }
        );
        it(std:("should handle large memory arrays in ZIP"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto memories = array<any>();
            for (auto i = 0; i < 1000; i++)
            {
                memories->push(object{
                    object::pair{std:("id"), std:("memory-") + i + string_empty}, 
                    object::pair{std:("agentId"), std:("test-agent")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Test memory content ") + i + string_empty}
                    }}, 
                    object::pair{std:("embedding"), ((array(768)))->fill(0.1)}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("index"), i}
                    }}
                });
            }
            zip->addFile(std:("database/memories.json"), Buffer::from(JSON->stringify(memories, nullptr, 2)));
            auto entry = zip->getEntry(std:("database/memories.json"));
            auto data = JSON->parse(zip->readAsText(entry));
            expect(data["length"])->toBe(1000);
            expect(const_(data)[0]["embedding"]["length"])->toBe(768);
        }
        );
        it(std:("should handle file paths correctly"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            zip->addFile(std:("uploads/file1.jpg"), Buffer::from(std:("image1")));
            zip->addFile(std:("uploads/subdir/file2.png"), Buffer::from(std:("image2")));
            zip->addFile(std:("uploads/deep/nested/path/file3.pdf"), Buffer::from(std:("pdf")));
            auto entries = zip->getEntries();
            auto uploadFiles = entries->filter([=](auto e) mutable
            {
                return AND((e["entryName"]->startsWith(std:("uploads/"))), (!e["isDirectory"]));
            }
            );
            expect(uploadFiles->length)->toBe(3);
            auto file1 = zip->getEntry(std:("uploads/file1.jpg"));
            expect(file1->getData()->toString())->toBe(std:("image1"));
        }
        );
        it(std:("should calculate ZIP size for large files"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto largeData = Buffer::alloc(10 * 1024 * 1024, std:("x"));
            zip->addFile(std:("uploads/large-file.bin"), largeData);
            auto zipBuffer = zip->toBuffer();
            expect(zipBuffer->length)->toBeLessThan(largeData->length);
            expect(zipBuffer->length)->toBeGreaterThan(0);
        }
        );
        it(std:("should validate import ZIP structure"), [=]() mutable
        {
            auto invalidZip = std::make_shared<AdmZip>();
            invalidZip->addFile(std:("test.txt"), Buffer::from(std:("test")));
            auto mockServer = as<any>(object{
                object::pair{std:("db"), object{
                    object::pair{std:("transaction"), [=](auto callback) mutable
                    {
                        throw any(std::make_shared<Error>(std:("Should not process invalid ZIP")));
                    }
                    }
                }}
            });
            auto importService = std::make_shared<AgentImportService>(as<std::shared_ptr<UUID>>(std:("target-agent-id")), mockServer);
            std::async([=]() { expect(importService->importFromZip(invalidZip->toBuffer()))->rejects->toThrow(std:("Invalid export: manifest.json not found")); });
            std::async([=]() { importService->cleanup(); });
        }
        );
    }
    );
}

MAIN
