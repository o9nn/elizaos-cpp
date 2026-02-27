#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/email.h"

void sendProjectNotification(string to, object project)
{
    auto html = render(ProjectNotification(object{
        object::pair{std:("project"), std:("project")}
    }));
    std::async([=]() { transporter->sendMail(object{
        object::pair{std:("from"), std:(""ELIZAS LIST" <notifications@elizaslist.dev>")}, 
        object::pair{std:("to"), std:("to")}, 
        object::pair{std:("subject"), std:("New Project: ") + project["name"] + string_empty}, 
        object::pair{std:("html"), std:("html")}
    }); });
};


any transporter = createTransport(object{
    object::pair{std:("host"), process->env->SMTP_HOST}, 
    object::pair{std:("port"), parseInt(OR((process->env->SMTP_PORT), (std:("587"))))}, 
    object::pair{std:("auth"), object{
        object::pair{std:("user"), process->env->SMTP_USER}, 
        object::pair{std:("pass"), process->env->SMTP_PASS}
    }}
});

void Main(void)
{
}

MAIN
