#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/email.h"

void sendProjectNotification(string to, object project)
{
    auto html = render(ProjectNotification(object{
        object::pair{std::string("project"), std::string("project")}
    }));
    std::async([=]() { transporter->sendMail(object{
        object::pair{std::string("from"), std::string(""ELIZAS LIST" <notifications@elizaslist.dev>")}, 
        object::pair{std::string("to"), std::string("to")}, 
        object::pair{std::string("subject"), std::string("New Project: ") + project["name"] + string_empty}, 
        object::pair{std::string("html"), std::string("html")}
    }); });
};


any transporter = createTransport(object{
    object::pair{std::string("host"), process->env->SMTP_HOST}, 
    object::pair{std::string("port"), parseInt(OR((process->env->SMTP_PORT), (std::string("587"))))}, 
    object::pair{std::string("auth"), object{
        object::pair{std::string("user"), process->env->SMTP_USER}, 
        object::pair{std::string("pass"), process->env->SMTP_PASS}
    }}
});

void Main(void)
{
}

MAIN
