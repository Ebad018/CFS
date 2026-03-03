#pragma once
#include <vector>
#include <string>

struct HistoryEvent {
    std::string text;
    int entityID = -1; // Primary ID (kept for backward compat or focus)
    std::vector<std::pair<std::string, int>> links; // Name -> ID mapping
};

class HistoryGenerator {
public:
    static std::vector<HistoryEvent> events;

    // Helper to accept just string (default ID -1)
    static void AddEvent(const std::string& event, int entityID = -1);
    
    // New Overload for multiple entities
    static void AddEvent(const std::string& event, std::vector<int> entityIDs);
    
    static const std::vector<HistoryEvent>& GetHistory();
    static void Clear();
};
