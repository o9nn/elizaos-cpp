#include "token.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> validateQueryResults(std::optional<std::any> params, const std::optional<std::vector<Token>>& results, std::optional<{ mainQuerySql: string } // Optional parameter for SQL string> sqlStrings) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { hideImported, status } = params;
        const auto mainSql = sqlStrings.mainQuerySql || "N/A";

        if (!results || results.length == 0) {
            /* ... */ return;
        }
        std::cout << "[Validation] Validating " + std::to_string(results.length) + " results..." << std::endl;

        if (hideImported == 1) {
            const auto importedTokensFound = results.filter((token) => token.imported == 1);
            if (importedTokensFound.length > 0) {
                const auto mints = importedTokensFound.map((t) => t.mint).join(", ");
                const auto errorMsg = "Integrity check failed: Filter hideImported=1 active, but found imported=1. Mints: " + std::to_string(mints) + ". SQL: " + std::to_string(mainSql);
                std::cerr << "[CRITICAL] " + std::to_string(errorMsg) << std::endl;
                throw std::runtime_error(errorMsg);
                } else {
                    std::cout << "[Validation] Passed: hideImported=1 check." << std::endl;
                }
            }
            if (status == "active") {
                const auto nonActiveTokensFound = results.filter(;
                [&](token) { return token.status != "active"; }
                );
                if (nonActiveTokensFound.length > 0) {
                    const auto details = nonActiveTokensFound;
                    std::to_string(t.mint) + "(" + std::to_string(t.status) + ")";
                    .join(", ");
                    const auto errorMsg = "Integrity check failed: Filter status='active' active, but found others. Mints/Statuses: " + std::to_string(details) + ". SQL: " + std::to_string(mainSql);
                    std::cerr << "[CRITICAL] " + std::to_string(errorMsg) << std::endl;
                    throw std::runtime_error(errorMsg);
                    } else {
                        std::cout << "[Validation] Passed: status='active' check." << std::endl;
                    }
                }
                if (status == "locked") {
                    const auto nonLockedTokensFound = results.filter(;
                    [&](token) { return token.status != "locked"; }
                    );
                    if (nonLockedTokensFound.length > 0) {
                        const auto details = nonLockedTokensFound;
                        std::to_string(t.mint) + "(" + std::to_string(t.status) + ")";
                        .join(", ");
                        const auto errorMsg = "Integrity check failed: Filter status='locked' active, but found others. Mints/Statuses: " + std::to_string(details) + ". SQL: " + std::to_string(mainSql);
                        std::cerr << "[CRITICAL] " + std::to_string(errorMsg) << std::endl;
                        throw std::runtime_error(errorMsg);
                        } else {
                            std::cout << "[Validation] Passed: status='locked' check." << std::endl;
                        }
                    }
                    std::cout << "[Validation] All checks passed." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

PgSelect buildTokensBaseQuery(const std::any& db, std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        hideImported,
        status,
        creator,
        search,
        sortBy,
        maxVolume,
        maxHolders,
        } = params;
        // Select specific columns needed eventually (adjust as needed)
        // Selecting all initially, will be refined before sorting
        auto query = db;
        .select({
            id: tokens.id,
            mint: tokens.mint,
            name: tokens.name,
            tokenPriceUSD: tokens.tokenPriceUSD,
            priceChange24h: tokens.priceChange24h,
            volume24h: tokens.volume24h,
            marketCapUSD: tokens.marketCapUSD,
            currentPrice: tokens.currentPrice,
            lastPriceUpdate: tokens.lastPriceUpdate,
            status: tokens.status,
            holderCount: tokens.holderCount,
            tokenSupplyUiAmount: tokens.tokenSupplyUiAmount,
            image: tokens.image,
            createdAt: tokens.createdAt,
            curveProgress: tokens.curveProgress,
            curveLimit: tokens.curveLimit,
            imported: tokens.imported,
            hidden: tokens.hidden,
            featured: tokens.featured,
            hide_from_featured: tokens.hide_from_featured,
            ticker: tokens.ticker,
            verified: tokens.verified,
            });
            .from(tokens);
            .$dynamic();

            const std::variant<(SQL, std::vector<undefined)>> conditions = [];
            const auto partnerLaunches = process.env.PARTNER_LAUNCHES.split(",") || [];
            const auto partnerCreators = process.env.PARTNER_CREATORS.split(",") || [];

            if (hideImported == 1) {
                std::to_string(tokens.imported) + " = 0";
                std::cout << "[Query Build] Adding condition: imported = 0" << std::endl;
            }
            auto specificStatusApplied = false;
            if (status == "active") {
                std::to_string(tokens.status) + " = 'active'";
                std::to_string(tokens.curveProgress) + " >= " + std::to_string(0);
                std::to_string(tokens.tokenSupplyUiAmount) + " = " + std::to_string(1000000000);
                std::cout << "[Query Build] Adding condition: status = 'active'" << std::endl;

                specificStatusApplied = true;
                } else if (status == "locked") {
                    std::to_string(tokens.status) + " = 'locked'";
                    std::cout << "[Query Build] Adding condition: status = 'locked'" << std::endl;
                    specificStatusApplied = true;
                }

                if (sortBy == "verified") {
                    std::to_string(tokens.verified) + " = 1";
                    std::cout << "[Query Build] Adding condition: verified = 1" << std::endl;
                }

                if (!specificStatusApplied) {
                    std::to_string(tokens.status) + " != 'pending'";
                    std::cout << "[Query Build] Adding condition: status != 'pending'" << std::endl;
                }
                "(" + std::to_string(tokens.hidden) + " != 1 OR " + std::to_string(tokens.hidden) + " IS NULL)";
                std::cout << "[Query Build] Adding condition: hidden != 1 OR hidden IS NULL" << std::endl;
                if (creator) {
                    conditions.push(eq(tokens.creator, creator));
                    std::cout << "[Query Build] Adding condition: creator = " + std::to_string(creator) << std::endl;
                }
                if (search) {
                    conditions.push(;
                    or(;
                    std::to_string(tokens.name) + " ILIKE " + std::to_string("%" + search + "%");
                    std::to_string(tokens.ticker) + " ILIKE " + std::to_string("%" + search + "%");
                    std::to_string(tokens.mint) + " ILIKE " + std::to_string("%" + search + "%");
                    );
                    );
                    std::cout << "[Query Build] Adding condition: search LIKE " + std::to_string(search) << std::endl;
                }

                if (partnerLaunches.length > 0 && partnerCreators.length > 0) {
                    conditions.push(;
                    sql`(;
                    NOT (;
                    lower(${tokens.ticker}) ~ ANY(ARRAY[;
                    ${sql.join(;
                    partnerLaunches.map(;
                    std::to_string("^" + t.toLowerCase() + "([^A-Za-z0-9_]|$)");
                    ),
                    ", ";
                )}
                ]);
                AND;
                lower(${tokens.creator}) NOT IN (;
                ${sql.join(;
                std::to_string(c.toLowerCase())
                ", ";
            )}
            );
            );
            )`;
            );
            logger.log(
            "[Query Build] Adding condition: restrict partner launch tickers to partnerCreators (case-insensitive)"
            );
        }

        std::to_string(tokens.ticker) + " NOT LIKE " + std::to_string(sql`${"%✅"}`);

        if (conditions.length > 0) {
            query = query.where(and(...conditions.filter((c): c is SQL => !!c)));
        }
        return query;

}

PgSelect buildTokensCountBaseQuery(const std::any& db, std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto query = db.select({ count: count() }).from(tokens).$dynamic();
    const auto { hideImported, status, creator, search, sortBy } = params;
    const std::variant<(SQL, std::vector<undefined)>> conditions = [];

    if (hideImported == 1) {
        std::to_string(tokens.imported) + " = 0";
    }

    if (sortBy == "verified") {
        std::to_string(tokens.verified) + " = 1";
    }

    auto specificStatusApplied = false;
    if (status == "active") {
        std::to_string(tokens.status) + " = 'active'";
        specificStatusApplied = true;
        } else if (status == "locked") {
            std::to_string(tokens.status) + " = 'locked'";
            specificStatusApplied = true;
        }
        if (!specificStatusApplied) {
            std::to_string(tokens.status) + " != 'pending'";
        }
        "(" + std::to_string(tokens.hidden) + " != 1 OR " + std::to_string(tokens.hidden) + " IS NULL)";
        if (creator) {
            conditions.push(eq(tokens.creator, creator));
        }
        if (search) {
            conditions.push(;
            or(;
            std::to_string(tokens.name) + " ILIKE " + std::to_string("%" + search + "%");
            std::to_string(tokens.ticker) + " ILIKE " + std::to_string("%" + search + "%");
            std::to_string(tokens.mint) + " ILIKE " + std::to_string("%" + search + "%");
            );
            );
            std::cout << "[Count Build] Adding condition: search LIKE " + std::to_string(search) << std::endl;
        }

        if (conditions.length > 0) {
            query = query.where(and(...conditions.filter((c): c is SQL => !!c)));
        }
        return query;

}

std::future<void> processSwapEvent(const std::any& swap, boolean = true shouldEmitGlobal) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Get WebSocket client
            const auto wsClient = getWebSocketClient();

            // Get DB connection to fetch token data and calculate featuredScore
            const auto db = getDB();

            // Get the token data for this swap
            const auto tokenData = db;
            .select();
            .from(tokens);
            .where(eq(tokens.mint, swap.tokenMint));
            .limit(1);

            // Prepare swap data for emission
            const auto enrichedSwap = { ...swap };

            // Add featuredScore if we have token data
            if (tokenData && tokenData.length > 0) {
                // Get max values for normalization
                const auto { maxVolume, maxHolders } = getFeaturedMaxValues(db);

                // Calculate featured score
                const auto featuredScore = calculateFeaturedScore(;
                tokenData[0],
                maxVolume,
                maxHolders;
                );

                // Add token data with featuredScore to the swap
                enrichedSwap.tokenData = {
                    ...tokenData[0],
                    featuredScore,
                    };
                }

                // Emit to token-specific room
                "token-" + std::to_string(swap.tokenMint);

                // Only log in debug mode or for significant events
                // Check for process is not ideal in Cloudflare Workers, use env var instead
                const auto debugWs = process.env.DEBUG_WEBSOCKET == "true";
                if (debugWs) {
                    std::cout << "Emitted swap event for token " + std::to_string(swap.tokenMint) << std::endl;
                }

                // Optionally emit to global room for activity feed
                if (shouldEmitGlobal) {
                    "token-" + std::to_string(swap.tokenMint);

                    if (debugWs) {
                        std::cout << "Emitted swap event to global feed" << std::endl;
                    }
                }

                return;
                } catch (error) {
                    std::cerr << "Error processing swap event:" << error << std::endl;
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> processTokenInfo(PublicKey mintPublicKey, AccountInfo<Buffer> tokenInfo, Connection connection, const std::string& requestor) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Check program ID to verify this is an SPL token
        const auto TOKEN_PROGRAM_ID = new PublicKey(;
        "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA";
        );
        const auto TOKEN_2022_PROGRAM_ID = new PublicKey(;
        "TokenzQdBNbLqP5VEhdkAS6EPFLC1PHnBqCXEpPxuEb";
        );

        const auto isSplToken = tokenInfo.owner.equals(TOKEN_PROGRAM_ID);
        const auto isSPL2022 = tokenInfo.owner.equals(TOKEN_2022_PROGRAM_ID);

        if (!isSplToken && !isSPL2022) {
            throw new Error(
            "Not a valid SPL token. Owner: " + tokenInfo.owner.toString()
            );
        }

        std::cout << "[search-token] Token owner: " + std::to_string(tokenInfo.owner.toString()) << std::endl;
        std::cout << "[search-token] Token is SPL-2022: " + std::to_string(isSPL2022) << std::endl;

        // Get mint info - decimals and authorities
        const auto mintInfo = connection.getParsedAccountInfo(mintPublicKey);
        logger.log(
        "[search-token] Mint info: " + std::to_string(JSON.stringify(mintInfo.value.data))
        );

        // Extract basic token info
        const auto parsedData = (mintInfo.value.data).parsed;
        const auto decimals = parsedData.info.decimals || 9;
        const auto mintAuthority = parsedData.info.mintAuthority || nullptr;

        std::cout << "[search-token] Decimals: " + std::to_string(decimals) << std::endl;
        std::cout << "[search-token] Mint authority: " + std::to_string(mintAuthority) << std::endl;

        // Initialize variables for token data
        auto tokenName = "";
        auto tokenSymbol = "";
        auto uri = "";
        auto imageUrl = "";
        auto description = "";
        std::optional<std::string> updateAuthority = nullptr;
        auto foundMetadata = false;

        // For SPL-2022 tokens, check for token metadata extension first
        if (isSPL2022 && parsedData.info.extensions) {
            std::cout << "[search-token] Checking SPL-2022 extensions for metadata" << std::endl;

            // Find the tokenMetadata extension if it exists
            const auto metadataExt = parsedData.info.extensions.find(;
            [&](ext: any) { return ext.extension == "tokenMetadata"; }
            );

            if (metadataExt && metadataExt.state) {
                logger.log(
                "[search-token] Found tokenMetadata extension: " + std::to_string(JSON.stringify(metadataExt.state))
                );

                // Extract metadata directly from the extension
                tokenName = metadataExt.state.name || "";
                tokenSymbol = metadataExt.state.symbol || "";
                uri = metadataExt.state.uri || "";
                updateAuthority = metadataExt.state.updateAuthority || nullptr;

                logger.log(
                "[search-token] SPL-2022 metadata - Name: " + std::to_string(tokenName) + ", Symbol: " + std::to_string(tokenSymbol)
                );
                std::cout << "[search-token] SPL-2022 metadata - URI: " + std::to_string(uri) << std::endl;
                logger.log(
                "[search-token] SPL-2022 metadata - Update Authority: " + std::to_string(updateAuthority)
                );

                foundMetadata = true;

                // Now fetch additional metadata from the URI if available
                if (uri) {
                    std::cout << "[search-token] Fetching metadata from URI: " + std::to_string(uri) << std::endl;
                    // Use fetch with timeout/error handling
                    try {
                        const auto uriResponse = fetch(uri); // Add timeout here if needed;

                        if (uriResponse.ok) {
                            const auto uriText = uriResponse.text();
                            std::cout << "[search-token] URI response: " + std::to_string(uriText) << std::endl;

                            try {
                                const auto uriData = JSON.parse(uriText);
                                logger.log(
                                "[search-token] Parsed URI data: " + std::to_string(JSON.stringify(uriData))
                                );

                                // Extract image and description if available
                                if (uriData.image) {
                                    imageUrl = uriData.image;
                                    logger.log(
                                    "[search-token] Found image URL in URI: " + std::to_string(imageUrl)
                                    );
                                }

                                if (uriData.description) {
                                    description = uriData.description;
                                    logger.log(
                                    "[search-token] Found description in URI: " + std::to_string(description)
                                    );
                                }
                                } catch (parseError) {
                                    logger.error(
                                    "[search-token] Error parsing URI JSON: " + std::to_string(parseError)
                                    );
                                }
                                } else {
                                    logger.error(
                                    "[search-token] Failed to fetch URI: " + std::to_string(uriResponse.status) + " " + std::to_string(uriResponse.statusText)
                                    );
                                }
                                } catch (fetchError) {
                                    std::cerr << "[search-token] Error fetching URI: " + std::to_string(fetchError) << std::endl;
                                }
                            }
                            } else {
                                logger.log(
                                "[search-token] No tokenMetadata extension found in SPL-2022 token";
                                );
                            }
                        }

                        // Only try to get Metaplex metadata if we didn't find it in SPL-2022 extensions
                        if (!foundMetadata) {
                            // Get metadata PDA
                            const auto METADATA_PROGRAM_ID = new PublicKey(;
                            "metaqbxxUerdq28cj1RbAWkYQm3ybzjb6a8bt518x1s";
                            );
                            const auto [metadataAddress] = PublicKey.findProgramAddressSync(;
                            [;
                            Buffer.from("metadata"),
                            METADATA_PROGRAM_ID.toBuffer(),
                            mintPublicKey.toBuffer(),
                            ],
                            METADATA_PROGRAM_ID;
                            );

                            logger.log(
                            "[search-token] Metadata address: " + std::to_string(metadataAddress.toString())
                            );

                            // Get metadata account data - direct read from chain with no fallbacks
                            const auto metadataAccount = connection.getAccountInfo(metadataAddress);
                            if (!metadataAccount || metadataAccount.data.length == 0) {
                                // For SPL-2022 tokens, we already checked extensions so this is just a warning
                                // For regular SPL tokens, this is an error
                                if (isSPL2022) {
                                    logger.warn(
                                    "[search-token] No Metaplex metadata found for SPL-2022 token: " + std::to_string(mintPublicKey.toString())
                                    );
                                    } else {
                                        logger.error(
                                        "[search-token] No metadata found for token: " + std::to_string(mintPublicKey.toString())
                                        );
                                        throw new Error(
                                        "No metadata found for token: " + std::to_string(mintPublicKey.toString())
                                        );
                                    }
                                    } else {
                                        // We found Metaplex metadata
                                        logger.log(
                                        "[search-token] Metadata account found, data length: " + std::to_string(metadataAccount.data.length) + " bytes"
                                        );
                                        logger.log(
                                        "[search-token] Raw metadata (hex): " + std::to_string(Buffer.from(metadataAccount.data).tostd::to_string("hex"))
                                        );

                                        // Direct metadata extraction
                                        updateAuthority = new PublicKey(;
                                        metadataAccount.data.slice(1, 33);
                                        ).toString();
                                        std::cout << "[search-token] Update authority: " + std::to_string(updateAuthority) << std::endl;

                                        // Calculate offsets for variable-length fields
                                        auto offset = 1 + 32 + 32; // Skip version byte + update authority + mint;

                                        // Extract name length and value
                                        const auto nameLength = metadataAccount.data[offset];
                                        offset += 1;
                                        const auto nameData = metadataAccount.data.slice(offset, offset + nameLength);
                                        tokenName = nameData.tostd::to_string("utf8").replace(/\0/g, "").trim();
                                        logger.log(
                                        "[search-token] Token name: " + std::to_string(tokenName) + " (" + std::to_string(nameLength) + " bytes)"
                                        );
                                        offset += nameLength;

                                        // Extract symbol - needs to account for padding between fields
                                        offset += 3; // Skip padding bytes before length;
                                        const auto symbolLength = metadataAccount.data[offset];
                                        offset += 1;
                                        const auto symbolData = metadataAccount.data.slice(;
                                        offset,
                                        offset + symbolLength;
                                        );
                                        tokenSymbol = symbolData.tostd::to_string("utf8").replace(/\0/g, "").trim();
                                        logger.log(
                                        "[search-token] Token symbol: " + std::to_string(tokenSymbol) + " (" + std::to_string(symbolLength) + " bytes)"
                                        );
                                        offset += symbolLength;

                                        // Extract URI
                                        offset += 3; // Skip padding bytes before length;
                                        const auto uriLength = metadataAccount.data[offset];
                                        offset += 1;
                                        const auto uriData = metadataAccount.data.slice(offset, offset + uriLength);
                                        uri = uriData.tostd::to_string("utf8").replace(/\0/g, "").trim();
                                        std::cout << "[search-token] Metadata URI: " + std::to_string(uri) + " (" + std::to_string(uriLength) + " bytes)" << std::endl;

                                        foundMetadata = true;

                                        // Now fetch additional metadata from the URI if available
                                        if (uri) {
                                            std::cout << "[search-token] Fetching metadata from URI: " + std::to_string(uri) << std::endl;
                                            try {
                                                const auto uriResponse = fetch(uri); // Add timeout here if needed;

                                                if (uriResponse.ok) {
                                                    const auto uriText = uriResponse.text();
                                                    std::cout << "[search-token] URI response: " + std::to_string(uriText) << std::endl;

                                                    try {
                                                        const auto uriData = JSON.parse(uriText);
                                                        logger.log(
                                                        "[search-token] Parsed URI data: " + std::to_string(JSON.stringify(uriData))
                                                        );

                                                        // Extract image and description if available
                                                        if (uriData.image) {
                                                            imageUrl = uriData.image;
                                                            logger.log(
                                                            "[search-token] Found image URL in URI: " + std::to_string(imageUrl)
                                                            );
                                                        }

                                                        if (uriData.description) {
                                                            description = uriData.description;
                                                            logger.log(
                                                            "[search-token] Found description in URI: " + std::to_string(description)
                                                            );
                                                        }
                                                        } catch (parseError) {
                                                            logger.error(
                                                            "[search-token] Error parsing URI JSON: " + std::to_string(parseError)
                                                            );
                                                        }
                                                        } else {
                                                            logger.error(
                                                            "[search-token] Failed to fetch URI: " + std::to_string(uriResponse.status) + " " + std::to_string(uriResponse.statusText)
                                                            );
                                                        }
                                                        } catch (fetchError) {
                                                            std::cerr << "[search-token] Error fetching URI: " + std::to_string(fetchError) << std::endl;
                                                        }
                                                    }
                                                }
                                            }

                                            // If we still didn't find metadata from either source, throw error
                                            if (!foundMetadata && !isSPL2022) {
                                                throw std::runtime_error(`No metadata found for token: ${mintPublicKey.toString()}`);
                                            }

                                            // For SPL-2022 tokens, we still consider them valid even without metadata
                                            // since they might not use the tokenMetadata extension

                                            // Check if we're in development mode
                                            const auto isLocalDev = process.env.LOCAL_DEV == "true";

                                            // Determine if requestor is the creator/authority
                                            // In development mode, always allow any token to be imported
                                            const auto isCreator = isLocalDev;
                                            ? true;
                                            : updateAuthority == requestor || mintAuthority == requestor;

                                            std::cout << "[search-token] Is local development mode? " + std::to_string(isLocalDev) << std::endl;
                                            std::cout << "[search-token] LOCAL_DEV value: " + std::to_string(process.env.LOCAL_DEV) << std::endl;
                                            std::cout << "[search-token] Is requestor the creator? " + std::to_string(isCreator) << std::endl;
                                            std::cout << "[search-token] Request wallet: " + std::to_string(requestor) << std::endl;
                                            std::cout << "[search-token] Update authority: " + std::to_string(updateAuthority) << std::endl;
                                            std::cout << "[search-token] Mint authority: " + std::to_string(mintAuthority) << std::endl;

                                            // Debug log for final creator check result
                                            if (isLocalDev) {
                                                logger.log(
                                                "[search-token] Bypassing creator check in development mode. Anyone can import this token.";
                                                );
                                                } else if (isCreator) {
                                                    logger.log(
                                                    "[search-token] Creator check passed - requestor is the token creator.";
                                                    );
                                                    } else {
                                                        logger.log(
                                                        "[search-token] Creator check failed - requestor is not the token creator.";
                                                        );
                                                    }

                                                    // If we don't have names yet (possible for SPL-2022 without tokenMetadata), use defaults
                                                    if (!tokenName) {
                                                        "Token " + std::to_string(mintPublicKey.toString().slice(0, 8));
                                                    }
                                                    if (!tokenSymbol) {
                                                        tokenSymbol = mintPublicKey.toString().slice(0, 4).toUpperCase();
                                                    }

                                                    // Return the token data
                                                    const auto tokenData = {;
                                                        name: tokenName,
                                                        symbol: tokenSymbol,
                                                        "Token " + std::to_string(tokenName) + " (" + std::to_string(tokenSymbol) + ")"
                                                        mint: mintPublicKey.toString(),
                                                        updateAuthority: updateAuthority,
                                                        mintAuthority: mintAuthority || nullptr,
                                                        creator: updateAuthority || mintAuthority || nullptr,
                                                        isCreator: isCreator,
                                                        metadataUri: uri,
                                                        image: imageUrl,
                                                        tokenType: isSPL2022 ? "spl-2022" : "spl-token", // Existing field
                                                        isToken2022: isSPL2022, // Add the boolean flag directly
                                                        decimals: decimals,
                                                        needsWalletSwitch: !isCreator,
                                                        };

                                                        std::cout << "[search-token] Final token data: " + std::to_string(JSON.stringify(tokenData)) << std::endl;

                                                        return tokenData;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<Response> checkBlockchainTokenBalance(const std::any& c, const std::string& // Use 'any' type for context or define a specific type
  mint, const std::string& address, auto checkMultipleNetworks = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Return type should be Response for Hono
    // Initialize return data
    auto balance = 0;
    auto foundNetwork = ""; // Renamed to avoid confusion with loop variable;
    // Get explicit mainnet and devnet URLs
    const auto mainnetUrl = getMainnetRpcUrl();
    const auto devnetUrl = getDevnetRpcUrl();

    // Log detailed connection info and environment settings
    std::cout << "IMPORTANT DEBUG INFO FOR TOKEN BALANCE CHECK:" << std::endl;
    std::cout << "Address: " + std::to_string(address) << std::endl;
    std::cout << "Mint: " + std::to_string(mint) << std::endl;
    std::cout << "CheckMultipleNetworks: " + std::to_string(checkMultipleNetworks) << std::endl;
    std::cout << "LOCAL_DEV setting: " + std::to_string(process.env.LOCAL_DEV) << std::endl;
    std::cout << "ENV.NETWORK setting: " + std::to_string(process.env.NETWORK || "not set") << std::endl;
    std::cout << "Mainnet URL: " + std::to_string(mainnetUrl) << std::endl;
    std::cout << "Devnet URL: " + std::to_string(devnetUrl) << std::endl;

    // Determine which networks to check - ONLY mainnet and devnet if in local mode
    const auto networksToCheck = checkMultipleNetworks;
    ? [;
    { name: "mainnet", url: mainnetUrl },
    { name: "devnet", url: devnetUrl },
    ];
    : [
    {
        name: process.env.NETWORK || "devnet",
        url: process.env.NETWORK == "mainnet" ? mainnetUrl : devnetUrl,
        },
        ];

        logger.log(
        "Will check these networks: " + std::to_string(networksToCheck.map((n) => `${n.name} (${n.url})`).join(", "))
        );

        // Try each network until we find a balance
        for (const auto& network : networksToCheck)
            try {
                logger.log(
                "Checking " + std::to_string(network.name) + " (" + std::to_string(network.url) + ") for token balance...";
                );
                const auto connection = new Connection(network.url, "confirmed");

                // Convert string addresses to PublicKey objects
                const auto mintPublicKey = new PublicKey(mint);
                const auto userPublicKey = new PublicKey(address);

                logger.log(
                "Getting token accounts for " + std::to_string(address) + " for mint " + std::to_string(mint) + " on " + std::to_string(network.name);
                );

                // Fetch token accounts with a simple RPC call
                const auto response = connection.getTokenAccountsByOwner(;
                userPublicKey,
                { mint: mintPublicKey },
            { commitment = "confirmed" }
            );

            // Log the number of accounts found
            logger.log(
            "Found " + std::to_string(response.value.length) + " token accounts on " + std::to_string(network.name);
            );

            // If we have accounts, calculate total balance
            if (response && response.value && response.value.length > 0) {
                auto networkBalance = 0;

                // Log each account
                for (int i = 0; i < response.value.length; i++) {
                    const auto { pubkey } = response.value[i];
                    std::cout << "Account " + std::to_string(i + 1) + ": " + std::to_string(pubkey.toString()) << std::endl;
                }

                // Get token balances from all accounts
                for (const int { pubkey } of response.value) {
                    try {
                        const auto accountInfo = connection.getTokenAccountBalance(pubkey);
                        if (accountInfo.value) {
                            const auto amount = accountInfo.value.amount;
                            const auto decimals = accountInfo.value.decimals;
                            const auto tokenAmount = Number(amount) / Math.pow(10, decimals);
                            networkBalance += tokenAmount;
                            logger.log(
                            "Account " + std::to_string(pubkey.toString()) + " has " + std::to_string(tokenAmount) + " tokens";
                            );
                        }
                        } catch (balanceError) {
                            logger.error(
                            "Error getting token account balance: " + std::to_string(balanceError)
                            );
                            // Continue with other accounts
                        }
                    }

                    // If we found tokens on this network, use this balance
                    if (networkBalance > 0) {
                        balance = networkBalance;
                        foundNetwork = network.name;
                        logger.log(
                        "SUCCESS: Found balance of " + std::to_string(balance) + " tokens on " + std::to_string(foundNetwork)
                        );
                        break; // Stop checking other networks once we find a balance;
                        } else {
                            logger.log(
                            "No balance found on " + std::to_string(network.name) + " despite finding accounts";
                            );
                        }
                        } else {
                            std::cout << "No token accounts found on " + std::to_string(network.name) << std::endl;
                        }
                        } catch (netError) {
                            logger.error(
                            "Error checking " + std::to_string(network.name) + " for token balance: " + std::to_string(netError)
                            );
                        }
                    }

                    logger.log(
                    "Final result: Balance=" + std::to_string(balance) + ", Network=" + std::to_string(foundNetwork || "none")
                    );
                    return c.json({;
                        balance,
                        percentage: 0, // We don't know the percentage when checking directly
                        isCreator: false, // We don't know if creator when checking directly
                        mint,
                        address,
                        network: foundNetwork || process.env.NETWORK || "unknown",
                        onChain: true,
                        });

}

std::future<void> uploadImportImage(Context c) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Require authentication
        const auto user = c.get("user");
        if (!user) {
            return c.json({ error: "Authentication required" }, 401);
        }

        const auto { imageBase64 } = c.req.json();
        // Removed env usage
        // const env = process.env;

        if (!imageBase64) {
            return c.json({ error: "No image data provided" }, 400);
        }

        // Extract content type and base64 data from data URL
        const auto imageMatch = imageBase64.match(/^data:(image\/[a-z+]+);base64,(.*)$/);
        if (!imageMatch) {
            return c.json({ error: "Invalid image data URI format" }, 400);
        }

        const auto contentType = imageMatch[1];
        const auto base64Data = imageMatch[2];
        const auto imageBuffer = Buffer.from(base64Data, "base64");

        // Determine file extension
        auto extension = ".jpg";
        if (contentType.includes("png")) extension = ".png";
        else if (contentType.includes("gif")) extension = ".gif";
        else if (contentType.includes("svg")) extension = ".svg";
        else if (contentType.includes("webp")) extension = ".webp";

        // Generate unique filename
        const auto imageFilename = std::to_string(crypto.randomUUID()) + std::to_string(extension);
        // No need for imageKey construction here if using uploadWithS3
        // const imageKey = `token-images/${imageFilename}`;

        // Upload using the uploader function
        const auto imageUrl = uploadWithS3(;
        // Env no longer needed here
        imageBuffer,
    { filename = imageFilename, contentType, basePath = "token-images" }
    );

    return c.json({ success: true, imageUrl });
    } catch (error) {
        std::cerr << "Error uploading import image:" << error << std::endl;
        return c.json({ error: "Failed to upload image" }, 500);
    }

}

std::future<std::vector<Token>> prioritizeFeaturedTokens(const std::vector<Token>& serializableResults, const std::vector<std::string>& priorityTokenAddresses, double limit) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto db = getDB();
            // Check if priority tokens exist in results
            const auto priorityTokensInResults = serializableResults.filter((token) =>;
            priorityTokenAddresses.includes(token.mint);
            );

            // Get missing priority tokens (if any)
            const auto missingTokenAddresses = priorityTokenAddresses.filter(;
            [&](addr) { return !priorityTokensInResults.some((token) => token.mint == addr); }
            );

            // If we have missing priority tokens, fetch them
            std::vector<Token> missingTokens = [];
            if (missingTokenAddresses.length > 0) {
                try {
                    const auto priorityTokensQuery = db;
                    .select();
                    .from(tokens);
                    .where(inArray(tokens.mint, missingTokenAddresses));

                    const auto fetchedTokens = priorityTokensQuery.execute();

                    // Convert BigInts in fetched tokens
                    missingTokens = fetchedTokens.map((token) => {
                        const std::unordered_map<std::string, std::any> serializableToken = {};
                        for (const int [key, value] of Object.entries(token)) {
                            if (typeof value == "bigint") {
                                serializableToken[key] = (value).toString();
                                } else {
                                    serializableToken[key] = value;
                                }
                            }
                            return serializableToken;
                            });

                            logger.log(
                            "[Featured Sort] Fetched " + std::to_string(missingTokens.length) + " additional priority tokens";
                            );
                            } catch (error) {
                                std::cerr << "[Featured Sort] Error fetching priority tokens:" << error << std::endl;
                                // Don't throw error, just continue with what we have
                            }
                        }

                        // Remove any priority tokens from the main result to avoid duplicates
                        const auto otherTokens = serializableResults.filter(;
                        [&](token) { return !priorityTokenAddresses.includes(token.mint); }
                        );

                        // Combine all priority tokens in the specified order, but only include tokens that exist
                        const auto allPriorityTokens = [];
                        for (const auto& addr : priorityTokenAddresses)
                            const auto existingToken = priorityTokensInResults.find(;
                            [&](t) { return t.mint == addr; }
                            );
                            if (existingToken) {
                                allPriorityTokens.push(existingToken);
                                } else {
                                    const auto fetchedToken = missingTokens.find((t) => t.mint == addr);
                                    if (fetchedToken) allPriorityTokens.push(fetchedToken);
                                    // If token doesn't exist in DB, we just skip it - no need to add null/undefined to array
                                }
                            }

                            // Combine priority tokens with other tokens, ensuring we don't exceed the limit
                            const auto combinedResults = [...allPriorityTokens, ...otherTokens];
                            const auto finalResults = combinedResults.slice(0, limit); // Keep within limit;

                            logger.log(
                            "[Featured Sort] Prioritized " + std::to_string(allPriorityTokens.length) + " tokens at the beginning";
                            );

                            return finalResults;
                            } catch (error) {
                                // If anything fails, return the original results to avoid breaking the API
                                std::cerr << "[Featured Sort] Error in prioritization process:" << error << std::endl;
                                return serializableResults.slice(0, limit);
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
