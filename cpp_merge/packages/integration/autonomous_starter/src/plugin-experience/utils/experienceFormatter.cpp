#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-experience/utils/experienceFormatter.h"

string formatExperienceForDisplay(std::shared_ptr<Experience> experience)
{
    auto typeEmoji = getTypeEmoji(experience->type);
    auto timestamp = ((std::make_shared<Date>(experience->createdAt)))->toLocaleString();
    return string_empty + typeEmoji + std:(" ") + experience->type::toUpperCase() + std:(" - ") + timestamp + std:("\
Action: ") + experience->action + std:("\
Learning: ") + experience->learning + std:("\
Confidence: ") + Math->round(experience->confidence * 100) + std:("%\
Importance: ") + Math->round(experience->importance * 100) + std:("%\
Domain: ") + experience->domain + std:("\
Tags: ") + experience->tags->join(std:(", ")) + string_empty;
};


string formatExperienceSummary(std::shared_ptr<Experience> experience)
{
    auto typeEmoji = getTypeEmoji(experience->type);
    return string_empty + typeEmoji + std:(" ") + experience->learning + std:(" (") + Math->round(experience->confidence * 100) + std:("% confidence)");
};


string formatExperienceList(array<std::shared_ptr<Experience>> experiences)
{
    if (experiences->get_length() == 0) {
        return std:("No experiences found.");
    }
    return experiences->map([=](auto exp, auto index) mutable
    {
        return string_empty + (index + 1) + std:(". ") + formatExperienceSummary(exp) + string_empty;
    }
    )->join(std:("\
"));
};


string formatPatternSummary(object pattern)
{
    auto significanceEmoji = OR((const_(object{
        object::pair{std:("high"), std:("🔴")}, 
        object::pair{std:("medium"), std:("🟡")}, 
        object::pair{std:("low"), std:("🟢")}
    })[pattern["significance"]]), (std:("⚪")));
    return string_empty + significanceEmoji + std:(" ") + pattern["description"] + std:(" (observed ") + pattern["frequency"] + std:(" times)");
};


std::shared_ptr<Map<string, array<std::shared_ptr<Experience>>>> groupExperiencesByDomain(array<std::shared_ptr<Experience>> experiences)
{
    shared groups = std::make_shared<Map<string, array<std::shared_ptr<Experience>>>>();
    experiences->forEach([=](auto exp) mutable
    {
        auto group = OR((groups->get(exp->domain)), (array<any>()));
        group->push(exp);
        groups->set(exp->domain, group);
    }
    );
    return groups;
};


object getExperienceStats(array<std::shared_ptr<Experience>> experiences)
{
    shared stats = object{
        object::pair{std:("total"), experiences->get_length()}, 
        object::pair{std:("byType"), as<Record<ExperienceType, double>>(object{})}, 
        object::pair{std:("byOutcome"), as<Record<OutcomeType, double>>(object{})}, 
        object::pair{std:("byDomain"), as<Record<string, double>>(object{})}, 
        object::pair{std:("averageConfidence"), 0}, 
        object::pair{std:("averageImportance"), 0}, 
        object::pair{std:("successRate"), 0}
    };
    if (experiences->get_length() == 0) return stats;
    Object->values(ExperienceType)->forEach([=](auto type) mutable
    {
        (*stats["byType"])[type] = experiences->filter([=](auto e) mutable
        {
            return static_cast<long>(e->type) == static_cast<long>(type);
        }
        )->get_length();
    }
    );
    Object->values(OutcomeType)->forEach([=](auto outcome) mutable
    {
        (*stats["byOutcome"])[outcome] = experiences->filter([=](auto e) mutable
        {
            return static_cast<long>(e->outcome) == static_cast<long>(outcome);
        }
        )->get_length();
    }
    );
    auto domains = array<any>{ std::make_shared<Set>(experiences->map([=](auto e) mutable
    {
        return e->domain;
    }
    )) };
    domains->forEach([=](auto domain) mutable
    {
        (*stats["byDomain"])[domain] = experiences->filter([=](auto e) mutable
        {
            return e->domain == domain;
        }
        )->get_length();
    }
    );
    auto totalConfidence = experiences->reduce([=](auto sum, auto exp) mutable
    {
        return sum + exp->confidence;
    }
    , 0);
    stats["averageConfidence"] = totalConfidence / experiences->get_length();
    auto totalImportance = experiences->reduce([=](auto sum, auto exp) mutable
    {
        return sum + exp->importance;
    }
    , 0);
    stats["averageImportance"] = totalImportance / experiences->get_length();
    auto positiveCount = OR(((*const_(stats["byOutcome"]))[OutcomeType::POSITIVE]), (0));
    auto negativeCount = OR(((*const_(stats["byOutcome"]))[OutcomeType::NEGATIVE]), (0));
    auto totalAttempts = positiveCount + negativeCount;
    stats["successRate"] = (totalAttempts > 0) ? any(positiveCount / totalAttempts) (0);
    return stats;
};


string getTypeEmoji(ExperienceType type)
{
    auto emojiMap = object{
        object::pair{ExperienceType::SUCCESS, std:("✅")}, 
        object::pair{ExperienceType::FAILURE, std:("❌")}, 
        object::pair{ExperienceType::DISCOVERY, std:("💡")}, 
        object::pair{ExperienceType::CORRECTION, std:("🔄")}, 
        object::pair{ExperienceType::LEARNING, std:("📚")}, 
        object::pair{ExperienceType::HYPOTHESIS, std:("🤔")}, 
        object::pair{ExperienceType::VALIDATION, std:("✔️")}, 
        object::pair{ExperienceType::WARNING, std:("⚠️")}
    };
    return OR((const_(emojiMap)[type]), (std:("📝")));
};


string formatExperienceForRAG(std::shared_ptr<Experience> experience)
{
    auto parts = array<string>{ std:("Experience Type: ") + experience->type + string_empty, std:("Outcome: ") + experience->outcome + string_empty, std:("Domain: ") + experience->domain + string_empty, std:("Action: ") + experience->action + string_empty, std:("Context: ") + experience->context + string_empty, std:("Result: ") + experience->result + string_empty, std:("Learning: ") + experience->learning + string_empty, std:("Confidence: ") + experience->confidence + string_empty, std:("Importance: ") + experience->importance + string_empty, std:("Tags: ") + experience->tags->join(std:(", ")) + string_empty };
    if (experience->previousBelief) {
        parts->push(std:("Previous Belief: ") + experience->previousBelief + string_empty);
    }
    if (experience->correctedBelief) {
        parts->push(std:("Corrected Belief: ") + experience->correctedBelief + string_empty);
    }
    return parts->join(std:("\
"));
};


array<string> extractKeywords(std::shared_ptr<Experience> experience)
{
    shared keywords = std::make_shared<Set<string>>();
    experience->tags->forEach([=](auto tag) mutable
    {
        return keywords->add(tag->toLowerCase());
    }
    );
    auto learningWords = experience->learning->toLowerCase()->split((new RegExp(std:("\W"))))->filter([=](auto word) mutable
    {
        return word->get_length() > 3;
    }
    );
    learningWords->forEach([=](auto word) mutable
    {
        return keywords->add(word);
    }
    );
    auto actionParts = experience->action->split((new RegExp(std:("[_\-\s]"))))->filter([=](auto part) mutable
    {
        return part->get_length() > 2;
    }
    );
    actionParts->forEach([=](auto part) mutable
    {
        return keywords->add(part->toLowerCase());
    }
    );
    keywords->add(experience->type);
    keywords->add(experience->outcome);
    keywords->add(experience->domain);
    return Array->from(keywords);
};


