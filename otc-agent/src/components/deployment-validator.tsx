"use client";

import { useDeploymentValidation } from "@/hooks/useDeploymentValidation";

export function DeploymentValidator() {
  useDeploymentValidation();
  return null;
}
