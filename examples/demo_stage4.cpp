#include "elizaos/ljspeechtools.hpp"
#include "elizaos/livevideochat.hpp"
#include "elizaos/agentlogger.hpp"
#include <iostream>
#include <filesystem>

using namespace elizaos;

int main() {
    AgentLogger logger;
    
    std::cout << "=== ElizaOS Stage 4 Multimedia Demo ===\n" << std::endl;
    
    // Demo LJSpeechTools
    std::cout << "1. Testing LJSpeechTools functionality:\n" << std::endl;
    
    LJSpeechTools speech_tools;
    if (!speech_tools.initialize()) {
        std::cerr << "Failed to initialize LJSpeechTools" << std::endl;
        return 1;
    }
    
    // Test audio transcription
    auto& transcriber = speech_tools.getTranscriber();
    AudioData test_audio;
    test_audio.samples.resize(22050, 0.5f); // 1 second of audio
    test_audio.sample_rate = 22050;
    test_audio.channels = 1;
    test_audio.duration_seconds = 1.0;
    
    auto transcription = transcriber.transcribe(test_audio);
    std::cout << "  - Transcription result: " << transcription.text << std::endl;
    std::cout << "  - Confidence: " << transcription.confidence << std::endl;
    
    // Test speech synthesis
    auto& synthesizer = speech_tools.getSynthesizer();
    SynthesisConfig synth_config;
    synth_config.sample_rate = 22050;
    synth_config.channels = 1;
    
    auto synthesized_audio = synthesizer.synthesize("Hello from ElizaOS C++ Stage 4!", synth_config);
    std::cout << "  - Synthesized audio length: " << synthesized_audio.samples.size() << " samples" << std::endl;
    std::cout << "  - Duration: " << synthesized_audio.duration_seconds << " seconds" << std::endl;
    
    // Test dataset preparation
    std::string temp_dir = (std::filesystem::temp_directory_path() / "elizaos_demo").string();
    std::filesystem::create_directories(temp_dir + "/input");
    
    std::cout << "  - Running dataset pipeline..." << std::endl;
    bool pipeline_success = speech_tools.runPipeline(
        temp_dir + "/input",
        temp_dir + "/output",
        false  // verbose = false
    );
    std::cout << "  - Pipeline result: " << (pipeline_success ? "SUCCESS" : "FAILED") << std::endl;
    
    std::cout << "\n2. Testing LiveVideoChat functionality:\n" << std::endl;
    
    // Demo LiveVideoChat
    LiveVideoChat video_chat;
    VideoChatConfig chat_config;
    chat_config.video_width = 1280;
    chat_config.video_height = 720;
    chat_config.video_fps = 30;
    chat_config.audio_sample_rate = 48000;
    chat_config.audio_channels = 2;
    
    if (!video_chat.initialize(chat_config)) {
        std::cerr << "Failed to initialize LiveVideoChat" << std::endl;
        return 1;
    }
    
    // Test video capture
    auto video_capture = video_chat.getVideoCapture();
    if (video_capture) {
        std::cout << "  - Starting video capture..." << std::endl;
        video_capture->start();
        
        auto video_frame = video_capture->getNextFrame();
        std::cout << "  - Video frame: " << video_frame.width << "x" << video_frame.height 
                  << " (" << video_frame.format << ")" << std::endl;
        std::cout << "  - Frame data size: " << video_frame.data.size() << " bytes" << std::endl;
        
        video_capture->stop();
    }
    
    // Test audio capture
    auto audio_capture = video_chat.getAudioCapture();
    if (audio_capture) {
        std::cout << "  - Starting audio capture..." << std::endl;
        audio_capture->start();
        
        auto audio_frame = audio_capture->getNextFrame();
        std::cout << "  - Audio frame: " << audio_frame.sample_rate << "Hz, " 
                  << audio_frame.channels << " channels" << std::endl;
        std::cout << "  - Frame samples: " << audio_frame.samples.size() << std::endl;
        
        audio_capture->stop();
    }
    
    // Test video chat session
    std::cout << "  - Creating video chat session..." << std::endl;
    auto session = video_chat.createSession("demo_session");
    if (session) {
        session->startSession("demo_peer");
        
        auto session_info = session->getSessionInfo();
        std::cout << "  - Session ID: " << session_info.session_id << std::endl;
        std::cout << "  - Peer ID: " << session_info.peer_id << std::endl;
        std::cout << "  - Start time: " << session_info.start_time_ms << std::endl;
        
        session->endSession();
    }
    
    // Test WebRTC peer connection
    std::cout << "  - Testing WebRTC functionality..." << std::endl;
    WebRTCPeerConnection peer_conn;
    peer_conn.initialize(chat_config);
    
    std::string offer = peer_conn.createOffer();
    std::cout << "  - SDP offer created (length: " << offer.length() << " chars)" << std::endl;
    
    // Create another peer to simulate answer
    WebRTCPeerConnection peer_conn2;
    peer_conn2.initialize(chat_config);
    std::string answer = peer_conn2.createAnswer(offer);
    std::cout << "  - SDP answer created (length: " << answer.length() << " chars)" << std::endl;
    
    // Complete the connection
    peer_conn.setRemoteDescription(answer);
    peer_conn2.setRemoteDescription(offer);
    
    std::cout << "  - Peer connection 1 state: " << static_cast<int>(peer_conn.getState()) << std::endl;
    std::cout << "  - Peer connection 2 state: " << static_cast<int>(peer_conn2.getState()) << std::endl;
    
    peer_conn.close();
    peer_conn2.close();
    
    // Cleanup
    video_chat.shutdown();
    
    // Clean up temp directory
    std::filesystem::remove_all(temp_dir);
    
    std::cout << "\n=== Demo completed successfully! ===" << std::endl;
    
    return 0;
}