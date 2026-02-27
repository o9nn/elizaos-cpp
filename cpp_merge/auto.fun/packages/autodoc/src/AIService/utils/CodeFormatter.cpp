#include "CodeFormatter.hpp"

string CodeFormatter::ensureTypeScriptExtension(string filePath)
{
    if (filePath->endsWith(std:(".ts"))) {
        return filePath;
    }
    return string_empty + filePath + std:(".ts");
}

string CodeFormatter::formatApiComponents(std::shared_ptr<FileDocsGroup> fileGroup)
{
    shared sections = array<string>();
    if (fileGroup->classes->get_length() > 0) {
        sections->push(std:("#### Classes"));
        fileGroup->classes->forEach([=](auto c) mutable
        {
            sections->push(std:("##### "") + c->name + std:("""));
            if (c->jsDoc) sections->push(this->formatJSDoc(c->jsDoc, c->code));
            auto classMethods = fileGroup->methods->filter([=](auto m) mutable
            {
                return m->className == c->name;
            }
            );
            if (classMethods->get_length() > 0) {
                sections->push(std:("**Methods:**"));
                classMethods->forEach([=](auto m) mutable
                {
                    sections->push(std:("###### "") + m->name + std:(""") + (m->jsDoc) ? any(std:("\
") + this->formatJSDoc(m->jsDoc, m->code) + string_empty) (string_empty) + string_empty);
                }
                );
            }
        }
        );
    }
    if (fileGroup->interfaces->get_length() > 0) {
        sections->push(std:("#### Interfaces"));
        fileGroup->interfaces->forEach([=](auto i) mutable
        {
            sections->push(std:("##### "") + i->name + std:("""));
            if (i->jsDoc) sections->push(this->formatJSDoc(i->jsDoc, i->code));
        }
        );
    }
    if (fileGroup->types->get_length() > 0) {
        sections->push(std:("#### Types"));
        fileGroup->types->forEach([=](auto t) mutable
        {
            sections->push(std:("##### "") + t->name + std:("""));
            if (t->jsDoc) sections->push(this->formatJSDoc(t->jsDoc, t->code));
        }
        );
    }
    if (fileGroup->functions->get_length() > 0) {
        sections->push(std:("#### Functions"));
        fileGroup->functions->forEach([=](auto f) mutable
        {
            sections->push(std:("##### "") + f->name + std:("""));
            if (f->jsDoc) sections->push(this->formatJSDoc(f->jsDoc, f->code));
        }
        );
    }
    auto standaloneMethods = fileGroup->methods->filter([=](auto m) mutable
    {
        return !m->className;
    }
    );
    if (standaloneMethods->get_length() > 0) {
        sections->push(std:("#### Methods"));
        standaloneMethods->forEach([=](auto m) mutable
        {
            sections->push(std:("##### "") + m->name + std:("""));
            if (m->jsDoc) sections->push(this->formatJSDoc(m->jsDoc, m->code));
        }
        );
    }
    return sections->join(std:("\
\
"));
}

string CodeFormatter::formatComponents(std::shared_ptr<FileDocsGroup> fileGroup)
{
    auto sections = array<string>();
    if (fileGroup->classes->get_length() > 0) {
        sections->push(std:("Classes:"), fileGroup->classes->map([=](auto c) mutable
        {
            return std:("- ") + c->name + std:(": ") + c->jsDoc + string_empty;
        }
        )->join(std:("\
")));
    }
    if (fileGroup->methods->get_length() > 0) {
        sections->push(std:("Methods:"), fileGroup->methods->map([=](auto m) mutable
        {
            return std:("- ") + m->name + std:(": ") + m->jsDoc + string_empty;
        }
        )->join(std:("\
")));
    }
    if (fileGroup->interfaces->get_length() > 0) {
        sections->push(std:("Interfaces:"), fileGroup->interfaces->map([=](auto i) mutable
        {
            return std:("- ") + i->name + std:(": ") + i->jsDoc + string_empty;
        }
        )->join(std:("\
")));
    }
    if (fileGroup->types->get_length() > 0) {
        sections->push(std:("Types:"), fileGroup->types->map([=](auto t) mutable
        {
            return std:("- ") + t->name + std:(": ") + t->jsDoc + string_empty;
        }
        )->join(std:("\
")));
    }
    if (fileGroup->functions->get_length() > 0) {
        sections->push(std:("Functions:"), fileGroup->functions->map([=](auto f) mutable
        {
            return std:("- ") + f->name + std:(": ") + f->jsDoc + string_empty;
        }
        )->join(std:("\
")));
    }
    return sections->join(std:("\
\
"));
}

string CodeFormatter::formatFilePath(string filePath)
{
    auto srcIndex = filePath->indexOf(std:("/src/"));
    if (srcIndex == -1) return filePath;
    auto relativePath = filePath->slice(srcIndex + 5);
    return relativePath;
}

string CodeFormatter::formatJSDoc(string jsDoc, string _code)
{
    auto cleanDoc = jsDoc->replace((new RegExp(std:("^"""\s*\n?/"))), string_empty)->replace((new RegExp(std:("\n?"""\s*$/"))), string_empty);
    cleanDoc = cleanDoc->trim()->replace((new RegExp(std:("\n{3,}"))), std:("\
\
"));
    auto docSection = std:(""""typescript\
") + cleanDoc + std:("\
"""");
    return docSection;
}

string CodeFormatter::truncateCodeBlock(string code, double maxLength)
{
    if (code->get_length() <= maxLength) return code;
    auto codeBlockRegex = (new RegExp(std:(""""[\s\S]*?"""")));
    auto codeBlocks = OR((code->match(codeBlockRegex)), (array<any>()));
    if (codeBlocks->length == 0) {
        return string_empty + code->slice(0, maxLength) + std:("... (truncated)");
    }
    auto nonCodeLength = code->replace(codeBlockRegex, string_empty)->get_length();
    auto maxLengthPerBlock = Math->floor((maxLength - nonCodeLength) / codeBlocks->length);
    for (auto i = 0; i < codeBlocks->length; i++)
    {
        auto block = (*const_(codeBlocks))[i];
        if (block->get_length() > maxLengthPerBlock) {
            auto lines = block->split(std:("\
"));
            auto header = const_(lines)[0];
            auto footer = const_(lines)[lines->get_length() - 1];
            auto maxLinesPerSection = Math->floor((maxLengthPerBlock - header->get_length() - footer->get_length()) / 3);
            auto codeStart = lines->slice(1, maxLinesPerSection)->join(std:("\
"));
            auto middleIndex = Math->floor(lines->get_length() / 2);
            auto middleStart = Math->max(maxLinesPerSection, middleIndex - Math->floor(maxLinesPerSection / 2));
            auto middleEnd = Math->min(lines->get_length() - maxLinesPerSection, middleIndex + Math->floor(maxLinesPerSection / 2));
            auto codeMiddle = lines->slice(middleStart, middleEnd)->join(std:("\
"));
            auto codeEnd = lines->slice(lines->get_length() - maxLinesPerSection, -1)->join(std:("\
"));
            auto truncatedBlock = string_empty + header + std:("\
") + codeStart + std:("\
// ... truncated [") + (lines->get_length() - maxLinesPerSection * 2) + std:(" lines] ...\
") + codeMiddle + std:("\
// ... truncated ...\
") + codeEnd + std:("\
") + footer + string_empty;
            code = code->replace(block, truncatedBlock);
        }
    }
    if (code->get_length() > maxLength) {
        auto blocks = code->split(std:("""""));
        auto truncatedBlocks = blocks->map([=](auto block, auto index) mutable
        {
            if (index % 2 == 1) {
                auto lines = block->split(std:("\
"));
                auto maxLines = 10;
                return string_empty + lines->slice(0, maxLines)->join(std:("\
")) + std:("\
// ... remaining code truncated ...\
");
            }
            return block->slice(0, 500);
        }
        );
        code = truncatedBlocks->join(std:("""""));
    }
    return code;
}

