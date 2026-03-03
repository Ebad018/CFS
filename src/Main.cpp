#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "SimEngine.h"
#include "Database.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include "SimEngine.h"
#include "Database.h"
#include "WorldMap.h"
#include "HistoryGenerator.h"
#include "GameData.h" // Needed for Skill/Item names
#include "HistoryGenerator.h"

// Helper for Realm Names
std::string GetRealmName(int major) {
    switch(major) {
        case 0: return "Mortal";
        case 1: return "Qi Condensation";
        case 2: return "Foundation";
        case 3: return "Golden Core";
        case 4: return "Nascent Soul";
        case 5: return "Soul Formation";
        case 6: return "Void Refining";
        case 7: return "Body Integration";
        case 8: return "Mahayana";
        case 9: return "Tribulation";
        default: return "God";
    }
}

enum class GameState {
    MainMenu,
    Playing,
    Settings
};

int main() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1280, 720)), "Cultivation Fortress");
    window.setFramerateLimit(60);
    
    // Seed Randomness
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    if (!ImGui::SFML::Init(window)) {
        return -1;
    }
    
    // UI State for Inspectors
    int inspectedEntityID = -1;
    bool showCharacterInspector = false;
    int selectedSectID = -1; // Shared state for Sect Inspector
    std::vector<int> sectLeaders; // Moved to main scope to allow clearing

    // Initialize Systems
    Database db;
    db.Connect("game.db");
    db.InitializeTables();

    // Defer World Gen to "New Game"
    // SimEngine::Get().InitializeWorld();
    WorldMap world;
    // world.GenerateMultiverse();
    
    GameState currentState = GameState::MainMenu;
    bool showLoadGamePopup = false;

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // --- MAIN MENU STATE ---
        if (currentState == GameState::MainMenu) {
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(displaySize);
            ImGui::Begin("MainMenuBackground", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
            
            // Center the Menu
            // Approx width of buttons ~200. CenterX = DisplayW/2 - 100
            // CenterY = DisplayH/2 - (Height of stack / 2)
            ImGui::SetCursorPos(ImVec2(displaySize.x * 0.5f - 100.0f, displaySize.y * 0.35f));
            ImGui::BeginGroup();
            
            ImGui::SetWindowFontScale(2.0f);
            ImGui::Text("Cultivation Fortress");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();
            ImGui::Spacing();

            if (ImGui::Button("Play", ImVec2(200, 50))) {
                ImGui::OpenPopup("PlayMenu"); // Open context menu or change state? User asked for "Options for Play... under play we have new game or load game"
            }
            
            if (ImGui::BeginPopup("PlayMenu")) {
                if (ImGui::Selectable("New Game")) {
                     // Start New Game Logic
                     SimEngine::Get().GetRegistry().Clear(); // Ensure clean
                     SimEngine::Get().InitializeWorld();
                     world.GenerateMultiverse();
                     currentState = GameState::Playing;
                }
                if (ImGui::Selectable("Load Game")) {
                     showLoadGamePopup = true;
                     ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::IsItemClicked()) {
                ImGui::OpenPopup("PlayMenu");
            }

            ImGui::Spacing();

            if (ImGui::Button("Settings", ImVec2(200, 50))) {
                currentState = GameState::Settings;
            }

            ImGui::Spacing();

            if (ImGui::Button("Exit", ImVec2(200, 50))) {
                window.close();
            }

            ImGui::EndGroup();
            
            // --- ASYNC SAVE/LOAD OVERLAY (MAIN MENU) ---
            if (SimEngine::Get().IsLoading() || SimEngine::Get().IsSaving()) {
                ImGui::OpenPopup("ProcessingModal");
            }
            if (ImGui::BeginPopupModal("ProcessingModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
                ImGui::Text("Working...");
                if (SimEngine::Get().IsLoading()) ImGui::Text("Loading World Data...");
                if (SimEngine::Get().IsSaving()) ImGui::Text("Saving World Data...");
                ImGui::Separator();
                ImGui::Text("Please Wait.");
                // Spinner could go here
                
                // If finished, close
                if (!SimEngine::Get().IsLoading() && !SimEngine::Get().IsSaving()) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

// Include filesystem
#include <filesystem>
namespace fs = std::filesystem;

// ...

            // Load Game Modal
            if (showLoadGamePopup) {
                ImGui::OpenPopup("Load Game");
            }
            if (ImGui::BeginPopupModal("Load Game", &showLoadGamePopup, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Select Save File:");
                ImGui::Separator();
                
                // Ensure SaveData exists
                if (!fs::exists("SaveData")) fs::create_directory("SaveData");
                
                for (const auto& entry : fs::directory_iterator("SaveData")) {
                    if (entry.is_directory()) {
                        std::string slotName = entry.path().filename().string();
                        // Ignore Sects folder if it exists in root (legacy) or ensure structure
                        // For now just list everything
                        if (ImGui::Button(slotName.c_str(), ImVec2(200, 0))) {
                            SimEngine::Get().GetRegistry().Clear(); // Clean state
                            SimEngine::Get().StartAsyncLoad(slotName, world); // ASYNC LOAD
                            currentState = GameState::Playing;
                            showLoadGamePopup = false;
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
                
                ImGui::Separator();
                if (ImGui::Button("Cancel", ImVec2(200, 0))) { 
                    showLoadGamePopup = false; 
                    ImGui::CloseCurrentPopup(); 
                }
                ImGui::EndPopup();
            }

            ImGui::End();
        }

        // --- SETTINGS STATE ---
        if (currentState == GameState::Settings) {
            ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse);
            ImGui::Text("Game Settings");
            ImGui::Separator();
            
            static float volume = 100.0f;
            ImGui::SliderFloat("Master Volume", &volume, 0.0f, 100.0f);
            
            static bool fullscreen = false;
            if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
                // Toggle TODO
            }

            ImGui::Spacing();
            if (ImGui::Button("Back")) {
                currentState = GameState::MainMenu;
            }
            ImGui::End();
        }

        // --- PLAYING STATE ---
        if (currentState == GameState::Playing) {
             // BLOCKING OVERLAY FOR ASYNC OPS
             if (SimEngine::Get().IsSaving() || SimEngine::Get().IsLoading()) {
                 ImGui::OpenPopup("GameProcessingModal");
                 if (ImGui::BeginPopupModal("GameProcessingModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
                     ImGui::Text("Processing...");
                     if (SimEngine::Get().IsSaving()) ImGui::Text("Saving Game State...");
                     if (SimEngine::Get().IsLoading()) ImGui::Text("Loading Game State...");
                     ImGui::Separator();
                     ImGui::Text("Please Do Not Force Close.");
                     
                     if (!SimEngine::Get().IsSaving() && !SimEngine::Get().IsLoading()) {
                         ImGui::CloseCurrentPopup();
                     }
                     ImGui::EndPopup();
                 }
                 // SKIP RENDERING THE GAME
                 ImGui::SFML::Render(window);
                 window.display();
                 continue; 
             }
        
            // ImGui Window
            ImGui::Begin("Cultivation Fortress");
            
            // Draw Toolbar
            static bool showSaveNamePopup = false;
            if (ImGui::Button("Save Game")) {
                showSaveNamePopup = true;
            }
            
            // Save Name Modal
            if (showSaveNamePopup) {
                ImGui::OpenPopup("Save Name");
            }
            if (ImGui::BeginPopupModal("Save Name", &showSaveNamePopup, ImGuiWindowFlags_AlwaysAutoResize)) {
                static char saveNameBuffer[128] = "Slot_Manual";
                ImGui::Text("Enter Save Name:");
                ImGui::InputText("##savename", saveNameBuffer, 128);
                
                ImGui::Separator();
                
                if (ImGui::Button("Save", ImVec2(120, 0))) {
                    SimEngine::Get().StartAsyncSave(saveNameBuffer, world);
                    showSaveNamePopup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    showSaveNamePopup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Main Menu")) {
                 currentState = GameState::MainMenu;
            }
            ImGui::Separator();

        // --- ASYNC LOADING SCREEN ---
        if (SimEngine::Get().IsSimulating()) {
            ImGui::OpenPopup("SimulatingModal");
        }
        
        if (ImGui::BeginPopupModal("SimulatingModal", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
            float progress = SimEngine::Get().GetSimulationProgress();
            ImGui::Text("Simulating History... Please Wait.");
            ImGui::ProgressBar(progress, ImVec2(300, 20));
            ImGui::Text("%.1f%%", progress * 100.0f);
            
            if (!SimEngine::Get().IsSimulating()) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
            
            ImGui::End(); // End Main Window early
            // Skip World Rendering to avoid race conditions
            ImGui::SFML::Render(window);
            window.display();
            continue; 
        }

        if (SimEngine::Get().IsSimulating()) {
             // Just in case modal logic falters, ensure we don't access registry
             ImGui::End();
             ImGui::SFML::Render(window);
             window.display();
             continue;
        }

        
        // Player Stats
        auto& engine = SimEngine::Get();
        int playerID = engine.GetPlayerEntityID();
        
        // Better:
        StatsComponent* sc = engine.GetRegistry().GetStats(playerID); // Need to add accessor
        CultivationComponent* cc = engine.GetRegistry().GetCultivation(playerID);
        TransformComponent* tc = engine.GetRegistry().GetTransform(playerID);

        if (sc && cc && tc) {
            ImGui::Text("Name: Ye Chen"); // Hardcoded for now or get from RenderComponent
            ImGui::Separator();
            ImGui::Text("Realm: Mortal (%d.%d)", cc->realm_major, cc->realm_minor);
            ImGui::ProgressBar(cc->qi / cc->max_qi, ImVec2(0.0f, 0.0f), "Qi");
            ImGui::Text("Qi: %.1f / %.1f", cc->qi, cc->max_qi);
            ImGui::Text("HP: %lld / %lld", sc->hp, sc->max_hp);
            ImGui::Text("Pos: %d, %d, %d", tc->x, tc->y, tc->z);
            ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", engine.GetDateString().c_str());
            
            if (ImGui::Button("Cultivate (10 ticks)")) {
                engine.PlayerCultivate(10);
            }
        }
        
        // --- History Window (Clickable) ---
        ImGui::Begin("World History");
        
        if (ImGui::Button("Regenerate World")) {
             world.Clear();
             SimEngine::Get().GetRegistry().Clear();
             HistoryGenerator::Clear();
             SimEngine::Get().InitializeWorld(); 
             world.GenerateMultiverse();
             
             // Reset UI State
             inspectedEntityID = -1;
             showCharacterInspector = false;
             selectedSectID = -1;
             sectLeaders.clear(); // Clear cached IDs
        }
        
        ImGui::SameLine();
        ImGui::SameLine();
        
        static int simAmount = 1;
        static int simUnitIdx = 0; // 0=Days, 1=Months, 2=Years
        const char* units[] = { "Days", "Months", "Years" };
        
        ImGui::PushItemWidth(100);
        ImGui::InputInt("##SimAmt", &simAmount);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::Combo("##SimUnit", &simUnitIdx, units, IM_ARRAYSIZE(units));
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Simulate")) { 
            SimEngine::TimeUnit unit = SimEngine::TimeUnit::Days;
            if (simUnitIdx == 1) unit = SimEngine::TimeUnit::Months;
            if (simUnitIdx == 2) unit = SimEngine::TimeUnit::Years;
            SimEngine::Get().StartAsyncSimulation(simAmount, unit); // Use Async Start
            
            // Abort this frame to prevent race condition with Registry access
            ImGui::End(); // Close "World History"
            ImGui::End(); // Close "Cultivation Fortress"
            ImGui::SFML::Render(window);
            window.display();
            continue; 
        }


        // Fetch Structured Events
        const auto& events = HistoryGenerator::GetHistory(); // Now returns vector<HistoryEvent>
        
        ImGui::BeginChild("LogScroll", ImVec2(0, 0), true);
        int hIdx = 0;
        for (const auto& ev : events) {
            ImGui::PushID(hIdx++);
            // Multi-link parsing
            if (!ev.links.empty()) {
                std::string display = ev.text;
                // We need to print segments. "A" [LINK] "B" [LINK] "C"
                // Find all occurrences of all names
                struct Match { size_t pos; size_t len; int id; std::string name; };
                std::vector<Match> matches;
                
                for(const auto& link : ev.links) {
                    size_t pos = display.find(link.first);
                    while(pos != std::string::npos) {
                        // Check overlap with existing matches?
                        bool overlapped = false;
                        for(const auto& m : matches) {
                            if (pos < m.pos + m.len && pos + link.first.length() > m.pos) {
                                overlapped = true; break;
                            }
                        }
                        if (!overlapped) {
                             matches.push_back({pos, link.first.length(), link.second, link.first});
                        }
                        pos = display.find(link.first, pos + 1);
                    }
                }
                
                // Sort by position
                std::sort(matches.begin(), matches.end(), [](const Match& a, const Match& b){ return a.pos < b.pos; });
                
                size_t cursor = 0;
                for(const auto& m : matches) {
                    // Text before
                    if (m.pos > cursor) {
                        bool isDeath = display.find("[DEATH]") != std::string::npos;
                        if (isDeath) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                        ImGui::TextWrapped("%s", display.substr(cursor, m.pos - cursor).c_str());
                        if (isDeath) ImGui::PopStyleColor();
                        ImGui::SameLine(0,0);
                    }
                    
                    // Link
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.6f, 1.0f, 1.0f)); 
                    ImGui::PushID(m.id * 1000 + hIdx); // Unique ID attempt
                    if (ImGui::SmallButton(m.name.c_str())) {
                         inspectedEntityID = m.id;
                         showCharacterInspector = true;
                         selectedSectID = -1; 
                         SectComponent* s = SimEngine::Get().GetRegistry().GetSect(m.id);
                         if (s) selectedSectID = s->sect_id;
                    }
                    ImGui::PopID();
                    ImGui::PopStyleColor();
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("View %s", m.name.c_str());
                    ImGui::SameLine(0,0);
                    
                    cursor = m.pos + m.len;
                }
                
                // Text after last match
                if (cursor < display.length()) {
                     bool isDeath = display.find("[DEATH]") != std::string::npos;
                     if (isDeath) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                     ImGui::TextWrapped("%s", display.substr(cursor).c_str());
                     if (isDeath) ImGui::PopStyleColor();
                } else {
                    ImGui::NewLine(); // Finish line if ended on button match
                }
                
            } else {
                 if (ev.text.find("[DEATH]") != std::string::npos) {
                     ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", ev.text.c_str());
                 } else {
                     ImGui::TextWrapped("%s", ev.text.c_str());
                 }
            }

            ImGui::PopID();
        }
        ImGui::EndChild();
        ImGui::End();
        
        // --- Character Inspector Popup ---
        if (showCharacterInspector && inspectedEntityID != -1) {
            ImGui::Begin("Character Inspector", &showCharacterInspector);
            auto& reg = SimEngine::Get().GetRegistry();
            
            RenderComponent* inspRC = reg.GetRenderable(inspectedEntityID);
            CultivationComponent* inspCC = reg.GetCultivation(inspectedEntityID);
            StatsComponent* inspSC = reg.GetStats(inspectedEntityID);
            SectComponent* inspSect = reg.GetSect(inspectedEntityID);
            InjuryComponent* inspInj = reg.GetInjury(inspectedEntityID);
            
            if (inspRC) {
                ImGui::TextColored(ImVec4(0,1,1,1), "Name: %s", inspRC->name.c_str());
                if (inspSect) ImGui::Text("Sect: %s (Rank: %d)", inspSect->name.c_str(), inspSect->rank);
                ImGui::Separator();
                
                if (ImGui::BeginTabBar("InspectorTabs")) {
                    // --- STATS TAB ---
                    if (ImGui::BeginTabItem("Stats")) {
                        if (inspCC) {
                            ImGui::Text("Realm: %s (Tier %d)", GetRealmName(inspCC->realm_major).c_str(), inspCC->realm_minor);
                            
                            // Gender
                            RelationComponent* inspRel = reg.GetRelation(inspectedEntityID);
                            std::string sex = inspRel ? inspRel->gender : "?";
                            ImGui::Text("Age: %d / %d (%s)", inspCC->age, inspCC->lifespan, sex.c_str());
                            
                            ImGui::Text("Talent: %.2f", inspCC->talent);
                        }
                        if (inspSC) {
                            ImGui::Text("HP: %lld / %lld", inspSC->hp, inspSC->max_hp);
                            ImGui::Text("Attack: %lld", inspSC->attack);
                            ImGui::Text("Defense: %lld", inspSC->defense);
                        }
                        if (inspInj && inspInj->severity > 0) {
                             ImGui::TextColored(ImVec4(1,0.5f,0,1), "Injury: %s", inspInj->description.c_str());
                             if (inspInj->year_sustained > 0) {
                                 ImGui::Text("Sustained: Year %d (Caused by %s)", inspInj->year_sustained, inspInj->source.c_str());
                             }
                             ImGui::Text("Recovering in: %d ticks", inspInj->duration);
                        }
                        ImGui::EndTabItem();
                    }
                    
                    // --- INVENTORY TAB ---
                    if (ImGui::BeginTabItem("Inventory")) {
                        InventoryComponent* inv = reg.GetInventory(inspectedEntityID);
                        if (inv) {
                            ImGui::Text("Spirit Stones: %d", inv->spirit_stones);
                            ImGui::Text("Items:");
                            if (inv->items.empty()) ImGui::Text(" (Empty)");
                            for (const auto& it : inv->items) {
                                // Find Name & Def
                                std::string name = it.item_id;
                                std::string desc = "";
                                std::string type = "";
                                int val = 0;
                                int grade = 1;
                                
                                for(const auto& def : GameData::GetItems()) {
                                    if(def.id == it.item_id) {
                                        name = def.name;
                                        desc = def.description;
                                        type = def.type;
                                        val = def.value;
                                        grade = def.tier;
                                        // Append stats if weapon
                                        if (def.attack_bonus > 0) desc += "\nAtk: +" + std::to_string(def.attack_bonus);
                                        break; 
                                    }
                                }
                                
                                ImGui::PushID(it.item_id.c_str()); // Ensure unique ID for context menu
                                std::string label = name + " x" + std::to_string(it.count);
                                if (ImGui::Selectable(label.c_str())) {
                                    // Optional: Click to inspect item details in future
                                }
                                
                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    ImGui::TextColored(ImVec4(1,1,0,1), "%s", name.c_str());
                                    ImGui::Text("Grade: %d  Type: %s", grade, type.c_str());
                                    ImGui::Text("Value: %d SS", val);
                                    ImGui::Separator();
                                    ImGui::TextWrapped("%s", desc.c_str());
                                    ImGui::EndTooltip();
                                }
                                if (ImGui::BeginPopupContextItem()) {
                                    if (ImGui::Selectable("Donate to Sect")) {
                                        SimEngine::Get().DonateToSect(inspectedEntityID, it.item_id, 1);
                                    }
                                    ImGui::EndPopup();
                                }
                                ImGui::PopID();
                            }
                        } else {
                             ImGui::Text("No Inventory");
                        }
                        ImGui::EndTabItem();
                    }
                    
                    // --- SKILLS TAB ---
                    if (ImGui::BeginTabItem("Skills")) {
                        SkillsComponent* skills = reg.GetSkills(inspectedEntityID);
                        if (skills) {
                            // Method
                            std::string method = skills->active_method_id;
                            std::string mDesc = "";
                            std::string mEff = "";
                            for(const auto& def : GameData::GetCultivationMethods()) {
                                if(def.id == method) {
                                    method = def.name;
                                    mDesc = def.description;
                                    mEff = def.passive_effect;
                                    break;
                                }
                            }
                            ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "Mantra: %s", method.c_str());
                            if (ImGui::IsItemHovered() && !mDesc.empty()) {
                                ImGui::SetTooltip("%s\n\nPassive: %s", mDesc.c_str(), mEff.c_str());
                            }
                            
                            ImGui::Separator();
                            ImGui::Text("Martial Arts:");
                            for (const auto& s : skills->known_skills) {
                                std::string sName = s;
                                std::string sDesc = "";
                                int sCost = 0;
                                for(const auto& def : GameData::GetSkills()) {
                                    if(def.id == s) {
                                        sName = def.name;
                                        sDesc = def.description;
                                        sCost = def.qi_cost;
                                        break;
                                    }
                                }
                                ImGui::BulletText("%s (Qi: %d)", sName.c_str(), sCost);
                                if (ImGui::IsItemHovered() && !sDesc.empty()) {
                                    ImGui::SetTooltip("%s", sDesc.c_str());
                                }
                            }
                        } else {
                            ImGui::Text("No Known Skills");
                        }
                        ImGui::EndTabItem();
                    }
                    
                    // --- RELATIONS TAB ---
                    if (ImGui::BeginTabItem("Relations")) {
                        RelationComponent* rel = reg.GetRelation(inspectedEntityID);
                        if (rel) {
                            // Parents
                             ImGui::Text("Lineage:");
                             // (Ideally show names if IDs define, requires lookup loop)
                             ImGui::BulletText("Relationships: %d mapped", (int)rel->network.size());
                             
                             ImGui::Separator();
                             ImGui::Text("Network:");
                             for (const auto& [tid, rdata] : rel->network) {
                                 // Look up name
                                 RenderComponent* trc = reg.GetRenderable(tid);
                                 std::string tName = trc ? trc->name : "Unknown";
                                 ImVec4 col = (rdata.affinity > 0) ? ImVec4(0,1,0,1) : (rdata.affinity < 0) ? ImVec4(1,0,0,1) : ImVec4(1,1,1,1);
                                 
                                 ImGui::TextColored(col, "%s : %d (%s)", tName.c_str(), rdata.affinity, rdata.type.c_str());
                                 
                                 // Tooltip for History
                                 if (ImGui::IsItemHovered()) {
                                     ImGui::BeginTooltip();
                                     ImGui::Text("History with %s:", tName.c_str());
                                     if (rdata.history.empty()) {
                                         ImGui::Text("- No recorded interactions.");
                                     } else {
                                         for (const auto& log : rdata.history) {
                                             ImGui::Text("- %s", log.c_str());
                                         }
                                     }
                                     ImGui::EndTooltip();
                                 }
                             }
                        } else {
                            ImGui::Text("No Social Connections");
                        }
                        ImGui::EndTabItem();
                    }
                    
                    ImGui::EndTabBar();
                }

                ImGui::Separator();
                if (ImGui::Button("Focus on Map")) {
                    TransformComponent* targetTC = reg.GetTransform(inspectedEntityID);
                    if (targetTC) {
                         // Focus Logic handled by camera logic elsewhere or user reads pos
                    }
                }
            } else {
                ImGui::Text("Character data not found (Entity Destroyed?)");
            }
            ImGui::End();
        }

        // --- Sect Inspector ---
        ImGui::Begin("Sect Inspector");
        auto& reg = SimEngine::Get().GetRegistry();
        
        // static std::vector<int> sectLeaders; // Moved to main scope
        
        if (ImGui::Button("Refresh Sect List")) {
            sectLeaders.clear();
            for (auto& [e, sectComp] : reg.sects) {
                if (sectComp.rank == 5) { 
                    sectLeaders.push_back(e);
                }
            }
        }
        
        if (sectLeaders.empty()) {
            ImGui::Text("Click Refresh to load Sects.");
        } else {
            ImGui::BeginChild("SectList", ImVec2(150, 0), true);
            for (int leaderID : sectLeaders) {
                SectComponent* sectCp = reg.GetSect(leaderID);
                if (sectCp) { // Safe Check
                    if (ImGui::Selectable(sectCp->name.c_str(), selectedSectID == sectCp->sect_id)) {
                        selectedSectID = sectCp->sect_id;
                    }
                }
            }
            ImGui::EndChild();
            
            ImGui::SameLine();
            
            ImGui::BeginGroup();
            if (selectedSectID != -1) {
                ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "SECT OVERVIEW");
                
                // Count Population
                int popCount = 0;
                long long totalPower = 0;
                for (auto& [e, s] : reg.sects) {
                    if (s.sect_id == selectedSectID) {
                        // Check if dead
                        if (reg.GetStats(e) && reg.GetStats(e)->is_dead) continue;
                        popCount++;
                        if (reg.GetStats(e)) totalPower += reg.GetStats(e)->attack;
                    }
                }
                // Find Leader
                int leaderEntity = -1;
                for(int lid : sectLeaders) {
                    SectComponent* s = reg.GetSect(lid);
                    if(s && s->sect_id == selectedSectID) leaderEntity = lid;
                }

                if (ImGui::BeginTabBar("SectTabs")) {
                    // --- TAB 1: OVERVIEW ---
                    if (ImGui::BeginTabItem("Overview")) {
                         ImGui::Text("Population: %d", popCount);
                         ImGui::Text("Total Power: %lld", totalPower);
                         ImGui::Separator();
                         
                         ImGui::Text("Members:");
                         ImGui::BeginChild("MemberList", ImVec2(0, 300));
                         for (auto& [e, memberSC] : reg.sects) {
                             if (memberSC.sect_id == selectedSectID) {
                                 // Dead Check
                                 StatsComponent* stats = reg.GetStats(e);
                                 if (stats && stats->is_dead) continue;
                                 
                                 RenderComponent* rc = reg.GetRenderable(e);
                                 CultivationComponent* cultC = reg.GetCultivation(e);
                                 
                                 std::string rankStr = (memberSC.rank == 6) ? "[G.Eld]" : (memberSC.rank == 5) ? "[Ldr]" : (memberSC.rank == 4) ? "[Eld]" : (memberSC.rank == 3) ? "[Core]" : (memberSC.rank == 2) ? "[Inn]" : "[Out]";
                                 std::string realmStr = cultC ? GetRealmName(cultC->realm_major) : "Unknown";
                                 std::string label = rankStr + " " + (rc ? rc->name : "???") + " (" + realmStr + ")";
                                 
                                 ImGui::PushID(e);
                                 if (ImGui::Selectable(label.c_str(), inspectedEntityID == e)) {
                                     inspectedEntityID = e;
                                     showCharacterInspector = true;
                                 }
                                 ImGui::PopID();
                             }
                         }
                         ImGui::EndChild();
                         ImGui::EndTabItem();
                    }

                    // --- TAB 2: INVENTORY ---
                    if (ImGui::BeginTabItem("Inventory")) {
                        if (leaderEntity != -1) {
                            SectResourceComponent* res = reg.GetSectResource(leaderEntity);
                            if (res) {
                                ImGui::Text("Spirit Stones: %d", res->spirit_stones);
                                ImGui::Text("Herbs: %d", res->herbs);
                                ImGui::Text("Ores: %d", res->ores);
                                ImGui::Separator();
                                ImGui::Text("Stash:");
                                if (res->stash.empty()) ImGui::TextDisabled("(Empty)");
                                for (const auto& it : res->stash) {
                                    // Use same tooltip logic (simplified inline here or helper?)
                                    // Copied from Inventory Tab logic for now
                                    std::string name = it.item_id;
                                    std::string tooltipDesc = "";
                                    
                                    for(const auto& def : GameData::GetItems()) {
                                        if(def.id == it.item_id) { 
                                            name = def.name; 
                                            tooltipDesc = "Type: " + def.type + " | Grade: " + std::to_string(def.tier) + "\nValue: " + std::to_string(def.value) + "\n" + def.description;
                                            break; 
                                        }
                                    }
                                    ImGui::PushID(it.item_id.c_str());
                                    std::string label = name + " x" + std::to_string(it.count);
                                    
                                    if (ImGui::Selectable(label.c_str())) {}

                                    if (ImGui::IsItemHovered() && !tooltipDesc.empty()) {
                                        ImGui::SetTooltip("%s", tooltipDesc.c_str());
                                    }
                                    if (ImGui::BeginPopupContextItem()) {
                                        if (ImGui::Selectable("Exchange (Use CP)")) {
                                            if (inspectedEntityID != -1)
                                                SimEngine::Get().ExchangeFromSect(inspectedEntityID, it.item_id);
                                        }
                                        ImGui::EndPopup();
                                    }
                                    ImGui::PopID();
                                }
                            } else {
                                ImGui::TextColored(ImVec4(1,0,0,1), "No Resources Found (Leader Missing?)");
                            }
                        } else {
                            ImGui::Text("No Leader found.");
                        }
                        ImGui::EndTabItem();
                    }

                    // --- TAB 3: DIPLOMACY ---
                    if (ImGui::BeginTabItem("Diplomacy")) {
                         if (leaderEntity != -1) {
                             SectDiplomacyComponent* diplo = reg.GetSectDiplomacy(leaderEntity);
                             if (diplo) {
                                 ImGui::Columns(3, "DiploCols");
                                 ImGui::Text("Sect"); ImGui::NextColumn();
                                 ImGui::Text("Affinity"); ImGui::NextColumn();
                                 ImGui::Text("Status"); ImGui::NextColumn();
                                 ImGui::Separator();
                                 
                                 for(auto& [tid, rel] : diplo->relations) {
                                     // Find target sect name (slow scan)
                                     std::string tName = "Unknown Sect " + std::to_string(tid);
                                     // Quick hack: Scan all entities with SectComp rank=5 and sect_id=tid
                                     // Optimization: Cache this map later
                                     for(auto& [e2, sc2] : reg.sects) {
                                         if(sc2.sect_id == tid && sc2.rank == 5) {
                                             tName = sc2.name; 
                                             break;
                                         }
                                     }
                                     
                                     ImGui::Text("%s", tName.c_str()); ImGui::NextColumn();
                                     ImGui::Text("%d", rel.affinity); ImGui::NextColumn();
                                     ImGui::TextColored(rel.status == "Ally" ? ImVec4(0,1,0,1) : rel.status == "Hostile" ? ImVec4(1,0,0,1) : ImVec4(1,1,1,1), "%s", rel.status.c_str()); ImGui::NextColumn();
                                 }
                                 ImGui::Columns(1);
                             } else {
                                 ImGui::Text("No Diplomacy Data.");
                             }
                         }
                         ImGui::EndTabItem();
                    }

                     // --- TAB 4: QUESTS ---
                    if (ImGui::BeginTabItem("Quests")) {
                        if (leaderEntity != -1) {
                             SectResourceComponent* res = reg.GetSectResource(leaderEntity);
                             if (res) {
                                 if (res->mission_queue.empty()) ImGui::Text("No Active Quests.");
                                 
                                 int missionIdx = 0;
                                 for (auto it = res->mission_queue.begin(); it != res->mission_queue.end(); ) {
                                     auto& m = *it;
                                     ImGui::PushID(missionIdx);
                                     
                                     // Find Item Name
                                     std::string iName = m.required_item_id;
                                     if(m.required_item_id != "SS") {
                                          for(const auto& def : GameData::GetItems()) {
                                              if(def.id == m.required_item_id) { iName = def.name; break; }
                                          }
                                     } else {
                                         iName = "Spirit Stones";
                                     }
                                     
                                     ImGui::BulletText("** %s **", m.type.c_str());
                                     ImGui::Indent();
                                     ImGui::Text("Requires: %d x %s", m.required_count, iName.c_str());
                                     ImGui::TextColored(ImVec4(1, 0.84f, 0, 1), "Reward: %d CP", m.reward_contribution);
                                     
                                     // Check Player Status (assuming inspected entity is player for now, or allowed interactivity)
                                     bool canComplete = false;
                                     InventoryComponent* pInv = reg.GetInventory(inspectedEntityID);
                                     if (pInv) {
                                         if (m.required_item_id == "SS") {
                                             if (pInv->spirit_stones >= m.required_count) canComplete = true;
                                         } else {
                                             for(auto& pi : pInv->items) {
                                                 if (pi.item_id == m.required_item_id && pi.count >= m.required_count) { canComplete = true; break; }
                                             }
                                         }
                                     }
                                     
                                     if (canComplete) {
                                         if (ImGui::Button("Complete Mission")) {
                                             bool success = SimEngine::Get().CompleteMission(inspectedEntityID, leaderEntity, missionIdx);
                                             if (success) {
                                                 // Iterator invalidated, restart loop or break
                                                 ImGui::PopID();
                                                 break; 
                                             }
                                         }
                                     } else {
                                         ImGui::BeginDisabled();
                                         ImGui::Button("Complete Mission");
                                         ImGui::EndDisabled();
                                         if (ImGui::IsItemHovered()) ImGui::SetTooltip("Missing required items.");
                                     }
                                     
                                     ImGui::Unindent();
                                     ImGui::Separator();
                                     
                                     ImGui::PopID();
                                     missionIdx++;
                                     ++it;
                                 }
                                 
                                 // Debug Add
                                 if (ImGui::Button("Debug: Add Test Quest")) {
                                     res->mission_queue.push_back({"Testing", -1, 100, 1, true, "HERB_001", 5});
                                 }
                             }
                        }
                        ImGui::EndTabItem();
                    }

                    // --- TAB 5: DEAD MEMBERS ---
                    if (ImGui::BeginTabItem("Dead Members")) {
                         ImGui::Text("Ancestors & Fallen Disciples:");
                         ImGui::BeginChild("DeadList", ImVec2(0, 300));
                         for (auto& [e, memberSC] : reg.sects) {
                             if (memberSC.sect_id == selectedSectID) {
                                 StatsComponent* stats = reg.GetStats(e);
                                 if (stats && stats->is_dead) {
                                     RenderComponent* rc = reg.GetRenderable(e);
                                     CultivationComponent* cult = reg.GetCultivation(e);
                                     
                                     std::string name = rc ? rc->name : "Unknown";
                                     std::string rankStr = (memberSC.rank == 6) ? "[G.Eld]" : (memberSC.rank == 5) ? "[Ldr]" : (memberSC.rank == 4) ? "[Eld]" : (memberSC.rank == 3) ? "[Core]" : (memberSC.rank == 2) ? "[Inn]" : "[Out]";
                                     
                                     ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s %s (Age %d)", rankStr.c_str(), name.c_str(), cult ? cult->age : 0);
                                     ImGui::SameLine();
                                     ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Dead]");
                                     
                                     ImGui::BulletText("Cause: %s", stats->cause_of_death.c_str());
                                     if(stats->death_year > 0) { ImGui::SameLine(); ImGui::Text("(Year %d)", stats->death_year); }
                                     
                                     ImGui::BulletText("Location: x%d, y%d", stats->death_x, stats->death_y);
                                     ImGui::Separator();
                                 }
                             }
                         }
                         ImGui::EndChild();
                         ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            } else {
                ImGui::Text("Select a Sect.");
            }
            ImGui::EndGroup();
        }
        ImGui::End();



        
        // Debug Map View (Refactored)

        
        // Debug Map View (Refactored)
        if (!SimEngine::Get().IsSimulating()) {
            if (ImGui::TreeNode("World Map")) {
                static int currentLayerIndex = 0;
                ImGui::SliderInt("Layer Base", &currentLayerIndex, -9, 9);
                
                static float zoom = 12.0f; // Bigger tiles
                ImGui::SliderFloat("Zoom", &zoom, 4.0f, 32.0f);
                
                MapLayer* currentLayer = world.GetLayer(currentLayerIndex);
                
                if (currentLayer) {
                    ImGui::Text("Viewing: %s (%dx%d)", currentLayer->layer_name.c_str(), currentLayer->width, currentLayer->height);
                    
                    // Scrollable Container: Use "MapRegion"
                    ImGui::BeginChild("MapRegion", ImVec2(0, 500), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);
                    
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    ImVec2 p = ImGui::GetCursorScreenPos();
                    ImVec2 size = ImGui::GetWindowSize();
                    float scrollX = ImGui::GetScrollX();
                    float scrollY = ImGui::GetScrollY();
                    
                    // --- 1. Draw Terrain (Culling & LOD) ---
                    // Calculate visible range
                    int startX = (int)(scrollX / zoom);
                    int startY = (int)(scrollY / zoom);
                    int endX = startX + (int)(size.x / zoom) + 2;
                    int endY = startY + (int)(size.y / zoom) + 2;
                    
                    // Clamp
                    if (startX < 0) startX = 0;
                    if (startY < 0) startY = 0;
                    if (endX > currentLayer->width) endX = currentLayer->width;
                    if (endY > currentLayer->height) endY = currentLayer->height;
                    
                    // LOD Calculation: Limit to ~10,000 tiles to prevent 16-bit vertex overflow
                    int visibleTiles = (endX - startX) * (endY - startY);
                    int step = 1;
                    if (visibleTiles > 10000) {
                        step = (int)std::ceil(std::sqrt((float)visibleTiles / 10000.0f));
                    }
                    
                    for (int y = startY; y < endY; y += step) {
                        for (int x = startX; x < endX; x += step) {
                            int idx = y * currentLayer->width + x;
                            const auto& tile = currentLayer->tiles[idx];
                            
                            ImU32 col = IM_COL32(255, 0, 255, 255); 
                            
                            // Colors matches Biome List logic
                            if (tile.type == "DEEP_WATER") col = IM_COL32(0, 0, 139, 255);
                            else if (tile.type == "WATER") col = IM_COL32(0, 0, 255, 255);
                            else if (tile.type == "SAND") col = IM_COL32(238, 214, 175, 255);
                            else if (tile.type == "GRASS") col = IM_COL32(34, 139, 34, 255);
                            else if (tile.type == "FOREST") col = IM_COL32(0, 100, 0, 255);
                            else if (tile.type == "MOUNTAIN") col = IM_COL32(105, 105, 105, 255);
                            else if (tile.type == "SNOW") col = IM_COL32(255, 250, 250, 255);
                            else if (tile.type == "VOLCANO") col = IM_COL32(255, 69, 0, 255);
                            else if (tile.type == "SWAMP") col = IM_COL32(85, 107, 47, 255); 
                            
                            if (!tile.poi_type.empty()) col = (col & 0x00FFFFFF) | 0xFF000000;
                            
                            float drawX = p.x + x * zoom;
                            float drawY = p.y + y * zoom;
                            float drawSize = zoom * step;
                            
                            // Fill the gap if step > 1
                            draw_list->AddRectFilled(ImVec2(drawX, drawY), ImVec2(drawX + drawSize, drawY + drawSize), col);
                        }
                    }

                // --- 2. Render Entities & Resource Nodes ---
                auto& registry = engine.GetRegistry();
                ImVec2 mousePos = ImGui::GetMousePos();
                
                std::string closestTooltip = "";
                int closestEntityID = -1;
                float minDistance = 20.0f; 
                
                for (auto& [id, render] : registry.renderables) {
                    TransformComponent* entityTC = registry.GetTransform(id);
                    LocationComponent* entityLoc = registry.GetLocation(id);
                    
                    if (entityTC && entityTC->z == currentLayerIndex) { 
                        // VISIBILITY CHECK
                        if (render.visible_on_map == false) continue;
                        if (entityLoc && entityLoc->is_inside) continue; 
                        
                         // Check Bounds
                         if (entityTC->x < startX || entityTC->x >= endX || entityTC->y < startY || entityTC->y >= endY) continue;
                         
                         float x = p.x + entityTC->x * zoom;
                         float y = p.y + entityTC->y * zoom;
                         float cx = x + zoom/2;
                         float cy = y + zoom/2;
                         
                         if (render.symbol == '^') { 
                             draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x+zoom, y+zoom), render.color);
                             draw_list->AddRect(ImVec2(x, y), ImVec2(x+zoom, y+zoom), IM_COL32(0,0,0,255));
                             draw_list->AddText(ImVec2(x + zoom + 2, y), IM_COL32(255, 255, 255, 255), render.name.c_str());
                         } else {
                             draw_list->AddCircleFilled(ImVec2(cx, cy), zoom/2 - 1, render.color);
                         }
                         
                         float dx = mousePos.x - cx;
                         float dy = mousePos.y - cy;
                         float dist = std::sqrt(dx*dx + dy*dy);
                         
                         if (dist < minDistance) {
                             minDistance = dist;
                             closestTooltip = render.name;
                             closestEntityID = id;
                         }
                    }
                }
                
                if (closestEntityID != -1) {
                    // Check for Residents
                    std::string residents = "";
                    int residentCount = 0;
                    
                    // Iterate to find who is inside
                    for (auto& [eid, loc] : registry.locations) {
                        if (loc.is_inside && loc.location_entity_id == closestEntityID) {
                            if (residentCount < 10) { // Limit display
                                RenderComponent* rc = registry.GetRenderable(eid);
                                if (rc) residents += "\n - " + rc->name;
                            }
                            residentCount++;
                        }
                    }
                    
                    if (residentCount > 0) {
                        closestTooltip += "\n\nResidents (" + std::to_string(residentCount) + "):" + residents;
                        if (residentCount > 10) closestTooltip += "\n...and " + std::to_string(residentCount - 10) + " more.";
                    }
                    
                    ImGui::SetTooltip("%s", closestTooltip.c_str());
                    draw_list->AddCircle(mousePos, 10.0f, IM_COL32(255, 255, 0, 255));
                } else if (ImGui::IsWindowHovered()) {
                     // Tile Tooltip (Only if window is hovered)
                    float relX = mousePos.x - p.x;
                    float relY = mousePos.y - p.y;
                    
                    int tx = (int)(relX / zoom);
                    int ty = (int)(relY / zoom);
                    if (tx >= 0 && tx < currentLayer->width && ty >= 0 && ty < currentLayer->height) {
                         const auto& t = currentLayer->tiles[ty * currentLayer->width + tx];
                         
                         ImGui::BeginTooltip();
                         ImGui::Text("Biome: %s", t.type.c_str());
                         ImGui::Text("Qi Density: %.1f", t.qi_density);
                         ImGui::Text("Coords: %d, %d", tx, ty);
                         if (!t.poi_type.empty()) ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "POI: %s", t.poi_type.c_str());
                         if (!t.hazard.empty()) ImGui::TextColored(ImVec4(1, 0, 0, 1), "Hazard: %s", t.hazard.c_str());
                         ImGui::EndTooltip();
                    }
                }
                
                if (!closestTooltip.empty()) {
                     // Find the entity ID that matches this name/location? 
                     // We iterated above but didn't save the ID.
                     // Let's redo the loop or save "hoveredEntityID".
                     // Better: Modify the loop above to save hoveredEntityID.
                     
                     // ACTUALLY, I will just append residents info here if I can find the ID.
                     // The loop above is purely for rendering and simple hit test.
                     // Let's change the loop above (Step 76) to store `hoveredEntityID`.
                }

                
                // Add dummy to define local scrollable area size
                ImGui::SetCursorPos(ImVec2(currentLayer->width * zoom, currentLayer->height * zoom)); 
                ImGui::Dummy(ImVec2(1,1));
                
                ImGui::EndChild();
                
                ImGui::Separator();
                
                // Legend
                if (ImGui::CollapsingHeader("Map Legend")) {
                    ImGui::Text("Biomes:");
                    ImGui::ColorButton("##Grass", ImVec4(34/255.f, 139/255.f, 34/255.f, 1.f)); ImGui::SameLine(); ImGui::Text("Grass"); ImGui::SameLine();
                    ImGui::ColorButton("##Forest", ImVec4(0, 100/255.f, 0, 1.f)); ImGui::SameLine(); ImGui::Text("Forest"); ImGui::SameLine();
                    ImGui::ColorButton("##Mtn", ImVec4(105/255.f, 105/255.f, 105/255.f, 1.f)); ImGui::SameLine(); ImGui::Text("Mountain");
                    
                    ImGui::ColorButton("##Snow", ImVec4(1, 1, 1, 1)); ImGui::SameLine(); ImGui::Text("Snow"); ImGui::SameLine();
                    ImGui::ColorButton("##Volcano", ImVec4(1, 69/255.f, 0, 1.f)); ImGui::SameLine(); ImGui::Text("Volcano"); ImGui::SameLine();
                    ImGui::ColorButton("##Swamp", ImVec4(85/255.f, 107/255.f, 47/255.f, 1.f)); ImGui::SameLine(); ImGui::Text("Swamp");

                    ImGui::Separator();
                    ImGui::Text("Resources:");
                    ImGui::ColorButton("##SS", ImVec4(0, 1, 1, 1)); ImGui::SameLine(); ImGui::Text("Spirit Stone Mine (Cyan #)");
                    ImGui::ColorButton("##Herb", ImVec4(1, 0, 0, 1)); ImGui::SameLine(); ImGui::Text("Spirit Herb (Red %)");
                    ImGui::ColorButton("##Ore", ImVec4(0.5f, 0.5f, 0.5f, 1)); ImGui::SameLine(); ImGui::Text("Common Ore (Grey o)");
                }
            }
            ImGui::TreePop();
            }
        }
        ImGui::End();
    } // End Playing State Check
        
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
