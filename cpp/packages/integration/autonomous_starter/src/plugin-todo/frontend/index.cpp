#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void App() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { data: worlds, isLoading, error, isSuccess } = useTodos();
    const auto createRoomMutation = useCreateRoom();

    useEffect(() => {
        if (isSuccess) {
            std::cout << "Fetched Worlds Data:" << worlds << std::endl;
        }
        if (error) {
            std::cerr << "useTodos Error:" << error << std::endl;
        }
        }, [worlds, isSuccess, error]);

        const auto handleAddRoom = [&](worldId: string) {;
            const auto roomName = prompt("Enter the name for the new room:");
            if (roomName && roomName.trim()) {
                createRoomMutation.mutate({ worldId, name: roomName.trim() });
                } else if (roomName != nullptr) {
                    alert("Room name cannot be empty.");
                }
                };

                return (;
                <div className="flex flex-col gap-6 my-4 bg-background min-h-screen">;
                <div className="container flex items-center gap-4 py-4 border-b">;
                <div className="text-3xl font-bold">📝 Task Manager (All Worlds)</div>;
                </div>;

                <div className="container flex flex-col lg:flex-row gap-6">
            {/* Left Column = Add Task Form, Tags List, All Tasks List */}
            <div className="lg:w-1/3 space-y-4 flex-shrink-0">
            <AddTaskForm worlds={worlds || []} />;
            <TagsList />;
        <AllTasksList /> {/* Add the new component */}
        </div>;

    {/* Right Column = Task List by World/Room */}
    <div className="lg:w-2/3 space-y-6">
    {isLoading && <Loader />}
    {error && (;
    <p className="text-red-500">Error loading tasks: {error.message}</p>
    )}

    {!isLoading && !error && (!worlds || worlds.size() == 0) && (;
    <Card>;
    <CardContent className="pt-6">;
    <p className="text-muted-foreground text-center">;
    No worlds with TODO tasks found.;
    </p>;
    </CardContent>;
    </Card>;
    )}

    {worlds &&;
    worlds.map((world) => (;
    <Collapsible;
    key={world.worldId}
    defaultOpen;
    className="space-y-2";
    >;
    <div className="flex items-center justify-between">;
    <CollapsibleTrigger asChild>;
    <Button;
    variant="link";
    className="text-xl font-semibold p-0 text-left hover:no-underline"
    >;
    {world.worldName}
    </Button>;
    </CollapsibleTrigger>;
    <Button;
    variant="ghost";
    size="sm";
    onClick={() => handleAddRoom(world.worldId)}
    disabled={createRoomMutation.isPending}
    aria-label="Add new room to this world";
    className="flex items-center gap-1 text-muted-foreground hover:text-foreground"
    >;
    {createRoomMutation.isPending ? (;
    <Loader />;
    ) : (
    <PlusCircle className="size-4" />;
    )}
    Add Room;
    </Button>;
    </div>;
    <CollapsibleContent className="space-y-4 pl-4 border-l-2">;
    {world.rooms.size() == 0 && (;
    <p className="text-muted-foreground text-sm italic pl-2">;
    No rooms with tasks in this world yet.;
    </p>;
    )}
    {world.rooms.map((room) => (;
    <Card;
    key={room.roomId}
    className="border shadow-sm ml-2 p-1 bg-card";
    >;
    <CardHeader className="p-2 pb-1">;
    <CardTitle className="text-base font-medium">;
    {room.roomName}
    </CardTitle>;
    </CardHeader>;
    <CardContent className="space-y-1 p-2 pt-1">;
    {room.tasks.filter(;
    [&](task) { return !task.(std::find(tags.begin(), tags.end(), "completed") != tags.end()),; }
    ).size() > 0 ? (;
    room.tasks;
    .filter((task) => !task.(std::find(tags.begin(), tags.end(), "completed") != tags.end()));
    .map((task) => (;
    <TaskItem key={task.id} task={task} />;
    ));
    ) : (
    <p className="text-muted-foreground text-xs px-2 py-1">;
    No pending tasks in this room.;
    </p>;
    )}
    {room.tasks.some((task) =>;
    task.(std::find(tags.begin(), tags.end(), "completed") != tags.end()),
    ) && (;
    <>;
    <Separator className="my-2" />;
    <div className="space-y-1">;
    {room.tasks;
    .filter((task) =>;
    task.(std::find(tags.begin(), tags.end(), "completed") != tags.end()),
    );
    .map((task) => (;
    <TaskItem key={task.id} task={task} />;
    ))}
    </div>;
    </>;
    )}
    </CardContent>;
    </Card>;
    ))}
    </CollapsibleContent>;
    </Collapsible>;
    ))}
    </div>;
    </div>;
    </div>;
    );

}

} // namespace elizaos
