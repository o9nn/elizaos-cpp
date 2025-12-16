#include "..components/eliza/CreateSection.hpp"
#include "..components/eliza/PreviewCard.hpp"
#include "index.module.css.hpp"
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

;
;
;
;

const ElizaView: FC = () => {
  const [characterName, setCharacterName] = useState('');
  const [selectedPersonality, setSelectedPersonality] = useState('Manager');
  const [selectedSkills, setSelectedSkills] = useState(['Post']);

  return (
    <div className={styles.container}>
      <CreateSection 
        characterName={characterName}
        setCharacterName={setCharacterName}
        selectedPersonality={selectedPersonality}
        setSelectedPersonality={setSelectedPersonality}
        selectedSkills={selectedSkills}
        setSelectedSkills={setSelectedSkills}
      />
      <PreviewCard 
        name={characterName || 'Default NPC'}
        description="I will manage your community, and respond to questions."
        avatar="/default-profile.png"
      />
    </div>
  );
};

default ElizaView; 
} // namespace elizaos
