#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ROBOT_TESTS_OCR-INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ROBOT_TESTS_OCR-INTEGRATION_TEST_H
#include "core.h"
#include "vitest.h"
#include "../service.h"
#include "@elizaos/core.h"
#include "canvas.h"

std::shared_ptr<Buffer> generateTextImage(string text, double width, double height, double fontSize = 24);

std::shared_ptr<Buffer> generateMultiLineTextImage(string text, double width, double height, double fontSize = 20);

std::shared_ptr<Buffer> generateComplexLayoutImage(double width, double height);

#endif
