#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

"use client";

;
;
;
;
;
;
;

 = useAuth();

  if (isLoading) {
    return (
      <div className="flex items-center space-x-2">
        <Skeleton className="h-8 w-16 rounded-full" />
        {/* <Skeleton className="h-4 w-20" /> */}
      </div>
    );
  }

  if (!token || !user) {
    return (
      <Button variant="outline" onClick={signin}>
        <Github className="mr-2 h-4 w-4" /> Login with GitHub
      </Button>
    );
  }

  return (
    <DropdownMenu>
      <DropdownMenuTrigger asChild>
        <Button variant="ghost" className="relative h-10 w-10 rounded-full p-0">
          <Avatar className="h-10 w-10">
            <AvatarImage src={user.avatar_url} alt={user.login} />
            <AvatarFallback>
              {user.login?.charAt(0).toUpperCase()}
            </AvatarFallback>
          </Avatar>
        </Button>
      </DropdownMenuTrigger>
      <DropdownMenuContent className="w-56" align="end" forceMount>
        <DropdownMenuLabel className="font-normal">
          <div className="flex flex-col space-y-1">
            <p className="text-sm font-medium leading-none">
              {user.name || user.login}
            </p>
          </div>
        </DropdownMenuLabel>
        <DropdownMenuSeparator />
        <DropdownMenuItem asChild>
          <Link href="/profile/edit">
            <Wallet className="mr-2 h-4 w-4" />
            <span>Link Wallets</span>
          </Link>
        </DropdownMenuItem>
        <DropdownMenuSeparator />
        <DropdownMenuItem onClick={signout}>
          <LogOut className="mr-2 h-4 w-4" />
          <span>Log out</span>
        </DropdownMenuItem>
      </DropdownMenuContent>
    </DropdownMenu>
  );
}

} // namespace elizaos
