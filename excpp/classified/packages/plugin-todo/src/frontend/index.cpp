#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AddTaskForm() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [name, setName] = useState('');
    const auto [type, setType] = useState<'one-off' | 'daily'>('one-off');
    const auto [priority, setPriority] = useState('4');
    const auto [dueDate, setDueDate] = useState('');
    const auto [isUrgent, setIsUrgent] = useState(false);
    const auto [selectedWorldId, setSelectedWorldId] = useState<string>('');
    const auto [selectedRoomId, setSelectedRoomId] = useState<string>('');
    const auto addTaskMutation = useAddTask();

    const auto availableRooms = worlds.find((w) => w.worldId == selectedWorldId).rooms || [];

    useEffect(() => {
        setSelectedRoomId('');
        }, [selectedWorldId]);

        const auto handleSubmit = [&](e: React.FormEvent) {;
            e.preventDefault();
            if (!name.trim() || !selectedRoomId) {
                alert('Please enter a task name and select a world/room.');
                return;
            }
            const CreateTaskData taskData = { name = name.trim(), type, roomId = selectedRoomId };
            if (type == 'one-off') {
                taskData.priority = parseInt(priority, 10);
                if (dueDate) {
                    taskData.dueDate = dueDate;
                }
                taskData.isUrgent = isUrgent;
            }
            addTaskMutation.mutate(taskData, {
                onSuccess: () => {
                    setName('');
                    setType('one-off');
                    setPriority('4');
                    setDueDate('');
                    setIsUrgent(false);
                    },
                    onError: (error) => {
                        "Error adding task: " + std::to_string(error.message)
                        },
                        });
                        };

                        return (;
                        <Card>;
                        <CardHeader>;
                        <CardTitle>Add New Task</CardTitle>;
                        </CardHeader>;
                        <CardContent>;
                        <form onSubmit={handleSubmit} className="space-y-4">;
                        <div className="space-y-2">;
                        <Label htmlFor="world-select">World</Label>;
                        <Select value={selectedWorldId} onValueChange={setSelectedWorldId} required>;
                        <SelectTrigger id="world-select" data-testid="world-select">;
                        <SelectValue placeholder="Select World" />;
                        </SelectTrigger>;
                        <SelectContent>;
                        {worlds.map((world, index) => (;
                        <SelectItem;
                    key={world.worldId}
                value={world.worldId}
            "world-option-" + std::to_string(index + 1);
            >;
        {world.worldName}
        </SelectItem>;
    ))}
    </SelectContent>;
    </Select>;
    </div>;

    {selectedWorldId && (;
    <div className="space-y-2">;
    <Label htmlFor="room-select">Room</Label>;
    <Select;
    value={selectedRoomId}
    onValueChange={setSelectedRoomId}
    required;
    disabled={!selectedWorldId || availableRooms.length == 0}
    >;
    <SelectTrigger id="room-select" data-testid="room-select">;
    <SelectValue;
    placeholder={availableRooms.length > 0 ? 'Select Room' : 'No rooms in world'}
    />;
    </SelectTrigger>;
    <SelectContent>;
    {availableRooms.map((room, index) => (;
    <SelectItem;
    key={room.roomId}
    value={room.roomId}
    "room-option-" + std::to_string(index + 1);
    >;
    {room.roomName}
    </SelectItem>;
    ))}
    </SelectContent>;
    </Select>;
    </div>;
    )}

    <Separator />;

    <div className="space-y-2">;
    <Label htmlFor="task-name">Task Name</Label>;
    <Input;
    id="task-name";
    data-testid="task-name";
    value={name}
    onChange={(e) => setName(e.target.value)}
    placeholder="What needs to be done?";
    required;
    disabled={!selectedRoomId}
    />;
    </div>;

    <div className="space-y-2">;
    <Label htmlFor="task-type">Type</Label>;
    <Select;
    value={type}
    onValueChange={(value) => setType(value as 'one-off' | 'daily')}
    disabled={!selectedRoomId}
    >;
    <SelectTrigger id="task-type" data-testid="task-type">;
    <SelectValue placeholder="Select type" />;
    </SelectTrigger>;
    <SelectContent>;
    <SelectItem value="one-off">One-off</SelectItem>;
    <SelectItem value="daily">Daily</SelectItem>;
    <SelectItem value="aspirational">Aspirational</SelectItem>;
    </SelectContent>;
    </Select>;
    </div>;

    {type == 'one-off' && (;
    <div className="pl-4 border-l-2 space-y-4">;
    <div className="space-y-2">;
    <Label htmlFor="task-priority">Priority (1=High)</Label>;
    <Select value={priority} onValueChange={setPriority} disabled={!selectedRoomId}>;
    <SelectTrigger id="task-priority" data-testid="task-priority">;
    <SelectValue placeholder="Select priority" />;
    </SelectTrigger>;
    <SelectContent>;
    <SelectItem value="1">1</SelectItem>;
    <SelectItem value="2">2</SelectItem>;
    <SelectItem value="3">3</SelectItem>;
    <SelectItem value="4">4</SelectItem>;
    </SelectContent>;
    </Select>;
    </div>;
    <div className="space-y-2">;
    <Label htmlFor="task-due-date">Due Date</Label>;
    <Input;
    id="task-due-date";
    data-testid="task-due-date";
    type="date";
    value={dueDate}
    onChange={(e) => setDueDate(e.target.value)}
    disabled={!selectedRoomId}
    />;
    </div>;
    <div className="flex items-center space-x-2">;
    <Checkbox;
    id="task-urgent";
    data-testid="task-urgent";
    checked={isUrgent}
    onCheckedChange={(checked) => setIsUrgent(checked == true)}
    disabled={!selectedRoomId}
    aria-labelledby="task-urgent-label";
    />;
    <Label;
    htmlFor="task-urgent";
    id="task-urgent-label";
    className="font-normal cursor-pointer";
    >;
    Urgent;
    </Label>;
    </div>;
    </div>;
    )}

    <Button;
    type="submit";
    data-testid="create-todo-btn";
    disabled={addTaskMutation.isPending || !selectedRoomId}
    >;
    {addTaskMutation.isPending ? <Loader />  = 'Add Task'}
    </Button>;
    {addTaskMutation.isError && (;
    <p className="text-red-500 text-sm">Error: {addTaskMutation.error.message}</p>
    )}
    </form>;
    </CardContent>;
    </Card>;
    );

}

void App() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { data: worlds, isLoading, error, isSuccess } = useTodos();
    const auto createRoomMutation = useCreateRoom();

    useEffect(() => {
        if (isSuccess) {
            std::cout << 'Fetched Worlds Data:' << worlds << std::endl;
        }
        if (error) {
            std::cerr << 'useTodos Error:' << error << std::endl;
        }
        }, [worlds, isSuccess, error]);

        const auto handleAddRoom = [&](worldId: string) {;
            const auto roomName = prompt('Enter the name for the new room:');
            if (roomName && roomName.trim()) {
                createRoomMutation.mutate({ worldId, name: roomName.trim() });
                } else if (roomName != nullptr) {
                    alert('Room name cannot be empty.');
                }
                };

                return (;
                <div data-testid="todo-app" className="flex flex-col gap-6 my-4 bg-background min-h-screen">;
                <div className="container flex items-center gap-4 py-4 border-b">;
                <div className="text-3xl font-bold">📝 Task Manager (All Worlds)</div>;
                </div>;

                <div className="container flex flex-col lg:flex-row gap-6">
            {/* Left Column = Add Task Form, Tags List, All Tasks List */}
            <div className="lg:w-1/3 space-y-4 flex-shrink-0">
            <AddTaskForm worlds={worlds || []} />;
            <TagsList />;
            <AllTasksList />;
            </div>;

        {/* Right Column = Task List by World/Room */}
        <div className="lg:w-2/3 space-y-6">
    {isLoading && <Loader data-testid="loader" />}
    {error && <p className="text-red-500">Error loading tasks = {error.message}</p>}

    {!isLoading && !error && (!worlds || worlds.length == 0) && (;
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
    data-testid="world-section";
    >;
    <div className="flex items-center justify-between">;
    <CollapsibleTrigger asChild>;
    <Button;
    variant="link";
    data-testid="world-toggle";
    className="text-xl font-semibold p-0 text-left hover:no-underline"
    >;
    {world.worldName}
    </Button>;
    </CollapsibleTrigger>;
    <Button;
    variant="ghost";
    size="sm";
    data-testid="add-room-btn";
    onClick={() => handleAddRoom(world.worldId)}
    disabled={createRoomMutation.isPending}
    aria-label="Add new room to this world";
    className="flex items-center gap-1 text-muted-foreground hover:text-foreground"
    >;
    {createRoomMutation.isPending ? <Loader />  = <PlusCircle className="size-4" />}
    Add Room;
    </Button>;
    </div>;
    <CollapsibleContent className="space-y-4 pl-4 border-l-2" data-testid="world-rooms">;
    {world.rooms.length == 0 && (;
    <p className="text-muted-foreground text-sm italic pl-2">;
    No rooms with tasks in this world yet.;
    </p>;
    )}
    {world.rooms.map((room) => (;
    <Card;
    key={room.roomId}
    data-testid="room-section";
    className="border shadow-sm ml-2 p-1 bg-card";
    >;
    <CardHeader className="p-2 pb-1">;
    <CardTitle className="text-base font-medium" data-testid="room-name">;
    {room.roomName}
    </CardTitle>;
    </CardHeader>;
    <CardContent className="space-y-1 p-2 pt-1">;
    {room.tasks.filter((task) => !task.tags.includes('completed')).length >;
    0 ? (;
    room.tasks;
    .filter((task) => !task.tags.includes('completed'));
    .map((task) => <TaskItem key={task.id} task={task} />);
    ) : (
    <p className="text-muted-foreground text-xs px-2 py-1">;
    No pending tasks in this room.;
    </p>;
    )}
    {room.tasks.some((task) => task.tags.includes('completed')) && (;
    <>;
    <Separator className="my-2" />;
    <div className="space-y-1" data-testid="completed-section">;
    {room.tasks;
    .filter((task) => task.tags.includes('completed'));
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
