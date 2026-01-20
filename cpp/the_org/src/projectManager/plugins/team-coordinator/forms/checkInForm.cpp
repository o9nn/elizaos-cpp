#include "checkInForm.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> sendCheckInReportForm(HandlerCallback callback, std::optional<std::vector<std::any>> channels, std::optional<std::any> serverInfo) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "Sending check-in report form to Discord..." << std::endl;
        logger.info('Server context:', {
            serverId: serverInfo.serverId,
            serverName: serverInfo.serverName,
            });
            logger.debug(`Received ${channels?.length || 0} channels for the form`);

            // Log every channel we received for debugging purposes
            if (channels && channels.length > 0) {
                logger.debug('Channels received:');
                for (const auto& channel : channels)
                    logger.debug(`- ${channel.name} (${channel.id}), type: ${channel.type}`);
                }
            }

            // Prepare channel options for dropdown if channels are provided
            const auto channelOptions =;
            channels.map((channel) => ({
                label: channel.name,
                value: channel.id,
                "description: " + "Text channel: #" + channel.name
                })) || [];

                // Log available channels for debugging
                if (channelOptions.length > 0) {
                    logger.debug(`Channel options prepared for form: ${channelOptions.length} options`);
                    } else {
                        std::cout << "No channel options available for the form" << std::endl;
                    }

                    // IMPORTANT: Discord API limits messages to 5 action rows maximum
                    const std::vector<DiscordActionRow> formComponents = [];
                    logger.debug('Building form components...');

                    // Add server info as a hidden field instead of select menu
                    formComponents.push_back({
                        type: 1, // ACTION_ROW
                        components: [
                        {
                            type: 4, // TEXT_INPUT (hidden)
                            custom_id: "server_info",
                            value: JSON.stringify({
                                serverId: serverInfo.serverId,
                                }),
                                style: 2, // HIDDEN
                                },
                                ],
                                });

                                // Add channel selection for sending check-in updates
                                if (channelOptions.length > 0) {
                                    formComponents.push_back({
                                        type: 1, // ACTION_ROW
                                        components: [
                                        {
                                            type: 3, // SELECT_MENU
                                            custom_id: "report_channel",
                                            placeholder: "Select channel to send check-in updates",
                                            options: channelOptions,
                                            required: true,
                                            },
                                            ],
                                            });
                                            logger.debug('Added channel selector for check-in updates');
                                        }

                                        // Add submit and cancel buttons
                                        formComponents.push_back({
                                            type: 1, // ACTION_ROW
                                            components: [
                                            {
                                                type: 2, // BUTTON
                                                style: 1, // PRIMARY
                                                custom_id: "submit_report_channel",
                                                label: "Confirm Channel",
                                                },
                                                {
                                                    type: 2, // BUTTON
                                                    style: 2, // SECONDARY
                                                    custom_id: "cancel_report_setup",
                                                    label: "Cancel",
                                                    },
                                                    ],
                                                    });
                                                    logger.debug('Added submit/cancel buttons');

                                                    // Create the final content object
                                                    const Content content = {;
                                                        "text: " + "Select a channel where check-in updates should be sent when users submit their responses after this you can create a check in schedule:"
                                                        source: "discord",
                                                        components: formComponents,
                                                        };

                                                        try {
                                                            std::cout << "Sending check-in report channel selection form to Discord..." << std::endl;
                                                            logger.debug('Server info being sent:', serverInfo);
                                                            logger.debug('Form components:', JSON.stringify(formComponents, null, 2));
                                                            logger.debug(`Components count: ${formComponents.length}`);

                                                            // Count total action rows to ensure we don't exceed Discord's limit of 5
                                                            if (formComponents.length > 5) {
                                                                logger.error(
                                                                "ERROR: Trying to send " + formComponents.size() + " components, but Discord only allows 5"
                                                                );
                                                                // Trim components to 5 to avoid API error
                                                                content.components = formComponents.slice(0, 5);
                                                                std::cout << "Components trimmed to 5 to avoid Discord API error" << std::endl;
                                                            }

                                                            callback(content, []);
                                                            std::cout << "Successfully sent check-in report form" << std::endl;
                                                            } catch (error: unknown) {
                                                                const auto err = error;
                                                                std::cerr << "Error sending check-in report form: " + err << std::endl;
                                                                std::cerr << "Error stack:" << err.stack << std::endl;
                                                                throw;
                                                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
