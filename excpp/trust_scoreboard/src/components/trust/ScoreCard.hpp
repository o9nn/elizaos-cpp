#include "ScoreCard.module.css.hpp"
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

struct ScoreCardProps {
    std::string title;
    double value;
    std::string description;
};


const ScoreCard: React.FC<ScoreCardProps> = ({ title, value, description }) => {
  return (
    <div className={styles.card}>
      <h3 className={styles.title}>{title}</h3>
      <div className={styles.value}>
        {value.toFixed(2)}
      </div>
      <p className={styles.description}>{description}</p>
    </div>
  );
};

default ScoreCard;
} // namespace elizaos
