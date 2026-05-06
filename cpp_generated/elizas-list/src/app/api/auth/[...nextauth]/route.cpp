#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/app/api/auth/[/* spread: nextauth */]/route.h"

std::shared_ptr<AuthOptions> authOptions = object{
    object::pair{std::string("adapter"), PrismaAdapter(prisma)}, 
    object::pair{std::string("providers"), array<any>{ GithubProvider(object{
        object::pair{std::string("clientId"), OR((process->env->GITHUB_ID), (string_empty))}, 
        object::pair{std::string("clientSecret"), OR((process->env->GITHUB_SECRET), (string_empty))}
    }) }}, 
    object::pair{std::string("secret"), process->env->NEXTAUTH_SECRET}, 
    object::pair{std::string("session"), object{
        object::pair{std::string("strategy"), std::string("jwt")}
    }}, 
    object::pair{std::string("pages"), object{
        object::pair{std::string("signIn"), std::string("/auth/signin")}
    }}
};
any handler = NextAuth(authOptions);

void Main(void)
{
}

MAIN
