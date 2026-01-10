#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/services/agent-export-import-simple.test.h"

void Main(void)
{
    describe(std::string("Agent Export/Import Services"), [=]() mutable
    {
        it(std::string("should validate manifest structure"), [=]() mutable
        {
            auto manifest = object{
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                object::pair{std::string("agentId"), std::string("test-agent-id")}, 
                object::pair{std::string("agentName"), std::string("Test Agent")}, 
                object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                object::pair{std::string("fileCount"), 0}, 
                object::pair{std::string("elizaVersion"), std::string("1.0.0")}
            };
            expect(manifest["version"])->toBe(std::string("1.0.0"));
            expect(manifest["agentId"])->toBeDefined();
            expect(manifest["agentName"])->toBeDefined();
            expect(manifest["tables"])->toBeInstanceOf(Array);
            expect(manifest["tables"]->get_length())->toBe(9);
        }
        );
        it(std::string("should create valid ZIP structure"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto manifest = object{
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                object::pair{std::string("agentId"), std::string("test-agent-id")}, 
                object::pair{std::string("agentName"), std::string("Test Agent")}, 
                object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                object::pair{std::string("fileCount"), 0}, 
                object::pair{std::string("elizaVersion"), std::string("1.0.0")}
            };
            zip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
            auto tables = array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") };
            for (auto& table : tables)
            {
                zip->addFile(std::string("database/") + table + std::string(".json"), Buffer::from(std::string("[]")));
            }
            auto entries = zip->getEntries();
            auto entryNames = entries->map([=](auto e) mutable
            {
                return e["entryName"];
            }
            );
            expect(entryNames)->toContain(std::string("manifest.json"));
            expect(entryNames->filter([=](auto name) mutable
            {
                return name->startsWith(std::string("database/"));
            }
            )->length)->toBe(9);
            auto manifestEntry = zip->getEntry(std::string("manifest.json"));
            expect(manifestEntry)->toBeDefined();
            auto manifestData = JSON->parse(zip->readAsText(manifestEntry));
            expect(manifestData["version"])->toBe(std::string("1.0.0"));
        }
        );
        it(std::string("should handle large memory arrays in ZIP"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto memories = array<any>();
            for (auto i = 0; i < 1000; i++)
            {
                memories->push(object{
                    object::pair{std::string("id"), std::string("memory-") + i + string_empty}, 
                    object::pair{std::string("agentId"), std::string("test-agent")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Test memory content ") + i + string_empty}
                    }}, 
                    object::pair{std::string("embedding"), ((array(768)))->fill(0.1)}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("index"), i}
                    }}
                });
            }
            zip->addFile(std::string("database/memories.json"), Buffer::from(JSON->stringify(memories, nullptr, 2)));
            auto entry = zip->getEntry(std::string("database/memories.json"));
            auto data = JSON->parse(zip->readAsText(entry));
            expect(data["length"])->toBe(1000);
            expect(const_(data)[0]["embedding"]["length"])->toBe(768);
        }
        );
        it(std::string("should handle file paths correctly"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            zip->addFile(std::string("uploads/file1.jpg"), Buffer::from(std::string("image1")));
            zip->addFile(std::string("uploads/subdir/file2.png"), Buffer::from(std::string("image2")));
            zip->addFile(std::string("uploads/deep/nested/path/file3.pdf"), Buffer::from(std::string("pdf")));
            auto entries = zip->getEntries();
            auto uploadFiles = entries->filter([=](auto e) mutable
            {
                return AND((e["entryName"]->startsWith(std::string("uploads/"))), (!e["isDirectory"]));
            }
            );
            expect(uploadFiles->length)->toBe(3);
            auto file1 = zip->getEntry(std::string("uploads/file1.jpg"));
            expect(file1->getData()->toString())->toBe(std::string("image1"));
        }
        );
        it(std::string("should calculate ZIP size for large files"), [=]() mutable
        {
            auto zip = std::make_shared<AdmZip>();
            auto largeData = Buffer::alloc(10 * 1024 * 1024, std::string("x"));
            zip->addFile(std::string("uploads/large-file.bin"), largeData);
            auto zipBuffer = zip->toBuffer();
            expect(zipBuffer->length)->toBeLessThan(largeData->length);
            expect(zipBuffer->length)->toBeGreaterThan(0);
        }
        );
        it(std::string("should validate import ZIP structure"), [=]() mutable
        {
            auto invalidZip = std::make_shared<AdmZip>();
            invalidZip->addFile(std::string("test.txt"), Buffer::from(std::string("test")));
            auto mockServer = as<any>(object{
                object::pair{std::string("db"), object{
                    object::pair{std::string("transaction"), [=](auto callback) mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Should not process invalid ZIP")));
                    }
                    }
                }}
            });
            auto importService = std::make_shared<AgentImportService>(as<std::shared_ptr<UUID>>(std::string("target-agent-id")), mockServer);
            std::async([=]() { expect(importService->importFromZip(invalidZip->toBuffer()))->rejects->toThrow(std::string("Invalid export: manifest.json not found")); });
            std::async([=]() { importService->cleanup(); });
        }
        );
    }
    );
}

MAIN
