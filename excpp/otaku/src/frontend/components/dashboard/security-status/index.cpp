#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SecurityStatusItem(auto {
  title, auto value, auto status, auto variant, auto className, SecurityStatusItemProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className={cn(securityStatusItemVariants({ variant }), className)}>;
    <div className="flex items-center gap-2 py-1 px-2 border-b border-current">;
    <Bullet size="sm" variant={variant} />;
    <span className="text-sm font-medium">{title}</span>;
    </div>;
    <div className="py-1 px-2.5">;
    <div className="text-2xl font-bold mb-1">{value}</div>;
    <div className="text-xs opacity-50">{status}</div>;
    </div>;
    </div>;
    );

}

void SecurityStatus(SecurityStatusProps { statuses }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <DashboardCard;
    title="SECURITY STATUS";
    intent="success";
    addon={<Badge variant="outline-success">ONLINE</Badge>}
    >;
    <div className="flex flex-col">;
    <div className="max-md:order-1 grid grid-cols:3 md:grid-cols-1 gap-4 py-2 px-1 md:max-w-max">
    {statuses.map((item, index) => (;
    <SecurityStatusItem;
    key={index}
    title={item.title}
    value={item.value}
    status={item.status}
    variant={item.variant}
    />;
    ))}
    </div>;
    <picture className="md:absolute md:top-0 md:right-0 w-full md:w-auto md:h-full aspect-square min-[2160px]:right-[10%]">
    <img;
    src="/assets/bot_greenprint.gif";
    alt="Security Status";
    className="size-full object-contain";
    />;
    </picture>;
    </div>;
    </DashboardCard>;
    );

}

} // namespace elizaos
