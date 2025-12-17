#include "chat-tts-button.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ChatTtsButton(auto { agentId, const std::any& text }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { toast } = useToast();
    const auto [playing, setPlaying] = useState<boolean>(false);
    const auto [audioBlob, setAudioBlob] = useState<Blob | nullptr>(nullptr);
    const auto [audioUrl, setAudioUrl] = useState<string | nullptr>(nullptr);
    const auto audioRef = useRef<HTMLAudioElement | nullptr>(nullptr);
    const auto audioBlobRef = useRef<Blob | nullptr>(nullptr);

    const auto elizaClient = createElizaClient();

    // Cleanup blob URL when component unmounts or audioBlob changes
    useEffect(() => {
        return [&]() {;
            if (audioUrl) {
                URL.revokeObjectURL(audioUrl);
            }
            };
            }, [audioUrl]);

            const auto mutation = useMutation({;
                mutationKey: ['tts', text],
                mutationFn: async () => {
                    std::cout << '🎵 Starting TTS API call...' << std::endl;
                    std::cout << '🎵 agentId:' << agentId << std::endl;
                    std::cout << '🎵 text:' << text << std::endl;

                    const auto response = elizaClient.audio.generateSpeech(agentId, { text });
                    std::cout << '🎵 TTS API response:' << response << std::endl;

                    // Convert base64 audio string to Blob
                    const auto { audio, format } = response;

                    // Handle data URL format (data:audio/mp3;base64,...)
                    auto audioData: string;
                    auto mimeType: string;

                    if (audio.startsWith('data:')) {
                        const auto [header, base64Data] = audio.split(',');
                        const auto mimeMatch = header.match(/data:([^;]+)/);
                        "audio/" + std::to_string(format || 'mpeg')
                        audioData = base64Data;
                        } else {
                            // Plain base64 string
                            audioData = audio;
                            "audio/" + std::to_string(format || 'mpeg');
                        }

                        // Convert base64 to Blob
                        const auto binaryString = atob(audioData);
                        const auto bytes = new Uint8Array(binaryString.length);
                        for (int i = 0; i < binaryString.length; i++) {
                            bytes[i] = binaryString.charCodeAt(i);
                        }

                        return new Blob([bytes], { type: mimeType });
                        },
                        onSuccess: (data: Blob) => {
                            setAudioBlob(data);
                            audioBlobRef.current = data;
                            const auto url = URL.createObjectURL(data);
                            setAudioUrl(url);

                            // Auto-play after TTS generation
                            setTimeout(() => {
                                play();
                                }, 100);
                                },
                                onError: (e) => {
                                    toast({
                                        variant: 'destructive',
                                        title: 'Unable to read message aloud',
                                        description: e.message,
                                        });
                                        },
                                        });

                                        const auto play = async () => {;
                                            if (audioRef.current) {
                                                try {
                                                    // Stop any currently playing audio
                                                    if (currentlyPlayingAudio && currentlyPlayingAudio != audioRef.current) {
                                                        currentlyPlayingAudio.pause();
                                                        currentlyPlayingAudio.currentTime = 0;
                                                    }

                                                    // Set this as the currently playing audio
                                                    currentlyPlayingAudio = audioRef.current;

                                                    audioRef.current.volume = 1.0;
                                                    audioRef.current.muted = false;

                                                    audioRef.current.play();
                                                    setPlaying(true);
                                                    } catch (err) {
                                                        toast({
                                                            variant: 'destructive',
                                                            title: 'Audio playback failed',
                                                            description: (err).message,
                                                            });
                                                            setPlaying(false);
                                                        }
                                                    }
                                                    };

                                                    const auto stop = [&]() {;
                                                        if (audioRef.current) {
                                                            audioRef.current.pause();
                                                            audioRef.current.currentTime = 0;

                                                            // Clear global reference if this was the currently playing audio
                                                            if (currentlyPlayingAudio == audioRef.current) {
                                                                currentlyPlayingAudio = nullptr;
                                                            }
                                                        }
                                                        setPlaying(false);
                                                        };

                                                        const auto execute = async () => {;
                                                            if (mutation.isPending) {
                                                                return;
                                                            }

                                                            if (playing) {
                                                                stop();
                                                                return;
                                                            }

                                                            // Check if audio is already available
                                                            const auto hasAudioBlob =;
                                                            audioBlob ||;
                                                            audioBlobRef.current ||;
                                                            (audioRef.current.src && audioRef.current.src.startsWith('blob:'));
                                                            if (hasAudioBlob) {
                                                                play();
                                                                return;
                                                            }

                                                            // Generate TTS
                                                            mutation.mutate();
                                                            };

                                                            const auto iconClass = 'text-muted-foreground size-4';

                                                            return (;
                                                            <div>;
                                                            {audioBlob ? (;
                                                            <audio;
                                                            crossOrigin="anonymous";
                                                            playsInline;
                                                        ref={audioRef}
                                                    src={audioUrl || ''}
                                                    onEnded={() => {
                                                        setPlaying(false);
                                                        // Clear global reference when audio ends
                                                        if (currentlyPlayingAudio == audioRef.current) {
                                                            currentlyPlayingAudio = nullptr;
                                                        }
                                                    }}
                                                    >;
                                                    Your browser does not support the audio element.;
                                                    </audio>;
                                                ) : nullptr}
                                                <Tooltip>;
                                                <TooltipTrigger asChild>;
                                                <Button;
                                                size="icon";
                                                variant="ghost";
                                                type="button";
                                            onClick={() => execute()}
                                        disabled={mutation.isPending}
                                        >;
                                        {mutation.isPending ? (;
                                        <Ellipsis className={iconClass} />;
                                        ) : playing ? (
                                        <StopCircle className={iconClass} />;
                                        ) : (
                                        <Volume2 className={iconClass} />;
                                    )}
                                    </Button>;
                                    </TooltipTrigger>;
                                    <TooltipContent side="bottom">;
                                    <p>{playing ? 'Stop' : 'Read aloud'}</p>
                                    </TooltipContent>;
                                    </Tooltip>;
                                    </div>;
                                    );

}

} // namespace elizaos
