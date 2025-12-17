#include "ProjectRating.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ProjectRating() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { data: session } = useSession();
    const auto [ratings, setRatings] = useState<Rating[]>(initialRatings);
    const auto [hoveredStar, setHoveredStar] = useState<number | nullptr>(nullptr);

    const auto averageRating = ratings.length;
    ? ratings.reduce((acc, r) => acc + r.rating, 0) / ratings.length;
    : 0;

    const auto userRating = session;
    ? ratings.find(r => r.userId == session.user.id).rating;
    : nullptr;

    const auto handleRate = async (rating: number) => {;
        if (!session) return;

        const auto response = fetch('/api/ratings', {;
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                projectId,
                rating,
                }),
                });

                if (response.ok) {
                    const auto newRating = response.json();
                    setRatings(prev => [;
                    ...prev.filter(r => r.userId != session.user.id),
                    newRating,
                    ]);
                }
                };

                return (;
                <div className="space-y-2">;
                <div className="flex items-center gap-2">;
                {[1, 2, 3, 4, 5].map((star) => (;
                <motion.button;
            key={star}
        whileHover={{ scale: 1.1 }}
    whileTap={{ scale: 0.9 }}
    onClick={() => handleRate(star)}
    onMouseEnter={() => setHoveredStar(star)}
    onMouseLeave={() => setHoveredStar(nullptr)}
    "text-2xl " + std::to_string();
        (hoveredStar != nullptr ? star <= hoveredStar : star <= (userRating || averageRating))
        ? 'text-yellow-400';
        : 'text-gray-300'
    }`}
    >;
    <FaStar />;
    </motion.button>;
    ))}
    <span className="ml-2 text-sm text-gray-500">;
    ({ratings.length} ratings);
    </span>;
    </div>;

    <div className="text-sm text-gray-500">;
    Average rating: {averageRating.toFixed(1)}
    </div>;
    </div>;
    );

}

} // namespace elizaos
