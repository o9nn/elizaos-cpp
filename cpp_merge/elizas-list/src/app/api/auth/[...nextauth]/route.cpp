#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/app/api/auth/[/* spread: nextauth */]/route.h"

std::shared_ptr<AuthOptions> authOptions = object{
    object::pair{std:("adapter"), PrismaAdapter(prisma)}, 
    object::pair{std:("providers"), array<any>{ GithubProvider(object{
        object::pair{std:("clientId"), OR((process->env->GITHUB_ID), (string_empty))}, 
        object::pair{std:("clientSecret"), OR((process->env->GITHUB_SECRET), (string_empty))}
    }) }}, 
    object::pair{std:("secret"), process->env->NEXTAUTH_SECRET}, 
    object::pair{std:("session"), object{
        object::pair{std:("strategy"), std:("jwt")}
    }}, 
    object::pair{std:("pages"), object{
        object::pair{std:("signIn"), std:("/auth/signin")}
    }}
};
any handler = NextAuth(authOptions);

void Main(void)
{
}

MAIN
