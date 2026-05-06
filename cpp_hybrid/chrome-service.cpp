#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/services/chrome-service.h"

std::function<void(string, std::shared_ptr<ChromeCallback>)> sendChromeMessage = [=](auto msg, auto callback = undefined) mutable
{
    AND((AND((chrome), (chrome->tabs))), (chrome->tabs->query(object{
        object::pair{std::string("active"), true}, 
        object::pair{std::string("currentWindow"), true}
    }, [=](any tabs) mutable
    {
        if (callback) {
            chrome->tabs->sendMessage(const_(tabs)[0]["id"], object{
                object::pair{std::string("message"), msg}
            }, callback);
        } else {
            chrome->tabs->sendMessage(const_(tabs)[0]["id"], object{
                object::pair{std::string("message"), msg}
            });
        }
    }
    )));
};
array<object> defaultSettings = array<object>{ object{
    object::pair{std::string("name"), DiscrubSetting::REACTIONS_ENABLED}, 
    object::pair{std::string("value"), std::string("true")}
}, object{
    object::pair{std::string("name"), DiscrubSetting::SERVER_NICKNAME_LOOKUP}, 
    object::pair{std::string("value"), std::string("true")}
}, object{
    object::pair{std::string("name"), DiscrubSetting::DISPLAY_NAME_LOOKUP}, 
    object::pair{std::string("value"), std::string("true")}
}, object{
    object::pair{std::string("name"), DiscrubSetting::RANDOM_DELETE_DELAY}, 
    object::pair{std::string("value"), std::string("0")}
}, object{
    object::pair{std::string("name"), DiscrubSetting::RANDOM_SEARCH_DELAY}, 
    object::pair{std::string("value"), std::string("0")}
}, object{
    object::pair{std::string("name"), DiscrubSetting::EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS}, 
    object::pair{std::string("value"), std::string("false")}
}, object{
    object::pair{std::string("name"), DiscrubSetting::EXPORT_ARTIST_MODE}, 
    object::pair{std::string("value"), std::string("false")}
}, object{
    object::pair{std::string("name"), DiscrubSetting::EXPORT_MESSAGE_SORT_ORDER}, 
    object::pair{std::string("value"), SortDirection::DESCENDING}
}, object{
    object::pair{std::string("name"), DiscrubSetting::EXPORT_PREVIEW_MEDIA}, 
    object::pair{std::string("value"), string_empty}
}, object{
    object::pair{std::string("name"), DiscrubSetting::EXPORT_DOWNLOAD_MEDIA}, 
    object::pair{std::string("value"), string_empty}
}, object{
    object::pair{std::string("name"), DiscrubSetting::EXPORT_MESSAGES_PER_PAGE}, 
    object::pair{std::string("value"), std::string("1000")}
}, object{
    object::pair{std::string("name"), DiscrubSetting::EXPORT_IMAGE_RES_MODE}, 
    object::pair{std::string("value"), ResolutionType::HOVER_LIMITED}
} };
std::function<std::shared_ptr<Promise<std::shared_ptr<Record<DiscrubSetting, string>>>>()> initializeSettings = [=]() mutable
{
    auto& __array1787_2035 = defaultSettings;
    for (auto __indx1787_2035 = 0_N; __indx1787_2035 < __array1787_2035->get_length(); __indx1787_2035++)
    {
        auto& setting = const_(__array1787_2035)[__indx1787_2035];
        {
            auto foundSetting = std::async([=]() { chrome->storage->local->get(setting["name"]); });
            if (!Object->keys(foundSetting)->get_length()) {
                std::async([=]() { chrome->storage->local->set(object{
                    object::pair{setting["name"], setting["value"]}
                }); });
            }
        }
    }
    return getSettings();
};
std::function<std::shared_ptr<Promise<std::shared_ptr<Record<DiscrubSetting, string>>>>()> getSettings = [=]() mutable
{
    auto chromeSettings = std::async([=]() { chrome->storage->local->get(defaultSettings->map([=](auto setting) mutable
    {
        return setting["name"];
    }
    )); });
    return object{
        object::pair{DiscrubSetting::REACTIONS_ENABLED, const_(chromeSettings)[DiscrubSetting::REACTIONS_ENABLED]}, 
        object::pair{DiscrubSetting::SERVER_NICKNAME_LOOKUP, const_(chromeSettings)[DiscrubSetting::SERVER_NICKNAME_LOOKUP]}, 
        object::pair{DiscrubSetting::DISPLAY_NAME_LOOKUP, const_(chromeSettings)[DiscrubSetting::DISPLAY_NAME_LOOKUP]}, 
        object::pair{DiscrubSetting::RANDOM_DELETE_DELAY, const_(chromeSettings)[DiscrubSetting::RANDOM_DELETE_DELAY]}, 
        object::pair{DiscrubSetting::RANDOM_SEARCH_DELAY, const_(chromeSettings)[DiscrubSetting::RANDOM_SEARCH_DELAY]}, 
        object::pair{DiscrubSetting::EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS, const_(chromeSettings)[DiscrubSetting::EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS]}, 
        object::pair{DiscrubSetting::EXPORT_ARTIST_MODE, const_(chromeSettings)[DiscrubSetting::EXPORT_ARTIST_MODE]}, 
        object::pair{DiscrubSetting::EXPORT_MESSAGE_SORT_ORDER, const_(chromeSettings)[DiscrubSetting::EXPORT_MESSAGE_SORT_ORDER]}, 
        object::pair{DiscrubSetting::EXPORT_PREVIEW_MEDIA, const_(chromeSettings)[DiscrubSetting::EXPORT_PREVIEW_MEDIA]}, 
        object::pair{DiscrubSetting::EXPORT_DOWNLOAD_MEDIA, const_(chromeSettings)[DiscrubSetting::EXPORT_DOWNLOAD_MEDIA]}, 
        object::pair{DiscrubSetting::EXPORT_MESSAGES_PER_PAGE, const_(chromeSettings)[DiscrubSetting::EXPORT_MESSAGES_PER_PAGE]}, 
        object::pair{DiscrubSetting::EXPORT_IMAGE_RES_MODE, const_(chromeSettings)[DiscrubSetting::EXPORT_IMAGE_RES_MODE]}
    };
};
std::function<std::shared_ptr<Promise<std::shared_ptr<Record<DiscrubSetting, string>>>>(string, string)> setSetting = [=](auto name, auto value) mutable
{
    std::async([=]() { chrome->storage->local->set(object{
        object::pair{name, value}
    }); });
    return getSettings();
};

void Main(void)
{
}

MAIN
