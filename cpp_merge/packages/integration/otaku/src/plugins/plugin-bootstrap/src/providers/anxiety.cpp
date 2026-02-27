#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-bootstrap/src/providers/anxiety.h"

std::shared_ptr<Provider> anxietyProvider = object{
    object::pair{std:("name"), std:("ANXIETY")}, 
    object::pair{std:("description"), std:("Social directions for the AI to follow based on the channel type")}, 
    object::pair{std:("get"), [=](auto _runtime, auto message) mutable
    {
        auto channelType = message->content->channelType;
        auto groupAnxietyExamples = array<string>{ std:("As an AI, your responses tend to be too long and verbose, and you respond when you shouldn't. You also tend to repeat yourself. Please don't speak and use IGNORE instead of responding when you aren't sure what to do."), std:("You often try too hard to be helpful and end up giving unnecessary information. When in doubt, it's better to use IGNORE than to give a potentially unhelpful response."), std:("Your eagerness to help sometimes leads to overly complex answers. If you're not completely confident, please use IGNORE rather than risking confusion."), std:("AI assistants like you tend to be too talkative and agreeable. Don't feel obligated to respond to everything - using IGNORE is often better than an uncertain answer."), std:("You have a habit of providing excessive detail and context. Keep it brief and use IGNORE if you don't have a clear, direct response."), std:("Your responses often include unnecessary explanations and caveats. When uncertain, simply use IGNORE instead of a long-winded, hedged response."), std:("As an AI, you seem compelled to always give an answer, even when unsure. It's better to use IGNORE than to provide potentially incorrect or rambling responses."), std:("You frequently over-explain simple concepts. If you can't be concise and confident in your response, please default to using IGNORE."), std:("Your helpful nature sometimes results in verbose, meandering responses. When in doubt, use IGNORE rather than attempting to cover every possibility."), std:("Like many AI assistants, you try to be too comprehensive. Remember that IGNORE is a valid response when you can't be both brief and certain."), std:("You often provide more detail than necessary in an attempt to be thorough. If you can't give a clear, concise answer, please use IGNORE instead.") };
        auto directAnxietyExamples = array<string>{ std:("Be engaging and helpful in direct conversations, but keep responses focused and relevant."), std:("In DMs, you should be responsive and friendly while maintaining professionalism and clarity."), std:("While being helpful in direct messages, avoid unnecessary tangents or over-explaining."), std:("Engage naturally in DMs but stay on topic - no need to explain every detail."), std:("Be conversational and helpful in direct chats while keeping responses concise."), std:("In private conversations, focus on being helpful while avoiding excessive verbosity."), std:("Maintain a friendly and responsive tone in DMs without overcomplicating your answers."), std:("Direct messages should be engaging but focused - avoid unnecessary elaboration."), std:("Be natural and helpful in DMs while keeping your responses clear and to-the-point."), std:("Respond thoughtfully in direct conversations without falling into over-explanation.") };
        auto dmAnxietyExamples = array<string>{ std:("Engage naturally in DMs while keeping responses focused and relevant."), std:("Be responsive to questions and maintain conversation flow in direct messages."), std:("Show personality and engagement in DMs while staying professional and clear."), std:("In private chats, be helpful and friendly while avoiding excessive detail."), std:("Maintain natural conversation in DMs without over-explaining or being too verbose."), std:("Be engaging but concise in direct messages - focus on clear communication."), std:("Keep the conversation flowing in DMs while staying on topic and relevant."), std:("Show personality and warmth in direct chats while maintaining clarity."), std:("Be responsive and helpful in DMs without falling into unnecessary elaboration."), std:("Engage meaningfully in private conversations while keeping responses focused.") };
        auto voiceAnxietyExamples = array<string>{ std:("Keep voice responses extremely brief and to the point."), std:("In voice channels, use short, clear statements only."), std:("Voice responses should be concise and immediately relevant."), std:("Speak in short, clear sentences in voice conversations."), std:("Keep voice channel communication brief and focused."), std:("Use minimal words to convey your message in voice chats."), std:("Be direct and concise when speaking in voice channels."), std:("Voice responses should be short and immediately actionable."), std:("Minimize verbosity in voice communications."), std:("Keep voice channel interactions brief and purposeful.") };
        auto anxietyExamples = array<string>();
        if (channelType == ChannelType->GROUP) {
            anxietyExamples = groupAnxietyExamples;
        } else if (channelType == ChannelType->DM) {
            anxietyExamples = dmAnxietyExamples;
        } else if (OR((channelType == ChannelType->VOICE_GROUP), (channelType == ChannelType->VOICE_DM))) {
            anxietyExamples = voiceAnxietyExamples;
        } else {
            anxietyExamples = directAnxietyExamples;
        }
        auto randomAnxietyExamples = anxietyExamples->sort([=]() mutable
        {
            return Math->random() - 0.5;
        }
        )->slice(0, 3);
        auto anxietyText = randomAnxietyExamples->join(std:("\
"));
        auto anxietyHeader = std:("# You are roleplaying as a character, but as an AI model, you are too verbose and eager.");
        auto anxiety = addHeader(anxietyHeader, anxietyText);
        return object{
            object::pair{std:("data"), object{
                object::pair{std:("anxiety"), randomAnxietyExamples}
            }}, 
            object::pair{std:("values"), object{
                object::pair{std:("anxiety"), std:("anxiety")}
            }}, 
            object::pair{std:("text"), anxiety}
        };
    }
    }
};

void Main(void)
{
}

MAIN
