#pragma once
#include <vector>
#include <string>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Struct for structured event data
struct HistoryEvent {
    int event_id = -1;
    int year = 0;
    std::string event_type = "Generic"; // "Birth", "Death", "Battle", "Constructed", "ArtifactDrop", "BeastTide"
    
    // Core relation graph for this event
    int primary_actor_id = -1;
    int secondary_actor_id = -1;
    int location_id = -1;
    int item_id = -1; // If it involves an artifact
    
    std::string text; // A constructed summary string
    
    // Optional extra data (e.g. "damage=50", "stolen_item=true")
    std::map<std::string, std::string> metadata;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(HistoryEvent, event_id, year, event_type, primary_actor_id, secondary_actor_id, location_id, item_id, text, metadata)
};

class HistoryGenerator {
public:
    static std::vector<HistoryEvent> events;

    // Phase 1 API: Structured Event Addition
    static int AddStructuredEvent(int year, const std::string& type, int primary_id, int secondary_id, int loc_id, const std::string& text, std::map<std::string, std::string> meta = {});
    
    // Backwards compatibility if needed
    static void AddEvent(const std::string& event, int entityID = -1);
    static void AddEvent(const std::string& event, std::vector<int> entityIDs);
    
    static const std::vector<HistoryEvent>& GetHistory();
    static void Clear();
    
private:
    static int next_event_id;
};
