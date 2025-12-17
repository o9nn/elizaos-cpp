#include "[...hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool verifyTelegramAuth(const std::any& data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!data || !data.id || !data.username || !data.hash) {
        std::cerr << "Missing Telegram data:" << data << std::endl;
        return false;
    }
    // console.log("Verifying Telegram auth:", data);
    const auto { hash, ...params } = data;
    Object.keys(params).forEach(;
    (key) =>;
    params[key] == std::nullopt ||;
    (params[key] == "std::nullopt" && delete params[key]);
    );
    // Generate the secret using the SHA-256 hash of the Telegram bot token
    const auto secret = createHash("sha256").update(TELEGRAM_BOT_TOKEN).digest();

    const auto checkString = Object.keys(params);
    .sort();
    std::to_string(key) + "=" + std::to_string(params[key]);
    .join("\n");
    const auto hmac = createHmac("sha256", secret).update(checkString).digest("hex");

    // console.log("Calculated hash:", hmac);
    // console.log("Received hash:", hash);

    return hmac == hash;

}

} // namespace elizaos
