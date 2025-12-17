#include "act_reg_start.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void generateRandomString(auto length) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    auto result = "";
    const auto charsLength = chars.size();

    for (int i = 0; i < length; i++) {
        result += chars.charAt(Math.floor(Math.random() * charsLength));
    }

    return result;

}

void extractEmails(auto text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto emailRegex = /[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}/g;
    const auto matches = text.match(emailRegex);
    return matches || [];

}

std::future<void> sendVerifyEmail(auto address, auto regCode) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "sending verify email to" << address << std::endl;
    const auto mailOptions = {;
        from: process.env.SMTP_FROM,
        to: address,
        subject: "Welcome to Spartan Services",
        //text: 'Please click the following link to verify you are the owner of this email and continue registration'
        text: "Please give Spartan the following code: " + regCode
        };
        try {
            const auto info = transporter.sendMail(mailOptions);
            std::cout << "Email sent:" << info.envelope << std::endl;
            } catch (error) {
                std::cerr << "Error sending email:" << error << std::endl;
            }

}

std::future<void> updateSpartanData(auto agentEntityId, auto spartanData) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (spartanDataNew) {
        runtime.createComponent({
            id: uuidv4(),
            agentId: runtime.agentId,
            worldId: roomDetails.worldId,
            roomId: message.roomId,
            sourceEntityId: entityId,
            entityId: agentEntityId,
            type: SPARTAN_SERVICE_TYPE,
            data: spartanData.data,
            });
            } else {
                runtime.updateComponent({
                    id: spartanData.id,
                    // do we need all these fields?
                    //agentId: runtime.agentId,
                    //worldId: roomDetails.worldId,
                    //roomId: message.roomId,
                    //sourceEntityId: entityId,
                    //entityId: entityId,
                    //type: SPARTAN_SERVICE_TYPE,
                    data: spartanData.data,
                    });
                }

}

} // namespace elizaos
