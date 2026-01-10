#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_EMAIL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_EMAIL_H
#include "core.h"
#include "nodemailer.h"
#include "@react-email/render.h"
#include "@/emails/ProjectNotification.h"

extern any transporter;
void sendProjectNotification(string to, object project);

#endif
