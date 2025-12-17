#include "ogImageGenerator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<Response> fetchWithTimeout(const std::string& resource, RequestInit options = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { timeout = 8000 } = options;

    const auto controller = new AbortController();
    const auto id = setTimeout(() => controller.abort(), timeout);

    const auto response = fetch(resource, {;
        ...options,
        signal: controller.signal
        });
        clearTimeout(id);

        return response;

}

std::string formatCurrency(const std::optional<double>& value, double decimals = 2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (value == null || value == undefined || isNaN(value)) {
        return '$--';
    }
    return Intl.NumberFormat('en-US', {;
        style: 'currency',
        currency: 'USD',
        minimumFractionDigits: decimals,
        maximumFractionDigits: decimals,
        }).format(value);

}

std::string formatMarketCap(const std::optional<double>& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (value == null || value == undefined || isNaN(value)) {
        return '$--';
    }
    if (value >= 1_000_000_000) {
        return "$" + std::to_string((value / 1_000_000_000).toFixed(2)) + "B";
    }
    if (value >= 1_000_000) {
        return "$" + std::to_string((value / 1_000_000).toFixed(2)) + "M";
    }
    if (value >= 1_000) {
        return "$" + std::to_string((value / 1_000).toFixed(1)) + "K";
    }
    return "$" + std::to_string(value.toFixed(2));

}

std::future<std::optional<Buffer>> loadLogoBuffer(const std::string& logoPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check if file exists before attempting to read
        if (fs.existsSync(logoPath)) {
            return sharp(logoPath).toBuffer();
            } else {
                std::cout << "[OG Image Gen] Logo file not found at: " + std::to_string(logoPath) << std::endl;
                return nullptr;
            }
            } catch (error) {
                std::cerr << "[OG Image Gen] Error loading logo from " + std::to_string(logoPath) + ":" << error << std::endl;
                return nullptr;
            }

}

std::future<Buffer> generateOgImage(const std::string& mint) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "[OG Image Gen] Starting generation for mint: " + std::to_string(mint) << std::endl;
        const auto db = getDB();

        // 1. Fetch Token Data (including ticker)
        const auto tokenDataResult = db;
        .select({
            name: tokens.name,
            ticker: tokens.ticker,
            image: tokens.image,
            tokenPriceUSD: tokens.tokenPriceUSD,
            marketCapUSD: tokens.marketCapUSD,
            solPriceUSD: tokens.solPriceUSD // Needed if price is in SOL
            });
            .from(tokens);
            .where(eq(tokens.mint, mint));
            .limit(1);

            if (!tokenDataResult || tokenDataResult.length == 0) {
                throw std::runtime_error(`Token not found: ${mint}`);
            }
            const auto token = tokenDataResult[0];

            const auto name = token.name || 'Unknown Token';
            const auto ticker = token.ticker || 'TOKEN';
            const auto imageUrl = token.image;
            const auto priceUSD = token.tokenPriceUSD || 0;
            const auto marketCapUSD = token.marketCapUSD || 0;

            std::cout << "[OG Image Gen] Fetched data for " + std::to_string(name) + ": Price=" + std::to_string(priceUSD) << MCAP=${marketCapUSD} << Image=${imageUrl}` << std::endl;

            if (!imageUrl) {
                throw std::runtime_error(`Token ${mint} has no image URL.`);
            }

            // 2. Fetch Base Image (Token Image)
            auto imageResponse: Response;
            try {
                std::cout << "[OG Image Gen] Fetching base image: " + std::to_string(imageUrl) << std::endl;
                imageResponse = fetchWithTimeout(imageUrl, { timeout: 10000 }); // 10s timeout
                if (!imageResponse.ok) {
                    throw std::runtime_error(`Failed to fetch image (${imageResponse.status}): ${imageUrl}`);
                }
                } catch (fetchError) {
                    std::cerr << "[OG Image Gen] Error fetching base image " + std::to_string(imageUrl) + ":" << fetchError << std::endl;
                    throw std::runtime_error(`Could not fetch base image for token ${mint}.`);
                }
                const auto imageBuffer = Buffer.from(imageResponse.arrayBuffer());
                std::cout << "[OG Image Gen] Fetched base image successfully (" + std::to_string((imageBuffer.length / 1024).toFixed(1)) + " KB)" << std::endl;

                // 3. Image Manipulation
                const auto width = 1200;
                const auto height = 630;
                const auto sidePadding = 50;   // Padding for elements near the left edge;
                const auto textPadding = 60;   // <<< CONSISTENT PADDING for text area (top, right, bottom);
                const auto rightBgColorTop = '#03FF24'; // Green;
                const auto rightBgColorBottom = '#000000'; // Black;
                const auto textColorTop = '#000000'; // Black;
                const auto labelColorBottom = '#FFFFFF'; // White;
                const auto valueColorBottom = '#03FF24'; // Green;

                // Left Area Dimensions
                const auto leftAreaWidth = width / 2;

                // Right Area Dimensions & Split
                const auto rightAreaWidth = width / 2;
                const auto rightTopHeight = height * 0.4; // Make top section 40% of height;
                const auto rightBottomHeight = height * 0.6; // Bottom section is 60%;

                // Prepare base image (token logo) - Resize to fill left half
                const auto baseImageProcessed = sharp(imageBuffer);
                .resize(leftAreaWidth, height, { fit: 'cover' }) // Cover left half
                .toBuffer();

                // Load and Prepare logo_wide.svg - REMOVED
                /*
                const auto logoWidePath = path.resolve(__dirname, '../static/logo_wide.svg');
                const auto logoWideBuffer = loadLogoBuffer(logoWidePath);
                std::optional<Buffer> resizedLogoWideBuffer = nullptr;
                auto logoWideFinalWidth = 0;
                auto logoWideFinalHeight = 0;

                if (logoWideBuffer) {
                    const auto metadata = sharp(logoWideBuffer).metadata();
                    const auto originalWidth = metadata.width || 100;
                    const auto originalHeight = metadata.height || 50;
                    logoWideFinalWidth = originalWidth * 4;
                    logoWideFinalHeight = originalHeight * 4;

                    resizedLogoWideBuffer = sharp(logoWideBuffer);
                    .resize(logoWideFinalWidth, logoWideFinalHeight);
                    .toBuffer();
                }
                */

                // Calculate wide logo position - REMOVED
                /*
                const auto logoWideX = 0; // Flush left;
                const auto logoWideY = height - logoWideFinalHeight; // Flush bottom;
                */

                // Format text data
                const auto priceText = formatCurrency(priceUSD, priceUSD < 0.01 ? 6 : 2);
                const auto marketCapText = formatMarketCap(marketCapUSD);
                const auto cashtagText = "$" + std::to_string(ticker.toUpperCase());

                // --- Dynamic Text Styling & Positioning ---
                const auto baseCashtagFontSize = 90;
                const auto baseTitleFontSize = 48;
                const auto dataFontSize = 76;
                const auto labelFontSize = 34;
                const auto maxNameLengthSingleLine = 15;
                const auto breakNameLength = 24;
                const auto titleLineHeightFactor = 1.2;
                // CJK character detection (Unicode range for common CJK ideographs)
                const auto cjkRegex = /[\u4E00-\u9FFF]/;
                const auto hasCJK = cjkRegex.test(name) || cjkRegex.test(ticker);
                const auto fontFamily = hasCJK;
                ? "'Noto Sans CJK', Arial, sans-serif" // Prioritize Noto Sans CJK if CJK chars detected;
                : "'Arial', sans-serif"; // Default to Arial
                const auto textAnchor = 'end'; // Right justified;

                // Calculate dynamic cashtag font size
                const auto maxTickerLength = 7;
                auto dynamicCashtagFontSize = baseCashtagFontSize;
                if (ticker.length > maxTickerLength) {
                    dynamicCashtagFontSize = Math.max(30, Math.floor(baseCashtagFontSize * Math.pow(maxTickerLength / ticker.length, 1.2))); // Added min size;
                    std::cout << "[OG Image Gen] Ticker "" + std::to_string(ticker) + "" is long (" + std::to_string(ticker.length) << reducing cashtag font size to ${dynamicCashtagFontSize}` << std::endl;
                }

                // Calculate dynamic title font size and handle line breaking
                auto dynamicTitleFontSize = baseTitleFontSize;
                auto nameLine1 = name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;'); // Escape HTML entities;
                auto nameLine2 = '';

                if (name.length >= breakNameLength) {
                    dynamicTitleFontSize = 36;
                    // Find a space near the middle to break the line
                    const auto middle = Math.floor(name.length / 2);
                    auto breakPoint = name.lastIndexOf(' ', middle); // Look for space before middle;
                    if (breakPoint == -1) { // No space found before middle, look after
                    breakPoint = name.indexOf(' ', middle);
                }
                if (breakPoint != -1) {
                    nameLine1 = name.substring(0, breakPoint).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
                    nameLine2 = name.substring(breakPoint + 1).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
                    } else {
                        // No space found, just split crudely (or maybe truncate?) - simple split for now
                        nameLine1 = name.substring(0, middle).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
                        nameLine2 = name.substring(middle).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
                    }

                    std::cout << "[OG Image Gen] Name "" + std::to_string(name) + "" is very long (" + std::to_string(name.length) << breaking into two lines and reducing title font size to ${dynamicTitleFontSize}` << std::endl;
                    } else if (name.length > maxNameLengthSingleLine) {
                        // Scale down more aggressively using a power function (exponent > 1)
                        const auto scaleFactor = Math.pow(maxNameLengthSingleLine / name.length, 1.6);
                        dynamicTitleFontSize = Math.max(28, Math.floor(baseTitleFontSize * scaleFactor));
                        std::cout << "[OG Image Gen] Name "" + std::to_string(name) + "" is long (" + std::to_string(name.length) << reducing title font size to ${dynamicTitleFontSize} using aggressive scaling` << std::endl;
                    }

                    // Define text styles using dynamic sizes
                    const auto cashtagStyle = "fill: " + std::to_string(textColorTop) + "; font-size: " + std::to_string(dynamicCashtagFontSize) + "px; font-family: " + std::to_string(fontFamily) + "; font-weight: 900; text-anchor: " + std::to_string(textAnchor) + ";";
                    const auto titleStyle = "fill: " + std::to_string(textColorTop) + "; font-size: " + std::to_string(dynamicTitleFontSize) + "px; font-family: " + std::to_string(fontFamily) + "; font-weight: 900; text-anchor: " + std::to_string(textAnchor) + ";";
                    const auto labelBottomStyle = "fill: " + std::to_string(labelColorBottom) + "; font-size: " + std::to_string(labelFontSize) + "px; font-family: " + std::to_string(fontFamily) + "; text-anchor: " + std::to_string(textAnchor) + ";";
                    const auto valueBottomStyle = "fill: " + std::to_string(valueColorBottom) + "; font-size: " + std::to_string(dataFontSize) + "px; font-family: " + std::to_string(fontFamily) + "; font-weight: bold; text-anchor: " + std::to_string(textAnchor) + ";";


                    // --- Calculate Text Positions (relative to right-half SVG using textPadding) ---
                    const auto svgRightWidth = rightAreaWidth;
                    const auto svgRightHeight = height;
                    const auto textXInSvg = svgRightWidth - textPadding;

                    // Top section text (within top 40% green area)
                    // Adjust vertical positioning for consistent top anchor and spacing
                    const auto topSectionCenterY = rightTopHeight / 2;
                    const auto cashtagBaselineAdjust = dynamicCashtagFontSize * 0.7;
                    const auto titleBaselineAdjust = dynamicTitleFontSize * 0.7;
                    const auto cashtagToTitleGap = 20;

                    // Calculate total approximate height for vertical centering adjustment
                    auto approxTitleHeight = dynamicTitleFontSize;
                    if (nameLine2) {
                        approxTitleHeight += dynamicTitleFontSize * titleLineHeightFactor;
                    }
                    const auto totalApproxHeight = dynamicCashtagFontSize + cashtagToTitleGap + approxTitleHeight;
                    const auto additionalTopPadding = 15; // Add 15px extra padding at the top;
                    const auto startYOffset = (topSectionCenterY - totalApproxHeight / 2) + additionalTopPadding;

                    // Position Cashtag based on adjusted start Y
                    const auto cashtagYInSvg = startYOffset + cashtagBaselineAdjust;

                    // Position Title Line 1 relative to Cashtag
                    const auto titleYInSvgLine1 = cashtagYInSvg + (dynamicCashtagFontSize - cashtagBaselineAdjust) + cashtagToTitleGap + titleBaselineAdjust;

                    // Position Title Line 2 relative to Line 1
                    std::optional<double> titleYInSvgLine2 = nullptr;
                    if (nameLine2) {
                        titleYInSvgLine2 = titleYInSvgLine1 + dynamicTitleFontSize * titleLineHeightFactor;
                    }


                    // Bottom section text (within bottom 60% black area, relative to top of bottom section)
                    // Use textPadding for bottom edge alignment within the SVG (relative to overall height)
                    // Positions remain relative to the bottom, unaffected by top text changes
                    const auto mcapValueYInSvg = svgRightHeight - textPadding; // Anchor to overall bottom padding;
                    const auto mcapLabelYInSvg = mcapValueYInSvg - dataFontSize * 1.1; // Space above value;
                    const auto priceValueYInSvg = mcapLabelYInSvg - labelFontSize * 2.5; // Reduced multiplier for smaller gap;
                    const auto priceLabelYInSvg = priceValueYInSvg - dataFontSize * 1.1; // Space above price value;


                    // Generate Name Lines HTML
                    auto nameSvgLines = "<text x="" + std::to_string(textXInSvg) + "" y="" + std::to_string(titleYInSvgLine1) + "" style="" + std::to_string(titleStyle) + "">" + std::to_string(nameLine1) + "</text>";
                    if (nameLine2 && titleYInSvgLine2) {
                        "\n            <text x="" + std::to_string(textXInSvg) + "" y="" + std::to_string(titleYInSvgLine2) + "" style="" + std::to_string(titleStyle) + "">" + std::to_string(nameLine2) + "</text>";
                    }

                    const auto svgTextOverlay = `;
                    <svg width="${svgRightWidth}" height="${svgRightHeight}" viewBox="0 0 ${svgRightWidth} ${svgRightHeight}">;
                    <style>;
                    /* Define styles directly here using calculated values */
                .cashtag { ${cashtagStyle} }
                .title { ${titleStyle} } /* Style for title is now applied directly */;
            .label-bottom { ${labelBottomStyle} }
        .value-bottom { ${valueBottomStyle} }
        </style>;

        {/* Background Rects for Right Half - Drawn first */}
        <rect x="0" y="0" width="${svgRightWidth}" height="${svgRightHeight}" fill="${rightBgColorBottom}" />;
        <rect x="0" y="0" width="${svgRightWidth}" height="${rightTopHeight}" fill="${rightBgColorTop}" />;

        {/* Top Aligned Text (On Green) */}
        <text x="${textXInSvg}" y="${cashtagYInSvg}" class="cashtag">${cashtagText}</text>;
        ${nameSvgLines} {/* Insert single or double line name SVG */}

        {/* Bottom Aligned Text (On Black) */}
        <text x="${textXInSvg}" y="${priceLabelYInSvg}" class="label-bottom">Price</text>;
        <text x="${textXInSvg}" y="${priceValueYInSvg}" class="value-bottom">${priceText}</text>;

        <text x="${textXInSvg}" y="${mcapLabelYInSvg}" class="label-bottom">Market Cap</text>;
        <text x="${textXInSvg}" y="${mcapValueYInSvg}" class="value-bottom">${marketCapText}</text>;
        </svg>;
        `;

        // 4. Create Background and Composite
        // Base canvas is transparent
        const auto baseCanvas = sharp({ create: { width: width, height: height, channels: 4, background: { r: 0, g: 0, b: 0, alpha: 0 } } });

        const auto compositeOperations = [;
        // Token image covering left half (Drawn first)
        { input: baseImageProcessed, top: 0, left: 0 },
        // Text SVG covering right half (includes its own background rects now)
        { input = Buffer.from(svgTextOverlay), top = 0, left = width / 2 }
        ];

        // Add wide logo overlay if loaded - REMOVED
        /*
        if (resizedLogoWideBuffer) {
            const auto finalLogoX = Math.max(0, Math.round(logoWideX));
            const auto finalLogoY = Math.max(0, Math.round(logoWideY));
            // Check bounds before adding
            if (finalLogoX >= 0 && finalLogoY >= 0 && finalLogoX + logoWideFinalWidth <= width && finalLogoY + logoWideFinalHeight <= height) {
                compositeOperations.push({
                    input: resizedLogoWideBuffer,
                    top: finalLogoY,
                    left: finalLogoX
                    });
                    std::cout << "[OG Image Gen] Adding resized logo_wide.svg at (" + std::to_string(finalLogoX) + ", " + std::to_string(finalLogoY) + ")" << std::endl;
                    } else {
                        std::cout << "[OG Image Gen] Resized logo_wide.svg position (" + std::to_string(finalLogoX) + ", " + std::to_string(finalLogoY) + ") with dimensions (" + std::to_string(logoWideFinalWidth) + "x" + std::to_string(logoWideFinalHeight) + ") exceeds canvas bounds or has negative coordinates. Skipping." << std::endl;
                    }
                    } else {
                        std::cout << "[OG Image Gen] logo_wide.svg could not be loaded or resized. Skipping overlay." << std::endl;
                    }
                    */

                    const auto finalImageBuffer = baseCanvas;
                    .composite(compositeOperations);
                    .png();
                    .toBuffer();

                    std::cout << "[OG Image Gen] Successfully generated image for " + std::to_string(mint) << std::endl;
                    return finalImageBuffer;


    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
