#pragma once

#include "json.hpp"
#include <string>

// Helper to generate json messages 
inline std::string gen_content(int status, const std::string& message) {
	nlohmann::json j;
	j["status"] = status;
	j["message"] = message;
	return j.dump(4);
}
