#include "scheduleForm.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> sendCheckInScheduleForm(HandlerCallback callback, std::optional<std::vector<std::any>> channels) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << 'Sending check-in schedule form to Discord...' << std::endl;
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
            "Text channel: #" + std::to_string(channel.name)
            })) || [];

            // Log available channels for debugging
            if (channelOptions.length > 0) {
                logger.debug(`Channel options prepared for form: ${channelOptions.length} options`);
                } else {
                    std::cout << 'No channel options available for the form' << std::endl;
                }

                // IMPORTANT: Discord API limits messages to 5 action rows maximum
                // Create components based on whether we have channels or not
                // We'll use different layouts to stay within the 5-component limit
                const std::vector<DiscordActionRow> formComponents = [];
                logger.debug('Building form components...');

                // Row 1: Always include check-in type selector
                formComponents.push({
                    type: 1, // ACTION_ROW
                    components: [
                    {
                        type: 3, // SELECT_MENU
                        custom_id: 'checkin_type',
                        placeholder: 'Select check-in type',
                        options: [
                        {
                            label: 'Daily Standup',
                            value: 'STANDUP',
                            description: 'Quick daily team sync-up meeting',
                            },
                            {
                                label: 'Sprint Check-in',
                                value: 'SPRINT',
                                description: 'Sprint progress and blockers discussion',
                                },
                                {
                                    label: 'Mental Health Check-in',
                                    value: 'MENTAL_HEALTH',
                                    description: 'Team wellness and support check-in',
                                    },
                                    {
                                        label: 'Project Status Update',
                                        value: 'PROJECT_STATUS',
                                        description: 'Detailed project progress review',
                                        },
                                        {
                                            label: 'Team Retrospective',
                                            value: 'RETRO',
                                            description: 'Team reflection and improvement discussion',
                                            },
                                            ],
                                            },
                                            ],
                                            });
                                            logger.debug('Added check-in type selector (1/5 components)');

                                            // Row 2: Add channel selection if we have channels
                                            if (channelOptions.length > 0) {
                                                formComponents.push({
                                                    type: 1, // ACTION_ROW
                                                    components: [
                                                    {
                                                        type: 3, // SELECT_MENU
                                                        custom_id: 'checkin_channel',
                                                        placeholder: 'Select channel for check-in',
                                                        options: channelOptions,
                                                        },
                                                        ],
                                                        });
                                                        logger.debug('Added channel selector (2/5 components)');
                                                    }

                                                    // Row 3: Always include frequency selector
                                                    formComponents.push({
                                                        type: 1, // ACTION_ROW
                                                        components: [
                                                        {
                                                            type: 3, // SELECT_MENU
                                                            custom_id: 'checkin_frequency',
                                                            placeholder: 'Select check-in frequency',
                                                            options: [
                                                            {
                                                                label: 'Weekdays (Mon-Fri)',
                                                                value: 'WEEKDAYS',
                                                                description: 'Every Monday through Friday',
                                                                },
                                                                {
                                                                    label: 'Daily (All days)',
                                                                    value: 'DAILY',
                                                                    description: 'Every day including weekends',
                                                                    },
                                                                    {
                                                                        label: 'Weekly',
                                                                        value: 'WEEKLY',
                                                                        description: 'Once every week',
                                                                        },
                                                                        {
                                                                            label: 'Bi-weekly',
                                                                            value: 'BIWEEKLY',
                                                                            description: 'Once every two weeks',
                                                                            },
                                                                            {
                                                                                label: 'Monthly',
                                                                                value: 'MONTHLY',
                                                                                description: 'Once every month',
                                                                                },
                                                                                {
                                                                                    label: 'Custom',
                                                                                    value: 'CUSTOM',
                                                                                    description: 'Custom schedule',
                                                                                    },
                                                                                    ],
                                                                                    },
                                                                                    ],
                                                                                    });
                                                                                    logger.debug('Added frequency selector (3/5 components)');

                                                                                    // Row 4: Time selection
                                                                                    formComponents.push({
                                                                                        type: 1, // ACTION_ROW
                                                                                        components: [
                                                                                        {
                                                                                            type: 3, // SELECT_MENU
                                                                                            custom_id: 'checkin_time',
                                                                                            placeholder: 'Select check-in time',
                                                                                            options: [
                                                                                            { label: '6:00 AM', value: '06:00' },
                                                                                            { label: '6:30 AM', value: '06:30' },
                                                                                            { label: '7:00 AM', value: '07:00' },
                                                                                            { label: '7:30 AM', value: '07:30' },
                                                                                            { label: '8:00 AM', value: '08:00' },
                                                                                            { label: '8:30 AM', value: '08:30' },
                                                                                            { label: '9:00 AM', value: '09:00' },
                                                                                            { label: '9:30 AM', value: '09:30' },
                                                                                            { label: '10:00 AM', value: '10:00' },
                                                                                            { label: '10:30 AM', value: '10:30' },
                                                                                            { label: '11:00 AM', value: '11:00' },
                                                                                            { label: '11:30 AM', value: '11:30' },
                                                                                            { label: '12:00 PM', value: '12:00' },
                                                                                            { label: '1:00 PM', value: '13:00' },
                                                                                            { label: '1:30 PM', value: '13:30' },
                                                                                            { label: '2:00 PM', value: '14:00' },
                                                                                            { label: '2:30 PM', value: '14:30' },
                                                                                            { label: '3:00 PM', value: '15:00' },
                                                                                            { label: '3:30 PM', value: '15:30' },
                                                                                            { label: '4:00 PM', value: '16:00' },
                                                                                            { label: '4:30 PM', value: '16:30' },
                                                                                            { label: '5:00 PM', value: '17:00' },
                                                                                            { label: '5:30 PM', value: '17:30' },
                                                                                            { label: '6:00 PM', value: '18:00' },
                                                                                            { label: '6:30 PM', value: '18:30' },
                                                                                            ],
                                                                                            },
                                                                                            ],
                                                                                            });
                                                                                            logger.debug('Added time selector (4/5 components)');

                                                                                            // Row 5: Always add submit and cancel buttons
                                                                                            formComponents.push({
                                                                                                type: 1, // ACTION_ROW
                                                                                                components: [
                                                                                                {
                                                                                                    type: 2, // BUTTON
                                                                                                    style: 1, // PRIMARY
                                                                                                    custom_id: 'submit_checkin_schedule',
                                                                                                    label: 'Create Check-in Schedule',
                                                                                                    },
                                                                                                    {
                                                                                                        type: 2, // BUTTON
                                                                                                        style: 2, // SECONDARY
                                                                                                        custom_id: 'cancel_checkin_schedule',
                                                                                                        label: 'Cancel',
                                                                                                        },
                                                                                                        ],
                                                                                                        });
                                                                                                        logger.debug('Added submit/cancel buttons (5/5 components)');

                                                                                                        // Create the final content object
                                                                                                        const Content content = {;
                                                                                                            text: 'Set up a check-in schedule:',
                                                                                                            source: 'discord',
                                                                                                            components: formComponents,
                                                                                                            };

                                                                                                            try {
                                                                                                                std::cout << 'Sending check-in schedule form to Discord...' << std::endl;
                                                                                                                logger.debug(`Components count: ${formComponents.length}`);

                                                                                                                // Count total action rows to ensure we don't exceed Discord's limit of 5
                                                                                                                if (formComponents.length > 5) {
                                                                                                                    logger.error(
                                                                                                                    "ERROR: Trying to send " + std::to_string(formComponents.length) + " components, but Discord only allows 5"
                                                                                                                    );
                                                                                                                    // Trim components to 5 to avoid API error
                                                                                                                    content.components = formComponents.slice(0, 5);
                                                                                                                    std::cout << 'Components trimmed to 5 to avoid Discord API error' << std::endl;
                                                                                                                }

                                                                                                                callback(content, []);
                                                                                                                std::cout << 'Successfully sent check-in schedule form' << std::endl;
                                                                                                                } catch (error: unknown) {
                                                                                                                    const auto err = error;
                                                                                                                    std::cerr << "Error sending check-in schedule form: " + std::to_string(err) << std::endl;
                                                                                                                    std::cerr << 'Error stack:' << err.stack << std::endl;
                                                                                                                    throw;
                                                                                                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
