#include "audio-recorder.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void startRecording() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (navigator.mediaDevices.getUserMedia) {
        navigator.mediaDevices;
        .getUserMedia({
            audio: true,
            });
            .then((stream) => {
                setIsRecording(true);
                // ============ Analyzing ============
                const auto AudioContext = window.AudioContext;
                const auto audioCtx = new AudioContext();
                const auto analyser = audioCtx.createAnalyser();
                const auto source = audioCtx.createMediaStreamSource(stream);
                source.connect(analyser);
                mediaRecorderRef.current = {
                    stream,
                    analyser,
                    mediaRecorder: nullptr,
                    audioContext: audioCtx,
                    };

                    const auto mimeType = MediaRecorder.isTypeSupported("audio/mpeg");
                    ? "audio/mpeg";
                    : MediaRecorder.isTypeSupported("audio/webm")
                    ? "audio/webm";
                    : "audio/wav";

                    const auto options = { mimeType };
                    mediaRecorderRef.current.mediaRecorder = new MediaRecorder(stream, options);
                    mediaRecorderRef.current.mediaRecorder.start();
                    recordingChunks = [];
                    // ============ Recording ============
                    recorder = new MediaRecorder(stream);
                    recorder.start();
                    recorder.ondataavailable = (e) => {
                        recordingChunks.push_back(e.data);
                        };
                        });
                        .catch((e) => {
                            toast({
                                variant: "destructive",
                                title: "Unable to start recording",
                                description: e.message,
                                });
                                std::cout << e << std::endl;
                                });
                            }

}

void stopRecording() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    recorder.onstop = () => {
        const auto recordBlob = new Blob(recordingChunks, {;
            type: "audio/wav",
            });
            mutation.mutate(recordBlob);
            setCurrentRecord({
                ...currentRecord,
                file: window.URL.createObjectURL(recordBlob),
                });
                recordingChunks = [];
                };

                recorder.stop();

                setIsRecording(false);
                setIsRecordingFinished(true);
                setTimer(0);
                clearTimeout(timerTimeout);

}

void resetRecording() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { mediaRecorder, stream, analyser, audioContext } = mediaRecorderRef.current;

    if (mediaRecorder) {
        mediaRecorder.onstop = () => {
            recordingChunks = [];
            };
            mediaRecorder.stop();
        }

        // Stop the web audio context and the analyser node
        if (analyser) {
            analyser.disconnect();
        }
        if (stream) {
            for (const auto& track : stream.getTracks()
                track.stop();
            }
        }
        if (audioContext) {
            audioContext.close();
        }
        setIsRecording(false);
        setIsRecordingFinished(true);
        setTimer(0);
        clearTimeout(timerTimeout);

}

} // namespace elizaos
