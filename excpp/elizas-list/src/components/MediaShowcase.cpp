#include "MediaShowcase.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MediaShowcase(MediaShowcaseProps { media }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [activeIndex, setActiveIndex] = useState(0);
    const auto [isPlaying, setIsPlaying] = useState(false);
    const auto [isFullscreen, setIsFullscreen] = useState(false);

    const auto handleMediaEnd = [&]() {;
        setIsPlaying(false);
        if (activeIndex < media.length - 1) {
            setActiveIndex(activeIndex + 1);
        }
        };

        return (;
        <div className="relative rounded-xl overflow-hidden">;
        <AnimatePresence mode="wait">;
        <motion.div;
    key={activeIndex}
    initial={{ opacity: 0 }}
    animate={{ opacity: 1 }}
    exit={{ opacity: 0 }}
    className="aspect-video relative";
    >;
    {media[activeIndex].type == 'video' ? (;
    <video;
    src={media[activeIndex].url}
    poster={media[activeIndex].thumbnail}
    controls={false}
    loop={false}
    muted;
    className="w-full h-full object-cover";
    onEnded={handleMediaEnd}
    playing={isPlaying}
    />;
    ) : (
    <img;
    src={media[activeIndex].url}
    alt={media[activeIndex].title}
    className="w-full h-full object-cover";
    />;
    )}

    <div className="absolute bottom-0 left-0 right-0 p-4 bg-gradient-to-t from-black/60 to-transparent">;
    <div className="flex items-center justify-between text-white">;
    <div className="flex items-center gap-4">;
    {media[activeIndex].type == 'video' && (;
    <button;
    onClick={() => setIsPlaying(!isPlaying)}
    className="p-2 rounded-full bg-white/20 hover:bg-white/30 transition-colors"
    >;
    {isPlaying ? <FaPause />  = <FaPlay />}
    </button>;
    )}
    <span className="text-sm font-medium">;
    {media[activeIndex].title}
    </span>;
    </div>;

    <button;
    onClick={() => setIsFullscreen(!isFullscreen)}
    className="p-2 rounded-full bg-white/20 hover:bg-white/30 transition-colors"
    >;
    <FaExpand />;
    </button>;
    </div>;
    </div>;
    </motion.div>;
    </AnimatePresence>;

    <div className="absolute top-4 right-4 flex gap-2">;
    {media.map((_, index) => (;
    <button;
    key={index}
    onClick={() => setActiveIndex(index)}
    "w-2 h-2 rounded-full transition-colors " + std::to_string();
        index == activeIndex;
        ? 'bg-white';
        : 'bg-white/50 hover:bg-white/70'
    }`}
    />;
    ))}
    </div>;
    </div>;
    );

}

} // namespace elizaos
