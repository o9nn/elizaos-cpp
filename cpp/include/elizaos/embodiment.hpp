#pragma once

/**
 * ElizaOS C++ - Embodiment Module
 *
 * Physical/sensor interface: sensory data types, motor actions,
 * and sensor/actuator interface classes.
 */

#include "elizaos.hpp"
#include "core.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// Sensory data types
// ============================================================================

enum class SensoryDataType {
    TEXTUAL,
    VISUAL,
    AUDITORY,
    ENVIRONMENTAL,
    PROPRIOCEPTIVE,
    UNKNOWN
};

enum class MotorActionType {
    MOVEMENT,
    SPEECH,
    COMMUNICATION,
    MANIPULATION,
    EXPRESSION,
    UNKNOWN
};

// ============================================================================
// Sensory data base
// ============================================================================

struct SensoryData {
    SensoryDataType type = SensoryDataType::UNKNOWN;
    double          confidence = 1.0;
    std::chrono::system_clock::time_point timestamp;

    virtual ~SensoryData() = default;
};

struct TextualData : public SensoryData {
    std::string text;
    std::string language = "en";
    std::string encoding = "UTF-8";

    TextualData() { type = SensoryDataType::TEXTUAL; }
    explicit TextualData(const std::string& t) : text(t) { type = SensoryDataType::TEXTUAL; }
};

struct VisualData : public SensoryData {
    int         width    = 0;
    int         height   = 0;
    int         channels = 3;
    std::string format   = "RGB";
    std::vector<uint8_t> pixels;

    VisualData() { type = SensoryDataType::VISUAL; }
};

struct AudioData : public SensoryData {
    int    sampleRate      = 44100;
    int    channels        = 1;
    double durationSeconds = 0.0;
    std::vector<float> samples;

    AudioData() { type = SensoryDataType::AUDITORY; }
};

struct EnvironmentalData : public SensoryData {
    double temperature = 20.0;
    double humidity    = 50.0;
    double pressure    = 1013.25;
    std::unordered_map<std::string, double> extraSensors;

    EnvironmentalData() { type = SensoryDataType::ENVIRONMENTAL; }
};

// ============================================================================
// Motor action base
// ============================================================================

struct MotorAction {
    std::string     actionId;
    MotorActionType type = MotorActionType::UNKNOWN;

    virtual ~MotorAction() = default;
};

struct MovementAction : public MotorAction {
    std::vector<double> targetPosition;
    double speed = 1.0;

    MovementAction() { type = MotorActionType::MOVEMENT; }
};

struct SpeechAction : public MotorAction {
    std::string text;
    std::string voice = "default";
    float       pitch = 1.0f;
    float       rate  = 1.0f;

    SpeechAction() { type = MotorActionType::SPEECH; }
    explicit SpeechAction(const std::string& t) : text(t) { type = MotorActionType::SPEECH; }
};

struct CommunicationAction : public MotorAction {
    std::string message;
    std::string recipient;
    std::string channel;

    CommunicationAction() { type = MotorActionType::COMMUNICATION; }
    CommunicationAction(const std::string& msg, const std::string& rec)
        : message(msg), recipient(rec) { type = MotorActionType::COMMUNICATION; }
};

// ============================================================================
// Abstract sensor interface
// ============================================================================

class SensorInterface {
public:
    virtual ~SensorInterface() = default;

    virtual bool initialize() = 0;
    virtual void shutdown()   = 0;
    virtual bool isActive() const = 0;

    virtual std::string     getName() const = 0;
    virtual SensoryDataType getType() const = 0;

    virtual std::shared_ptr<SensoryData> readData() { return nullptr; }

    void setConfiguration(const std::unordered_map<std::string, std::string>& cfg) { config_ = cfg; }
    const std::unordered_map<std::string, std::string>& getConfiguration() const { return config_; }

protected:
    std::unordered_map<std::string, std::string> config_;
    bool active_ = false;
};

// ============================================================================
// Abstract motor interface
// ============================================================================

class MotorInterface {
public:
    virtual ~MotorInterface() = default;

    virtual bool initialize() = 0;
    virtual void shutdown()   = 0;
    virtual bool isActive() const = 0;

    virtual std::string     getName() const = 0;
    virtual MotorActionType getType() const = 0;

    virtual bool canExecute(std::shared_ptr<MotorAction> action) const = 0;
    virtual bool executeAction(std::shared_ptr<MotorAction> action)    = 0;
    virtual bool isActionComplete(const std::string& actionId) const   = 0;
    virtual double getActionProgress(const std::string& actionId) const = 0;

    void setConfiguration(const std::unordered_map<std::string, std::string>& cfg) { config_ = cfg; }
    const std::unordered_map<std::string, std::string>& getConfiguration() const { return config_; }

protected:
    std::unordered_map<std::string, std::string> config_;
    bool active_ = false;
};

// ============================================================================
// Concrete implementations (console I/O)
// ============================================================================

class ConsoleTextInput : public SensorInterface {
public:
    bool            initialize() override { active_ = true; return true; }
    void            shutdown()   override { active_ = false; }
    bool            isActive()   const override { return active_; }
    std::string     getName()    const override { return "ConsoleTextInput"; }
    SensoryDataType getType()    const override { return SensoryDataType::TEXTUAL; }
    std::shared_ptr<SensoryData> readData() override;
};

class ConsoleTextOutput : public MotorInterface {
public:
    bool            initialize() override { active_ = true; return true; }
    void            shutdown()   override { active_ = false; }
    bool            isActive()   const override { return active_; }
    std::string     getName()    const override { return "ConsoleTextOutput"; }
    MotorActionType getType()    const override { return MotorActionType::COMMUNICATION; }

    bool canExecute(std::shared_ptr<MotorAction> action) const override;
    bool executeAction(std::shared_ptr<MotorAction> action) override;
    bool isActionComplete(const std::string& actionId) const override { return true; }
    double getActionProgress(const std::string& actionId) const override { return 1.0; }
};

// ============================================================================
// Mock interfaces for testing
// ============================================================================

class MockMotorInterface : public MotorInterface {
public:
    explicit MockMotorInterface(MotorActionType t) : motorType_(t) {}

    bool            initialize() override { active_ = true; return true; }
    void            shutdown()   override { active_ = false; }
    bool            isActive()   const override { return active_; }
    std::string     getName()    const override { return "MockMotorInterface"; }
    MotorActionType getType()    const override { return motorType_; }

    bool canExecute(std::shared_ptr<MotorAction>) const override { return active_; }
    bool executeAction(std::shared_ptr<MotorAction> action) override {
        if (action) lastActionId_ = action->actionId;
        return true;
    }
    bool isActionComplete(const std::string&) const override { return true; }
    double getActionProgress(const std::string&) const override { return 1.0; }

private:
    MotorActionType motorType_;
    std::string     lastActionId_;
};

} // namespace elizaos
