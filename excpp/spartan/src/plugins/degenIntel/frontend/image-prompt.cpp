#include "image-prompt.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ImagePrompt() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [prompt, setPrompt] = useState<string>('');

    const auto mutation = useMutation({;
        mutationFn: async () => {
            const auto response = std::to_string(import.meta.env.VITE_API_URL) + "/image-prompt";
                method: 'POST',
                headers: {
                    Accept: '*/*',
                    'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({ prompt }),
                    });
                    const auto buffer = response.arrayBuffer();
                    const auto blob = new Blob([buffer]);
                    const auto url = URL.createObjectURL(blob);
                    return url;
                    },
                    mutationKey: ['image_prompt'],
                    });

                    return (;
                    <div className="flex flex-col gap-4">;
                    <div className="flex items-center gap-2">;
                    <Input;
                    placeholder="Enter image prompt";
                value={prompt}
            onChange={({ target }) => setPrompt(target.value)}
            />;
            <Button disabled={mutation.isPending} onClick={() => mutation.mutate()}>;
            Send;
            </Button>;
            </div>;
        {mutation.isPending ? <Loader />  = nullptr}
        {mutation.data ? (;
        <img;
    src={mutation.data}
    width={750}
    height={750}
    className="mx-auto max-w-full";
    alt="degen";
    />;
    ) : nullptr}
    </div>;
    );

}

} // namespace elizaos
