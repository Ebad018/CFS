#include "HistoryGenerator.h"
#include "SimEngine.h"

std::vector<HistoryEvent> HistoryGenerator::events;
int HistoryGenerator::next_event_id = 1;

int HistoryGenerator::AddStructuredEvent(int year, const std::string& type, int primary_id, int secondary_id, int loc_id, const std::string& text, std::map<std::string, std::string> meta) {
    HistoryEvent ev;
    ev.event_id = next_event_id++;
    ev.year = year;
    ev.event_type = type;
    ev.primary_actor_id = primary_id;
    ev.secondary_actor_id = secondary_id;
    ev.location_id = loc_id;
    ev.text = "[Year " + std::to_string(year) + "] " + text;
    ev.metadata = meta;
    
    events.push_back(ev);
    return ev.event_id;
}

void HistoryGenerator::AddEvent(const std::string& event, int entityID) {
    // Backwards compatibility fallback
    std::string date = SimEngine::Get().GetDateString();
    std::string fullEvent = "[" + date + "] " + event;
    
    HistoryEvent ev;
    ev.event_id = next_event_id++;
    ev.text = fullEvent;
    ev.primary_actor_id = entityID;
    
    events.push_back(ev);
}

void HistoryGenerator::AddEvent(const std::string& event, std::vector<int> entityIDs) {
    // Backwards compatibility fallback
    std::string date = SimEngine::Get().GetDateString();
    std::string fullEvent = "[" + date + "] " + event;

    int firstID = -1;
    if (!entityIDs.empty()) {
        firstID = entityIDs[0];
    }
    
    HistoryEvent ev;
    ev.event_id = next_event_id++;
    ev.text = fullEvent;
    ev.primary_actor_id = firstID;

    events.push_back(ev);
}

const std::vector<HistoryEvent>& HistoryGenerator::GetHistory() {
    return events;
}

void HistoryGenerator::Clear() {
    events.clear();
    next_event_id = 1;
}
