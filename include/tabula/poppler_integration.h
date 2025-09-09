// Simple header to expose a run function for poppler-based extraction
#pragma once
#include <string>
#include <vector>

int poppler_integration_run(const std::vector<std::string> &args,
                            const std::vector<int> &pages = std::vector<int>());
