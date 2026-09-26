#pragma once

#include <string>

// Caller must hold book_mutex().
std::string book_snapshot_json();
