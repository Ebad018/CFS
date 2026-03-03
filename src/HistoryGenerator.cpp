#include "HistoryGenerator.h"

#include "SimEngine.h"

std::vector<HistoryEvent> HistoryGenerator::events;

void HistoryGenerator::AddEvent(const std::string& event, int entityID) {
    std::string date = SimEngine::Get().GetDateString();
    std::string fullEvent = "[" + date + "] " + event;

    std::vector<std::pair<std::string, int>> links;
    if (entityID != -1) {
        auto* render = SimEngine::Get().GetRegistry().GetRenderable(entityID);
        if (render) links.push_back({render->name, entityID});
    }
    events.insert(events.begin(), {fullEvent, entityID, links}); 
}

void HistoryGenerator::AddEvent(const std::string& event, std::vector<int> entityIDs) {
    std::string date = SimEngine::Get().GetDateString();
    std::string fullEvent = "[" + date + "] " + event;

    std::vector<std::pair<std::string, int>> links;
    int firstID = -1;
    for (int id : entityIDs) {
        if (id == -1) continue;
        if (firstID == -1) firstID = id;
        auto* render = SimEngine::Get().GetRegistry().GetRenderable(id);
        if (render) links.push_back({render->name, id});
    }
    events.insert(events.begin(), {fullEvent, firstID, links});
}

const std::vector<HistoryEvent>& HistoryGenerator::GetHistory() {
    return events;
}

void HistoryGenerator::Clear() {
    events.clear();
}
