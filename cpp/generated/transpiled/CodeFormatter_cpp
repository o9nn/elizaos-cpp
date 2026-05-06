#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/autodoc/src/AIService/utils/CodeFormatter.h"

string CodeFormatter::ensureTypeScriptExtension(string filePath)
{
    if (filePath->endsWith(std::string(".ts"))) {
        return filePath;
    }
    return string_empty + filePath + std::string(".ts");
}

string CodeFormatter::formatApiComponents(std::shared_ptr<FileDocsGroup> fileGroup)
{
    shared sections = array<string>();
    if (fileGroup->classes->get_length() > 0) {
        sections->push(std::string("#### Classes"));
        fileGroup->classes->forEach([=](auto c) mutable
        {
            sections->push(std::string("##### `") + c->name + std::string("`"));
            if (c->jsDoc) sections->push(this->formatJSDoc(c->jsDoc, c->code));
            auto classMethods = fileGroup->methods->filter([=](auto m) mutable
            {
                return m->className == c->name;
            }
            );
            if (classMethods->get_length() > 0) {
                sections->push(std::string("**Methods:**"));
                classMethods->forEach([=](auto m) mutable
                {
                    sections->push(std::string("###### `") + m->name + std::string("`") + (m->jsDoc) ? any(std::string("\
") + this->formatJSDoc(m->jsDoc, m->code) + string_empty) : any(string_empty) + string_empty);
                }
                );
            }
        }
        );
    }
    if (fileGroup->interfaces->get_length() > 0) {
        sections->push(std::string("#### Interfaces"));
        fileGroup->interfaces->forEach([=](auto i) mutable
        {
            sections->push(std::string("##### `") + i->name + std::string("`"));
            if (i->jsDoc) sections->push(this->formatJSDoc(i->jsDoc, i->code));
        }
        );
    }
    if (fileGroup->types->get_length() > 0) {
        sections->push(std::string("#### Types"));
        fileGroup->types->forEach([=](auto t) mutable
        {
            sections->push(std::string("##### `") + t->name + std::string("`"));
            if (t->jsDoc) sections->push(this->formatJSDoc(t->jsDoc, t->code));
        }
        );
    }
    if (fileGroup->functions->get_length() > 0) {
        sections->push(std::string("#### Functions"));
        fileGroup->functions->forEach([=](auto f) mutable
        {
            sections->push(std::string("##### `") + f->name + std::string("`"));
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
        sections->push(std::string("#### Methods"));
        standaloneMethods->forEach([=](auto m) mutable
        {
            sections->push(std::string("##### `") + m->name + std::string("`"));
            if (m->jsDoc) sections->push(this->formatJSDoc(m->jsDoc, m->code));
        }
        );
    }
    return sections->join(std::string("\
\
"));
}

string CodeFormatter::formatComponents(std::shared_ptr<FileDocsGroup> fileGroup)
{
    auto sections = array<string>();
    if (fileGroup->classes->get_length() > 0) {
        sections->push(std::string("Classes:"), fileGroup->classes->map([=](auto c) mutable
        {
            return std::string("- ") + c->name + std::string(": ") + c->jsDoc + string_empty;
        }
        )->join(std::string("\
")));
    }
    if (fileGroup->methods->get_length() > 0) {
        sections->push(std::string("Methods:"), fileGroup->methods->map([=](auto m) mutable
        {
            return std::string("- ") + m->name + std::string(": ") + m->jsDoc + string_empty;
        }
        )->join(std::string("\
")));
    }
    if (fileGroup->interfaces->get_length() > 0) {
        sections->push(std::string("Interfaces:"), fileGroup->interfaces->map([=](auto i) mutable
        {
            return std::string("- ") + i->name + std::string(": ") + i->jsDoc + string_empty;
        }
        )->join(std::string("\
")));
    }
    if (fileGroup->types->get_length() > 0) {
        sections->push(std::string("Types:"), fileGroup->types->map([=](auto t) mutable
        {
            return std::string("- ") + t->name + std::string(": ") + t->jsDoc + string_empty;
        }
        )->join(std::string("\
")));
    }
    if (fileGroup->functions->get_length() > 0) {
        sections->push(std::string("Functions:"), fileGroup->functions->map([=](auto f) mutable
        {
            return std::string("- ") + f->name + std::string(": ") + f->jsDoc + string_empty;
        }
        )->join(std::string("\
")));
    }
    return sections->join(std::string("\
\
"));
}

string CodeFormatter::formatFilePath(string filePath)
{
    auto srcIndex = filePath->indexOf(std::string("/src/"));
    if (srcIndex == -1) return filePath;
    auto relativePath = filePath->slice(srcIndex + 5);
    return relativePath;
}

string CodeFormatter::formatJSDoc(string jsDoc, string _code)
{
    auto cleanDoc = jsDoc->replace((new RegExp(std::string("^```\s*\n?/"))), string_empty)->replace((new RegExp(std::string("\n?```\s*$/"))), string_empty);
    cleanDoc = cleanDoc->trim()->replace((new RegExp(std::string("\n{3,}"))), std::string("\
\
"));
    auto docSection = std::string("```typescript\
") + cleanDoc + std::string("\
```");
    return docSection;
}

string CodeFormatter::truncateCodeBlock(string code, double maxLength)
{
    if (code->get_length() <= maxLength) return code;
    auto codeBlockRegex = (new RegExp(std::string("```[\s\S]*?```")));
    auto codeBlocks = OR((code->match(codeBlockRegex)), (array<any>()));
    if (codeBlocks->length == 0) {
        return string_empty + code->slice(0, maxLength) + std::string("... (truncated)");
    }
    auto nonCodeLength = code->replace(codeBlockRegex, string_empty)->get_length();
    auto maxLengthPerBlock = Math->floor((maxLength - nonCodeLength) / codeBlocks->length);
    for (auto i = 0; i < codeBlocks->length; i++)
    {
        auto block = (*const_(codeBlocks))[i];
        if (block->get_length() > maxLengthPerBlock) {
            auto lines = block->split(std::string("\
"));
            auto header = const_(lines)[0];
            auto footer = const_(lines)[lines->get_length() - 1];
            auto maxLinesPerSection = Math->floor((maxLengthPerBlock - header->get_length() - footer->get_length()) / 3);
            auto codeStart = lines->slice(1, maxLinesPerSection)->join(std::string("\
"));
            auto middleIndex = Math->floor(lines->get_length() / 2);
            auto middleStart = Math->max(maxLinesPerSection, middleIndex - Math->floor(maxLinesPerSection / 2));
            auto middleEnd = Math->min(lines->get_length() - maxLinesPerSection, middleIndex + Math->floor(maxLinesPerSection / 2));
            auto codeMiddle = lines->slice(middleStart, middleEnd)->join(std::string("\
"));
            auto codeEnd = lines->slice(lines->get_length() - maxLinesPerSection, -1)->join(std::string("\
"));
            auto truncatedBlock = string_empty + header + std::string("\
") + codeStart + std::string("\
// ... truncated [") + (lines->get_length() - maxLinesPerSection * 2) + std::string(" lines] ...\
") + codeMiddle + std::string("\
// ... truncated ...\
") + codeEnd + std::string("\
") + footer + string_empty;
            code = code->replace(block, truncatedBlock);
        }
    }
    if (code->get_length() > maxLength) {
        auto blocks = code->split(std::string("```"));
        auto truncatedBlocks = blocks->map([=](auto block, auto index) mutable
        {
            if (index % 2 == 1) {
                auto lines = block->split(std::string("\
"));
                auto maxLines = 10;
                return string_empty + lines->slice(0, maxLines)->join(std::string("\
")) + std::string("\
// ... remaining code truncated ...\
");
            }
            return block->slice(0, 500);
        }
        );
        code = truncatedBlocks->join(std::string("```"));
    }
    return code;
}

