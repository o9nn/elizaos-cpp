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



//  // AvatarGroup removed

// The group prop will be a central channel, enriched with server_id for navigation context
// Assume group.participants might be available or added later.
struct GroupCardProps {
    ClientMessageChannel & { server_id: UUID; participants?: Partial<Agent>[] } group;

  // Assuming participant count might come from metadata or a separate query in the parent component

  // const handleEditClick = (e: React.MouseEvent) => {
  //   e.stopPropagation();
  //   if (onEdit) onEdit(group);
  // };


} // namespace elizaos
