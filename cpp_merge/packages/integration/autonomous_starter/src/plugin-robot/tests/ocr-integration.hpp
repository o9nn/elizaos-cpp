#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ROBOT_TESTS_OCR_INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ROBOT_TESTS_OCR_INTEGRATION_TEST_H
#include "core.hpp"
#include "vitest.hpp"
#include "../service.h"
// External dependency removed
#include "canvas.hpp"

std::shared_ptr<Buffer> generateTextImage(string text, double width, double height, double fontSize = 24);

std::shared_ptr<Buffer> generateMultiLineTextImage(string text, double width, double height, double fontSize = 20);

std::shared_ptr<Buffer> generateComplexLayoutImage(double width, double height);

#endif
