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
        auto units = array<string>{ std:("B"), std:("KB"), std:("MB"), std:("GB") };
        auto size = bytes;
        auto unitIndex = 0;
        while (AND((size >= 1024), (unitIndex < units->get_length() - 1)))
        {
            size /= 1024;
            unitIndex++;
        }
        return string_empty + size->toFixed(1) + std:(" ") + const_(units)[unitIndex] + string_empty;
    };
    shared parseDownloadLinks = [=](auto assets) mutable
    {
        shared links = array<std::shared_ptr<DownloadLink>>();
        assets->forEach([=](auto asset) mutable
        {
            auto filename = asset->name->toLowerCase();
            if (filename->includes(std:(".dmg"))) {
                links->push(object{
                    object::pair{std:("platform"), std:("macOS")}, 
                    object::pair{std:("architecture"), (filename->includes(std:("universal"))) ? any(std:("universal")) ((filename->includes(std:("arm64"))) ? std:("arm64") : std:("x64"))}, 
                    object::pair{std:("filename"), asset->name}, 
                    object::pair{std:("size"), formatFileSize(asset->size)}, 
                    object::pair{std:("downloadUrl"), asset->browser_download_url}, 
                    object::pair{std:("type"), std:("dmg")}
                });
            } else if (OR((filename->includes(std:(".exe"))), (filename->includes(std:(".msi"))))) {
                links->push(object{
                    object::pair{std:("platform"), std:("Windows")}, 
                    object::pair{std:("architecture"), (filename->includes(std:("arm64"))) ? std:("arm64") : std:("x64")}, 
                    object::pair{std:("filename"), asset->name}, 
                    object::pair{std:("size"), formatFileSize(asset->size)}, 
                    object::pair{std:("downloadUrl"), asset->browser_download_url}, 
                    object::pair{std:("type"), std:("installer")}
                });
            } else if (filename->includes(std:(".appimage"))) {
                links->push(object{
                    object::pair{std:("platform"), std:("Linux")}, 
                    object::pair{std:("architecture"), (filename->includes(std:("arm64"))) ? std:("arm64") : std:("x64")}, 
                    object::pair{std:("filename"), asset->name}, 
                    object::pair{std:("size"), formatFileSize(asset->size)}, 
                    object::pair{std:("downloadUrl"), asset->browser_download_url}, 
                    object::pair{std:("type"), std:("appimage")}
                });
            } else if (filename->includes(std:(".deb"))) {
                links->push(object{
                    object::pair{std:("platform"), std:("Linux")}, 
                    object::pair{std:("architecture"), (filename->includes(std:("arm64"))) ? std:("arm64") : std:("x64")}, 
                    object::pair{std:("filename"), asset->name}, 
                    object::pair{std:("size"), formatFileSize(asset->size)}, 
                    object::pair{std:("downloadUrl"), asset->browser_download_url}, 
                    object::pair{std:("type"), std:("deb")}
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
                    throw any(std::make_shared<Error>(std:("GitHub API error: ") + response->status + string_empty));
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
                setError((is<Error>(err)) ? any(err->message) (std:("Failed to fetch releases")));
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
        object::pair{std:("releases"), std:("releases")}, 
        object::pair{std:("latestRelease"), std:("latestRelease")}, 
        object::pair{std:("downloadLinks"), std:("downloadLinks")}, 
        object::pair{std:("loading"), std:("loading")}, 
        object::pair{std:("error"), std:("error")}, 
        object::pair{std:("refetch"), std:("refetch")}
    };
};

void Main(void)
{
}

MAIN
