#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/lander/src/hooks/useGithubReleases.h"

std::function<object()> useGithubReleases = [=]() mutable
{
    auto [releases] = useState<array<std::shared_ptr<GitHubRelease>>>(array<any>());
    auto [latestRelease, setLatestRelease] = useState<any>(nullptr);
    auto [downloadLinks, setDownloadLinks] = useState<array<std::shared_ptr<DownloadLink>>>(array<any>());
    auto [loading, setLoading] = useState(true);
    auto [error, setError] = useState<any>(nullptr);
    shared formatFileSize = [=](auto bytes) mutable
    {
        auto units = array<string>{ std::string("B"), std::string("KB"), std::string("MB"), std::string("GB") };
        auto size = bytes;
        auto unitIndex = 0;
        while (AND((size >= 1024), (unitIndex < units->get_length() - 1)))
        {
            size /= 1024;
            unitIndex++;
        }
        return string_empty + size->toFixed(1) + std::string(" ") + const_(units)[unitIndex] + string_empty;
    };
    shared parseDownloadLinks = [=](auto assets) mutable
    {
        shared links = array<std::shared_ptr<DownloadLink>>();
        assets->forEach([=](auto asset) mutable
        {
            auto filename = asset->name->toLowerCase();
            if (filename->includes(std::string(".dmg"))) {
                links->push(object{
                    object::pair{std::string("platform"), std::string("macOS")}, 
                    object::pair{std::string("architecture"), (filename->includes(std::string("universal"))) ? any(std::string("universal")) : any((filename->includes(std::string("arm64"))) ? std::string("arm64") : std::string("x64"))}, 
                    object::pair{std::string("filename"), asset->name}, 
                    object::pair{std::string("size"), formatFileSize(asset->size)}, 
                    object::pair{std::string("downloadUrl"), asset->browser_download_url}, 
                    object::pair{std::string("type"), std::string("dmg")}
                });
            } else if (OR((filename->includes(std::string(".exe"))), (filename->includes(std::string(".msi"))))) {
                links->push(object{
                    object::pair{std::string("platform"), std::string("Windows")}, 
                    object::pair{std::string("architecture"), (filename->includes(std::string("arm64"))) ? std::string("arm64") : std::string("x64")}, 
                    object::pair{std::string("filename"), asset->name}, 
                    object::pair{std::string("size"), formatFileSize(asset->size)}, 
                    object::pair{std::string("downloadUrl"), asset->browser_download_url}, 
                    object::pair{std::string("type"), std::string("installer")}
                });
            } else if (filename->includes(std::string(".appimage"))) {
                links->push(object{
                    object::pair{std::string("platform"), std::string("Linux")}, 
                    object::pair{std::string("architecture"), (filename->includes(std::string("arm64"))) ? std::string("arm64") : std::string("x64")}, 
                    object::pair{std::string("filename"), asset->name}, 
                    object::pair{std::string("size"), formatFileSize(asset->size)}, 
                    object::pair{std::string("downloadUrl"), asset->browser_download_url}, 
                    object::pair{std::string("type"), std::string("appimage")}
                });
            } else if (filename->includes(std::string(".deb"))) {
                links->push(object{
                    object::pair{std::string("platform"), std::string("Linux")}, 
                    object::pair{std::string("architecture"), (filename->includes(std::string("arm64"))) ? std::string("arm64") : std::string("x64")}, 
                    object::pair{std::string("filename"), asset->name}, 
                    object::pair{std::string("size"), formatFileSize(asset->size)}, 
                    object::pair{std::string("downloadUrl"), asset->browser_download_url}, 
                    object::pair{std::string("type"), std::string("deb")}
                });
            }
        }
        );
        return links;
    };
    shared fetchReleases = [=]() mutable
    {
        setLoading(true);
        setError(nullptr);
        {
            utils::finally __finally4034_4067([&]() mutable
            {
                setLoading(false);
            });
            try
            {
                auto response = std::async([=]() { fetch(getApiReleasesUrl()); });
                if (!response->ok) {
                    throw any(std::make_shared<Error>(std::string("GitHub API error: ") + response->status + string_empty));
                }
                auto data = std::async([=]() { response->json(); });
                auto latestStable = data->find([=](auto release) mutable
                {
                    return AND((!release->prerelease), (!release->draft));
                }
                );
                if (latestStable) {
                    setLatestRelease(latestStable);
                    auto links = parseDownloadLinks(latestStable->assets);
                    setDownloadLinks(links);
                }
            }
            catch (const any& err)
            {
                setError((is<Error>(err)) ? any(err->message) : any(std::string("Failed to fetch releases")));
            }
        }
    };
    auto refetch = [=]() mutable
    {
        fetchReleases();
    };
    useEffect([=]() mutable
    {
        fetchReleases();
    }
    , array<any>());
    return object{
        object::pair{std::string("releases"), std::string("releases")}, 
        object::pair{std::string("latestRelease"), std::string("latestRelease")}, 
        object::pair{std::string("downloadLinks"), std::string("downloadLinks")}, 
        object::pair{std::string("loading"), std::string("loading")}, 
        object::pair{std::string("error"), std::string("error")}, 
        object::pair{std::string("refetch"), std::string("refetch")}
    };
};

void Main(void)
{
}

MAIN
