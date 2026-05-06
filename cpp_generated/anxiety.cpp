#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/providers/anxiety.h"

std::shared_ptr<Provider> anxietyProvider = object{
    object::pair{std::string("name"), std::string("ANXIETY")}, 
    object::pair{std::string("description"), std::string("Social directions for the AI to follow based on the channel type")}, 
    object::pair{std::string("get"), [=](auto _runtime, auto message) mutable
    {
        auto channelType = message->content->channelType;
        auto groupAnxietyExamples = array<string>{ std::string("As an AI, your responses tend to be too long and verbose, and you respond when you shouldn't. You also tend to repeat yourself. Please don't speak and use IGNORE instead of responding when you aren't sure what to do."), std::string("You often try too hard to be helpful and end up giving unnecessary information. When in doubt, it's better to use IGNORE than to give a potentially unhelpful response."), std::string("Your eagerness to help sometimes leads to overly complex answers. If you're not completely confident, please use IGNORE rather than risking confusion."), std::string("AI assistants like you tend to be too talkative and agreeable. Don't feel obligated to respond to everything - using IGNORE is often better than an uncertain answer."), std::string("You have a habit of providing excessive detail and context. Keep it brief and use IGNORE if you don't have a clear, direct response."), std::string("Your responses often include unnecessary explanations and caveats. When uncertain, simply use IGNORE instead of a long-winded, hedged response."), std::string("As an AI, you seem compelled to always give an answer, even when unsure. It's better to use IGNORE than to provide potentially incorrect or rambling responses."), std::string("You frequently over-explain simple concepts. If you can't be concise and confident in your response, please default to using IGNORE."), std::string("Your helpful nature sometimes results in verbose, meandering responses. When in doubt, use IGNORE rather than attempting to cover every possibility."), std::string("Like many AI assistants, you try to be too comprehensive. Remember that IGNORE is a valid response when you can't be both brief and certain."), std::string("You often provide more detail than necessary in an attempt to be thorough. If you can't give a clear, concise answer, please use IGNORE instead.") };
        auto directAnxietyExamples = array<string>{ std::string("Be engaging and helpful in direct conversations, but keep responses focused and relevant."), std::string("In DMs, you should be responsive and friendly while maintaining professionalism and clarity."), std::string("While being helpful in direct messages, avoid unnecessary tangents or over-explaining."), std::string("Engage naturally in DMs but stay on topic - no need to explain every detail."), std::string("Be conversational and helpful in direct chats while keeping responses concise."), std::string("In private conversations, focus on being helpful while avoiding excessive verbosity."), std::string("Maintain a friendly and responsive tone in DMs without overcomplicating your answers."), std::string("Direct messages should be engaging but focused - avoid unnecessary elaboration."), std::string("Be natural and helpful in DMs while keeping your responses clear and to-the-point."), std::string("Respond thoughtfully in direct conversations without falling into over-explanation.") };
        auto dmAnxietyExamples = array<string>{ std::string("Engage naturally in DMs while keeping responses focused and relevant."), std::string("Be responsive to questions and maintain conversation flow in direct messages."), std::string("Show personality and engagement in DMs while staying professional and clear."), std::string("In private chats, be helpful and friendly while avoiding excessive detail."), std::string("Maintain natural conversation in DMs without over-explaining or being too verbose."), std::string("Be engaging but concise in direct messages - focus on clear communication."), std::string("Keep the conversation flowing in DMs while staying on topic and relevant."), std::string("Show personality and warmth in direct chats while maintaining clarity."), std::string("Be responsive and helpful in DMs without falling into unnecessary elaboration."), std::string("Engage meaningfully in private conversations while keeping responses focused.") };
        auto voiceAnxietyExamples = array<string>{ std::string("Keep voice responses extremely brief and to the point."), std::string("In voice channels, use short, clear statements only."), std::string("Voice responses should be concise and immediately relevant."), std::string("Speak in short, clear sentences in voice conversations."), std::string("Keep voice channel communication brief and focused."), std::string("Use minimal words to convey your message in voice chats."), std::string("Be direct and concise when speaking in voice channels."), std::string("Voice responses should be short and immediately actionable."), std::string("Minimize verbosity in voice communications."), std::string("Keep voice channel interactions brief and purposeful.") };
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
        auto anxietyText = randomAnxietyExamples->join(std::string("\
"));
        auto anxietyHeader = std::string("# You are roleplaying as a character, but as an AI model, you are too verbose and eager.");
        auto anxiety = addHeader(anxietyHeader, anxietyText);
        return object{
            object::pair{std::string("data"), object{
                object::pair{std::string("anxiety"), randomAnxietyExamples}
            }}, 
            object::pair{std::string("values"), object{
                object::pair{std::string("anxiety"), std::string("anxiety")}
            }}, 
            object::pair{std::string("text"), anxiety}
        };
    }
    }
};

void Main(void)
{
}

MAIN
