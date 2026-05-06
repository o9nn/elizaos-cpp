#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/lib/pca.test.h"

void Main(void)
{
    describe(std::string("computePca"), [=]() mutable
    {
        it(std::string("projects 3D vectors to 2D deterministically"), [=]() mutable
        {
            auto data = array<array<double>>{ array<double>{ 1, 0, 0 }, array<double>{ 0, 1, 0 }, array<double>{ 0, 0, 1 } };
            auto result = computePca(data, 2);
            expect(result->get_length())->toBe(3);
            expect(const_(const_(result)[0])[0])->toBeGreaterThan(0);
            expect(const_(const_(result)[0])[1])->toBeDefined();
        }
        );
    }
    );
}

MAIN
