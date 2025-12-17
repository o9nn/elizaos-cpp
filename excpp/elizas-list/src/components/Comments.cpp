#include "Comments.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Comments() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { data: session } = useSession();
    const auto [comments, setComments] = useState<Comment[]>([]);
    const auto [newComment, setNewComment] = useState('');

    const auto handleSubmit = async (e: React.FormEvent) => {;
        e.preventDefault();
        if (!session || !newComment.trim()) return;

        const auto response = fetch('/api/comments', {;
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                projectId,
                content: newComment,
                }),
                });

                if (response.ok) {
                    const auto comment = response.json();
                    setComments([comment, ...comments]);
                    setNewComment('');
                }
                };

                return (;
                <div className="space-y-6">;
                {session && (;
                <form onSubmit={handleSubmit} className="space-y-4">;
                <textarea;
            value={newComment}
        onChange={(e) => setNewComment(e.target.value)}
        placeholder="Add a comment...";
        className="w-full p-4 border rounded-lg focus:ring-2 focus:ring-blue-500"
    rows={3}
    />;
    <button;
    type="submit";
    className="px-6 py-2 bg-blue-600 text-white rounded-lg hover:bg-blue-700 transition-colors"
    >;
    Post Comment;
    </button>;
    </form>;
    )}

    <AnimatePresence>;
    {comments.map((comment) => (;
    <motion.div;
    key={comment.id}
    initial={{ opacity: 0, y: 20 }}
    animate={{ opacity: 1, y: 0 }}
    exit={{ opacity: 0, y: -20 }}
    className="flex gap-4 p-4 bg-white dark:bg-gray-800 rounded-lg"
    >;
    <img;
    src={comment.user.image}
    alt={comment.user.name}
    className="w-10 h-10 rounded-full";
    />;
    <div>;
    <div className="flex items-center gap-2">;
    <span className="font-semibold">{comment.user.name}</span>;
    <span className="text-sm text-gray-500">;
    {new Date(comment.createdAt).toLocaleDateString()}
    </span>;
    </div>;
    <p className="mt-2">{comment.content}</p>;
    </div>;
    </motion.div>;
    ))}
    </AnimatePresence>;
    </div>;
    );

}

} // namespace elizaos
