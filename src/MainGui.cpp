// --- Cultivation Fortress: SFML/ImGui graphical front-end ---
// Renders the Perlin-noise overworld and provides menu-driven control.
// All game logic lives in cfs_core (SimEngine); this file is presentation only.
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "SimEngine.h"
#include "HistoryGenerator.h"
#include "GameData.h"
#include "Skills.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

constexpr float TILE = 8.0f; // World units per map tile

enum class Screen { MainMenu, Embark, Playing };

struct UiState {
    Screen screen = Screen::MainMenu;

    // Selection
    int selected_citizen = -1;
    int selected_candidate = 0;
    std::set<int> party;
    int mission_type = 0;
    int mission_target = 0;

    // Event log
    std::vector<std::string> log;
    bool log_dirty = false;

    // Time control: fixed pace - one in-game year (360 days) per real hour
    bool paused = true;
    static constexpr float SPEED = 360.0f / 3600.0f; // 0.1 days per second
    float accum = 0.0f;

    // After-action reports
    int reports_seen = 0;
    bool show_reports = false;
    int report_view = -1;

    // Menus
    bool show_game_menu = false;
    char save_name[64] = "Slot_1";
    std::string menu_feedback;

    // Camera
    sf::Vector2f cam{0.f, 0.f};
    float zoom = 2.0f;
    sf::Vector2i last_mouse{0, 0};

    // Cached map geometry
    sf::VertexArray map_va{sf::PrimitiveType::Triangles};
    unsigned map_built_seed = 0;
};

std::string RealmName(int major) {
    switch (major) {
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

sf::Color BiomeColor(Biome b, float qi) {
    sf::Color c;
    switch (b) {
        case Biome::DeepWater: c = sf::Color(14, 40, 92); break;
        case Biome::Water:     c = sf::Color(30, 80, 160); break;
        case Biome::Sand:      c = sf::Color(212, 198, 148); break;
        case Biome::Grass:     c = sf::Color(86, 142, 62); break;
        case Biome::Forest:    c = sf::Color(36, 96, 44); break;
        case Biome::Swamp:     c = sf::Color(72, 92, 56); break;
        case Biome::Hills:     c = sf::Color(132, 122, 92); break;
        case Biome::Mountain:  c = sf::Color(122, 122, 128); break;
        case Biome::Snow:      c = sf::Color(234, 238, 244); break;
    }
    // Spirit-rich land shimmers faintly violet
    if (qi > 0.62f) {
        float t = std::min(1.0f, (qi - 0.62f) * 1.6f) * 0.35f;
        c.r = (uint8_t)(c.r + (180 - c.r) * t);
        c.g = (uint8_t)(c.g + (140 - c.g) * t);
        c.b = (uint8_t)(c.b + (255 - c.b) * t);
    }
    return c;
}

void BuildMapVA(UiState& ui, GameMap& map) {
    ui.map_va.clear();
    ui.map_va.setPrimitiveType(sf::PrimitiveType::Triangles);
    ui.map_va.resize((size_t)map.Width() * map.Height() * 6);
    size_t v = 0;
    for (int y = 0; y < map.Height(); ++y) {
        for (int x = 0; x < map.Width(); ++x) {
            const WorldTile& t = map.At(x, y);
            sf::Color c = BiomeColor((Biome)t.biome, t.qi);
            float px = x * TILE, py = y * TILE;
            sf::Vector2f p0{px, py}, p1{px + TILE, py}, p2{px + TILE, py + TILE}, p3{px, py + TILE};
            ui.map_va[v].position = p0; ui.map_va[v++].color = c;
            ui.map_va[v].position = p1; ui.map_va[v++].color = c;
            ui.map_va[v].position = p2; ui.map_va[v++].color = c;
            ui.map_va[v].position = p0; ui.map_va[v++].color = c;
            ui.map_va[v].position = p2; ui.map_va[v++].color = c;
            ui.map_va[v].position = p3; ui.map_va[v++].color = c;
        }
    }
    ui.map_built_seed = map.Seed();
}

void DrainLog(SimEngine& eng, UiState& ui) {
    for (auto& line : eng.RecentLog()) ui.log.push_back(line);
    if (!eng.RecentLog().empty()) ui.log_dirty = true;
    eng.RecentLog().clear();
    if (ui.log.size() > 4000) ui.log.erase(ui.log.begin(), ui.log.begin() + (ui.log.size() - 4000));
}

std::vector<std::string> ListSaves() {
    std::vector<std::string> out;
    if (!fs::exists("SaveData")) return out;
    for (const auto& e : fs::directory_iterator("SaveData"))
        if (e.is_regular_file() && e.path().extension() == ".json")
            out.push_back(e.path().stem().string());
    std::sort(out.begin(), out.end());
    return out;
}

void CenterCameraOnTown(SimEngine& eng, UiState& ui) {
    ui.cam = {(eng.Map().town_x + 0.5f) * TILE, (eng.Map().town_y + 0.5f) * TILE};
    ui.zoom = 3.0f;
}

void EnterPlaying(SimEngine& eng, UiState& ui) {
    if (eng.Map().IsGenerated() && ui.map_built_seed != eng.Map().Seed())
        BuildMapVA(ui, eng.Map());
    CenterCameraOnTown(eng, ui);
    DrainLog(eng, ui);
    ui.screen = Screen::Playing;
    ui.paused = true;
    ui.selected_citizen = -1;
    ui.party.clear();
    ui.show_game_menu = false;
    ui.reports_seen = (int)eng.MissionReports().size(); // Don't replay old reports on load
    ui.show_reports = false;
    ui.report_view = -1;
}

// "♂ " / "♀ " prefix (glyphs merged from Segoe UI Symbol at startup)
const char* GenderSymbol(Registry& reg, int eid) {
    auto it = reg.relations.find(eid);
    if (it == reg.relations.end()) return "";
    return it->second.gender == "Male" ? "\xE2\x99\x82 " : "\xE2\x99\x80 ";
}

// ---------------------------------------------------------------------------
// Character detail rendering (shared by Embark + Inspector)
// ---------------------------------------------------------------------------

// Rich tooltip for one skill: XP progress, what it does, what levels give, how to train.
void SkillTooltip(Registry& reg, int eid, const std::string& skill, int lvl) {
    int xp = 0;
    auto sk = reg.skill_sets.find(eid);
    if (sk != reg.skill_sets.end()) {
        auto x = sk->second.xp.find(skill);
        if (x != sk->second.xp.end()) xp = x->second;
    }
    int need = SkillXpToNext(lvl);

    ImGui::BeginTooltip();
    ImGui::TextColored(ImVec4(0.95f, 0.8f, 0.4f, 1), "%s - Level %d", skill.c_str(), lvl);
    if (lvl >= SKILL_CAP) {
        ImGui::TextColored(ImVec4(0.4f, 0.9f, 1, 1), "MASTERED");
    } else {
        ImGui::Text("XP to level %d:", lvl + 1);
        ImGui::SameLine();
        ImGui::ProgressBar((float)xp / (float)need, ImVec2(140, 14),
                           (std::to_string(xp) + "/" + std::to_string(need)).c_str());
    }
    auto meta = SkillMetadata().find(skill);
    if (meta != SkillMetadata().end()) {
        ImGui::Separator();
        ImGui::PushTextWrapPos(320);
        ImGui::TextWrapped("%s", meta->second.description.c_str());
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.5f, 0.9f, 0.5f, 1), "Effect:");
        ImGui::TextWrapped("%s", meta->second.effect_per_level.c_str());
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1), "Training:");
        ImGui::TextWrapped("%s", meta->second.how_to_train.c_str());
        ImGui::PopTextWrapPos();
    }
    ImGui::EndTooltip();
}

// One skill row (label + level bar) with the tooltip on hover.
void DrawSkillRow(Registry& reg, int eid, const std::string& name, int lvl) {
    ImGui::Text("  %-16s", name.c_str());
    bool hovered = ImGui::IsItemHovered();
    ImGui::SameLine();
    ImGui::ProgressBar((float)lvl / (float)SKILL_CAP, ImVec2(120, 14), std::to_string(lvl).c_str());
    hovered |= ImGui::IsItemHovered();
    if (hovered) SkillTooltip(reg, eid, name, lvl);
}

void DrawPersonalityAndSkills(Registry& reg, int eid) {
    auto pc = reg.personalities.find(eid);
    if (pc != reg.personalities.end()) {
        auto& p = pc->second;
        std::vector<std::pair<std::string, int>> traits = {
            {"Ambition", p.ambition}, {"Greed", p.greed}, {"Bravery", p.bravery}, {"Loyalty", p.loyalty},
            {"Ruthlessness", p.ruthlessness}, {"Caution", p.caution}, {"Wisdom", p.wisdom},
            {"Social", p.social}, {"Vengeance", p.vengeance}, {"Diligence", p.diligence}};
        std::sort(traits.begin(), traits.end(), [](auto& a, auto& b) { return a.second > b.second; });
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.5f, 1), "Personality");
        for (int i = 0; i < 4; ++i)
            ImGui::Text("  %s: %d", traits[i].first.c_str(), traits[i].second);
    }
    auto sk = reg.skill_sets.find(eid);
    if (sk != reg.skill_sets.end()) {
        std::vector<std::pair<std::string, int>> all(sk->second.skills.begin(), sk->second.skills.end());
        std::sort(all.begin(), all.end(), [](auto& a, auto& b) { return a.second > b.second; });
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 0.8f, 1), "Skills  (hover for details)");
        for (auto& [name, lvl] : all) {
            if (lvl <= 0) continue;
            DrawSkillRow(reg, eid, name, lvl);
        }
    }
}

// Hover card for a person: skills, weapon, inventory, condition.
void PersonTooltip(SimEngine& eng, int eid) {
    Registry& reg = eng.GetRegistry();
    if (!reg.renderables.count(eid)) return;
    ImGui::BeginTooltip();
    ImGui::TextColored(ImVec4(0.95f, 0.8f, 0.4f, 1), "%s%s", GenderSymbol(reg, eid),
                       reg.renderables[eid].name.c_str());
    if (!reg.renderables[eid].title.empty()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.85f, 0.6f, 1.0f, 1), "\"%s\"", reg.renderables[eid].title.c_str());
    }
    auto cc = reg.cultivations.find(eid);
    if (cc != reg.cultivations.end())
        ImGui::Text("Age %d  |  Talent %.2f", cc->second.age, cc->second.talent);

    auto sk = reg.skill_sets.find(eid);
    if (sk != reg.skill_sets.end()) {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 0.8f, 1), "Skills");
        std::vector<std::pair<int, std::string>> all;
        for (auto& [name, lvl] : sk->second.skills)
            if (lvl > 0) all.push_back({lvl, name});
        std::sort(all.rbegin(), all.rend());
        int shown = 0;
        for (auto& [lvl, name] : all) {
            if (shown++ >= 6) break;
            int xp = sk->second.xp.count(name) ? sk->second.xp.at(name) : 0;
            ImGui::Text("  %-16s %d  (%d/%d xp)", name.c_str(), lvl, xp, SkillXpToNext(lvl));
        }
        if (all.empty()) ImGui::TextDisabled("  (unskilled)");
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 0.8f, 1), "Inventory");
    ImGui::Text("  Weapon: %s", eng.EquippedWeaponName(eid).c_str());
    auto inv = reg.inventories.find(eid);
    if (inv != reg.inventories.end()) {
        ImGui::Text("  Spirit stones: %d", inv->second.spirit_stones);
        for (const auto& it : inv->second.items) {
            if (it.equipped) continue;
            const ItemDefinition* def = GameData::FindItem(it.item_id);
            ImGui::Text("  %s x%d", def ? def->name.c_str() : it.item_id.c_str(), it.count);
        }
    }

    auto body = reg.bodies.find(eid);
    if (body != reg.bodies.end()) {
        std::string s = Body::Summarize(body->second);
        if (s != "unharmed") {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1, 0.55f, 0.3f, 1), "Condition: %s", s.c_str());
        }
    }
    ImGui::EndTooltip();
}

void DrawBodyTree(Registry& reg, int eid) {
    auto body = reg.bodies.find(eid);
    if (body == reg.bodies.end()) { ImGui::Text("No anatomy data."); return; }
    const BodyComponent& b = body->second;

    float blood_pct = b.max_blood > 0 ? b.blood / b.max_blood : 1.0f;
    ImGui::Text("Blood:");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, blood_pct > 0.5f ? ImVec4(0.7f, 0.1f, 0.1f, 1) : ImVec4(1, 0, 0, 1));
    ImGui::ProgressBar(blood_pct, ImVec2(140, 14));
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("Pain: %d/100", (int)b.pain);
    ImGui::Separator();

    std::vector<int> depth(b.parts.size(), 0);
    for (size_t i = 0; i < b.parts.size(); ++i)
        if (b.parts[i].parent >= 0) depth[i] = depth[b.parts[i].parent] + 1;

    for (size_t i = 0; i < b.parts.size(); ++i) {
        const BodyPart& p = b.parts[i];
        ImVec4 col(0.65f, 0.85f, 0.65f, 1.0f);
        std::string status;
        if (p.IsGone()) {
            col = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);
            status = " [" + WoundLevelName(p.Wound()) + "]";
        } else if (p.wound >= (int)WoundLevel::Broken) {
            col = ImVec4(1.0f, 0.55f, 0.2f, 1.0f);
            status = " [" + WoundLevelName(p.Wound()) + " " + std::to_string(p.hp) + "/" + std::to_string(p.max_hp) + "]";
        } else if (p.wound != (int)WoundLevel::Fine) {
            col = ImVec4(0.95f, 0.9f, 0.3f, 1.0f);
            status = " [" + WoundLevelName(p.Wound()) + " " + std::to_string(p.hp) + "/" + std::to_string(p.max_hp) + "]";
        }
        if (p.bleed > 0.5f) status += " (bleeding)";
        std::string pad((size_t)depth[i] * 2, ' ');
        ImGui::TextColored(col, "%s%s%s", pad.c_str(), p.name.c_str(), status.c_str());
    }
}

// ---------------------------------------------------------------------------
// Screens
// ---------------------------------------------------------------------------

void DrawMainMenu(sf::RenderWindow& window, SimEngine& eng, UiState& ui) {
    ImVec2 ds = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(ds.x * 0.5f, ds.y * 0.42f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(420, 0), ImGuiCond_Always);
    ImGui::Begin("##MainMenu", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::SetWindowFontScale(2.2f);
    ImGui::TextColored(ImVec4(0.95f, 0.8f, 0.4f, 1), "Cultivation Fortress");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextDisabled("A settlement simulation of flesh and qi");
    ImGui::Spacing(); ImGui::Spacing();

    if (ImGui::Button("New Game", ImVec2(-1, 44))) {
        eng.NewGame();
        ui.selected_candidate = 0;
        ui.log.clear();
        ui.screen = Screen::Embark;
    }
    ImGui::Spacing();
    if (ImGui::Button("Load Game", ImVec2(-1, 44))) ImGui::OpenPopup("LoadPopup");
    ImGui::Spacing();
    if (ImGui::Button("Exit", ImVec2(-1, 44))) window.close();

    if (ImGui::BeginPopup("LoadPopup")) {
        auto saves = ListSaves();
        if (saves.empty()) ImGui::TextDisabled("No saves found.");
        for (auto& s : saves) {
            if (ImGui::Selectable(s.c_str())) {
                if (eng.LoadGame(s)) {
                    ui.log.clear();
                    EnterPlaying(eng, ui);
                } else {
                    ui.menu_feedback = "Failed to load " + s;
                }
            }
        }
        ImGui::EndPopup();
    }
    if (!ui.menu_feedback.empty()) ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "%s", ui.menu_feedback.c_str());
    ImGui::End();
}

void DrawEmbark(SimEngine& eng, UiState& ui) {
    Registry& reg = eng.GetRegistry();
    ImVec2 ds = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(ds.x * 0.5f, ds.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(std::min(ds.x - 40.f, 1100.f), std::min(ds.y - 40.f, 720.f)), ImGuiCond_Always);
    ImGui::Begin("Choose Your Ten Founders", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::TextWrapped("Ten souls stand ready to found a settlement in the wilds. Inspect them, reroll any you dislike, then embark.");
    ImGui::Spacing();

    // Left: candidate table
    ImGui::BeginChild("CandList", ImVec2(560, -48), ImGuiChildFlags_Borders);
    if (ImGui::BeginTable("Cands", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Sex", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupColumn("Age", ImGuiTableColumnFlags_WidthFixed, 40);
        ImGui::TableSetupColumn("Talent", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupColumn("Best Skill", ImGuiTableColumnFlags_WidthFixed, 140);
        ImGui::TableHeadersRow();

        const auto& cands = eng.Candidates();
        for (int i = 0; i < (int)cands.size(); ++i) {
            int eid = cands[i];
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(i);
            std::string cname = std::string(GenderSymbol(reg, eid)) + reg.renderables[eid].name;
            if (ImGui::Selectable(cname.c_str(), ui.selected_candidate == i,
                                  ImGuiSelectableFlags_SpanAllColumns))
                ui.selected_candidate = i;
            ImGui::PopID();
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", reg.relations[eid].gender.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%d", reg.cultivations[eid].age);
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.2f", reg.cultivations[eid].talent);
            ImGui::TableSetColumnIndex(4);
            // Best skill
            std::string best = "-";
            int best_v = 0;
            for (auto& [n, v] : reg.skill_sets[eid].skills)
                if (v > best_v) { best_v = v; best = n + " " + std::to_string(v); }
            ImGui::Text("%s", best.c_str());
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();

    // Right: selected candidate details
    ImGui::SameLine();
    ImGui::BeginChild("CandDetail", ImVec2(0, -48), ImGuiChildFlags_Borders);
    const auto& cands = eng.Candidates();
    if (ui.selected_candidate >= 0 && ui.selected_candidate < (int)cands.size()) {
        int eid = cands[ui.selected_candidate];
        ImGui::SetWindowFontScale(1.3f);
        ImGui::TextColored(ImVec4(0.95f, 0.8f, 0.4f, 1), "%s", reg.renderables[eid].name.c_str());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("%s, age %d  |  Talent %.2f  |  Lifespan ~%d",
                    reg.relations[eid].gender.c_str(), reg.cultivations[eid].age,
                    reg.cultivations[eid].talent, reg.cultivations[eid].lifespan);
        ImGui::Separator();
        DrawPersonalityAndSkills(reg, eid);
    } else {
        ImGui::TextDisabled("Select a founder to inspect them.");
    }
    ImGui::EndChild();

    // Bottom buttons
    if (ImGui::Button("Reroll Selected", ImVec2(160, 32)))
        eng.RerollCandidate(ui.selected_candidate);
    ImGui::SameLine();
    if (ImGui::Button("Reroll All", ImVec2(160, 32)))
        eng.RerollCandidates();
    ImGui::SameLine();
    ImGui::SameLine(ImGui::GetWindowWidth() - 240);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.2f, 1));
    if (ImGui::Button("EMBARK", ImVec2(220, 32))) {
        eng.StartGame();
        EnterPlaying(eng, ui);
    }
    ImGui::PopStyleColor();
    ImGui::End();
}

// --- Playing screen panels ---

void DrawTopBar(SimEngine& eng, UiState& ui) {
    Registry& reg = eng.GetRegistry();
    auto& town = reg.towns[eng.TownID()];
    auto& res = reg.sect_resources[eng.TownID()];
    ImVec2 ds = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(ds.x, 0));
    ImGui::Begin("##TopBar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::TextColored(ImVec4(0.95f, 0.8f, 0.4f, 1), "%s", town.name.c_str());
    ImGui::SameLine();
    ImGui::Text("| %s", eng.GetDateString().c_str());
    ImGui::SameLine(0, 24);

    if (ui.paused) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.55f, 0.2f, 1));
        if (ImGui::Button(" Play ")) ui.paused = false;
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.45f, 0.1f, 1));
        if (ImGui::Button("Pause")) ui.paused = true;
        ImGui::PopStyleColor();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("One in-game year passes per real hour.");
    ImGui::SameLine();
    if (ImGui::Button("Reports")) ui.show_reports = !ui.show_reports;
    ImGui::SameLine(0, 24);
    if (ImGui::Button("Menu (Esc)")) ui.show_game_menu = !ui.show_game_menu;

    ImGui::Text("Pop %d   Morale %d   Defense %d", town.population, town.morale, town.defense_rating);
    ImGui::SameLine(0, 24);
    ImGui::TextColored(ImVec4(0.7f, 0.9f, 0.5f, 1), "Food %d", town.food_reserves);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.8f, 0.65f, 0.4f, 1), "Wood %d", town.wood);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.75f, 1), "Stone %d", town.stone);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.9f, 0.6f, 1), "Herbs %d", res.herbs);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1), "Ore %d", res.ores);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1), "Spirit Stones %d", res.spirit_stones);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.95f, 0.85f, 0.3f, 1), "Wealth %d", town.wealth);

    int mayor = town.mayor_id;
    ImGui::SameLine(0, 24);
    if (mayor != -1 && reg.renderables.count(mayor))
        ImGui::Text("Mayor: %s", reg.renderables[mayor].name.c_str());
    else
        ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "No Mayor!");

    ImGui::End();
}

void DrawCitizens(SimEngine& eng, UiState& ui) {
    Registry& reg = eng.GetRegistry();
    ImGui::SetNextWindowPos(ImVec2(10, 90), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(380, 420), ImGuiCond_FirstUseEver);
    ImGui::Begin("Cultivators");

    auto people = eng.LivingPeople();
    if (ImGui::BeginTable("People", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerH)) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Age", ImGuiTableColumnFlags_WidthFixed, 36);
        ImGui::TableSetupColumn("Job", ImGuiTableColumnFlags_WidthFixed, 84);
        ImGui::TableSetupColumn("Health", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableHeadersRow();

        for (int eid : people) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(eid);
            bool selected = ui.selected_citizen == eid;
            std::string name = std::string(GenderSymbol(reg, eid)) + reg.renderables[eid].name;
            auto cc = reg.cultivations.find(eid);
            if (cc != reg.cultivations.end() && cc->second.realm_major > 0) name += " *";
            if (ImGui::Selectable(name.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
                ui.selected_citizen = eid;
            if (ImGui::IsItemHovered()) PersonTooltip(eng, eid);
            ImGui::PopID();

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", cc != reg.cultivations.end() ? cc->second.age : 0);

            ImGui::TableSetColumnIndex(2);
            if (eng.IsOnMission(eid)) ImGui::TextColored(ImVec4(0.5f, 0.7f, 1, 1), "Mission");
            else ImGui::Text("%s", reg.jobs.count(eid) ? reg.jobs[eid].job.c_str() : "-");

            ImGui::TableSetColumnIndex(3);
            auto body = reg.bodies.find(eid);
            if (body != reg.bodies.end()) {
                std::string s = Body::Summarize(body->second);
                if (s == "unharmed") ImGui::TextColored(ImVec4(0.5f, 0.85f, 0.5f, 1), "fine");
                else {
                    ImGui::TextColored(ImVec4(1, 0.5f, 0.3f, 1), "hurt");
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", s.c_str());
                }
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void DrawInspector(SimEngine& eng, UiState& ui) {
    if (ui.selected_citizen == -1) return;
    Registry& reg = eng.GetRegistry();
    int eid = ui.selected_citizen;
    if (!reg.renderables.count(eid)) { ui.selected_citizen = -1; return; }

    ImGui::SetNextWindowPos(ImVec2(10, 520), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(420, 360), ImGuiCond_FirstUseEver);
    bool open = true;
    ImGui::Begin(("Inspector: " + reg.renderables[eid].name + "###Inspector").c_str(), &open);
    if (!open) { ui.selected_citizen = -1; ImGui::End(); return; }

    auto st = reg.stats.find(eid);
    if (st != reg.stats.end() && st->second.is_dead) {
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "DECEASED (Year %d): %s",
                           st->second.death_year, st->second.cause_of_death.c_str());
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("InspTabs")) {
        if (ImGui::BeginTabItem("Overview")) {
            auto& cc = reg.cultivations[eid];
            auto& rel = reg.relations[eid];
            if (!reg.renderables[eid].title.empty())
                ImGui::TextColored(ImVec4(0.85f, 0.6f, 1.0f, 1), "\"%s\"", reg.renderables[eid].title.c_str());
            ImGui::Text("%s, age %d / %d", rel.gender.c_str(), cc.age, cc.lifespan);
            ImGui::Text("Realm: %s %d", RealmName(cc.realm_major).c_str(), cc.realm_minor);
            ImGui::Text("Qi:");
            ImGui::SameLine();
            ImGui::ProgressBar(cc.max_qi > 0 ? cc.qi / cc.max_qi : 0, ImVec2(150, 14),
                               (std::to_string((int)cc.qi) + "/" + std::to_string((int)cc.max_qi)).c_str());
            ImGui::Text("Talent: %.2f", cc.talent);
            auto needs = reg.needs.find(eid);
            if (needs != reg.needs.end())
                ImGui::Text("Morale: %d/100   Hunger: %d days", needs->second.morale, needs->second.hunger);
            ImGui::Text("Attack: %d   Defense: %d", eng.EffectiveAttack(eid), eng.EffectiveDefense(eid));
            if (eng.IsOnMission(eid)) ImGui::TextColored(ImVec4(0.5f, 0.7f, 1, 1), "Currently away on a mission.");
            ImGui::Separator();

            // Tasks are auto-assigned by the labor system (Town panel sets priorities)
            std::string cur_job = reg.jobs.count(eid) ? reg.jobs[eid].job : "Idle";
            ImGui::Text("Task: %s", cur_job.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("(auto-assigned by labor priorities)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Set labor priorities in the Town panel.\nThe best-skilled available people take each task.\nThe unassigned cultivate or lounge, by personality.");

            // Weapon assignment
            std::string weapon = eng.EquippedWeaponName(eid);
            ImGui::Text("Weapon: %s", weapon.c_str());
            ImGui::SameLine();
            if (ImGui::SmallButton("Change##weapon")) ImGui::OpenPopup("WeaponPopup");
            if (ImGui::BeginPopup("WeaponPopup")) {
                if (ImGui::Selectable("(bare hands)")) {
                    ui.log.push_back(eng.UnassignWeapon(eid));
                    ui.log_dirty = true;
                }
                auto& stash = reg.sect_resources[eng.TownID()].stash;
                for (int i = 0; i < (int)stash.size(); ++i) {
                    const ItemDefinition* def = GameData::FindItem(stash[i].item_id);
                    std::string label = (def ? def->name : stash[i].item_id) +
                                        (def ? "  (Atk +" + std::to_string(def->attack) + ")" : "");
                    ImGui::PushID(i);
                    if (ImGui::Selectable(label.c_str())) {
                        ui.log.push_back(eng.AssignWeapon(eid, stash[i].item_id));
                        ui.log_dirty = true;
                    }
                    ImGui::PopID();
                }
                if (stash.empty()) ImGui::TextDisabled("Armory is empty.");
                ImGui::EndPopup();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Body")) {
            ImGui::BeginChild("BodyScroll");
            DrawBodyTree(reg, eid);
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Mind & Skills")) {
            ImGui::BeginChild("SkillScroll");
            DrawPersonalityAndSkills(reg, eid);
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Relations")) {
            auto& rel = reg.relations[eid];
            if (rel.spouse_id != -1 && reg.renderables.count(rel.spouse_id))
                ImGui::Text("Spouse: %s", reg.renderables[rel.spouse_id].name.c_str());
            if (!rel.children.empty()) {
                ImGui::Text("Children:");
                for (int c : rel.children)
                    if (reg.renderables.count(c)) ImGui::BulletText("%s", reg.renderables[c].name.c_str());
            }
            ImGui::Separator();
            std::vector<std::pair<int, int>> bonds;
            for (auto& [tid, r] : rel.network) bonds.push_back({r.affinity, tid});
            std::sort(bonds.rbegin(), bonds.rend());
            ImGui::BeginChild("RelScroll");
            for (auto& [aff, tid] : bonds) {
                if (!reg.renderables.count(tid)) continue;
                ImVec4 col = aff > 30 ? ImVec4(0.4f, 0.9f, 0.4f, 1) : aff < -30 ? ImVec4(1, 0.35f, 0.35f, 1) : ImVec4(0.8f, 0.8f, 0.8f, 1);
                ImGui::TextColored(col, "%-22s %+d  (%s)", reg.renderables[tid].name.c_str(), aff,
                                   rel.network[tid].type.c_str());
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void DrawTown(SimEngine& eng, UiState& ui) {
    Registry& reg = eng.GetRegistry();
    ImVec2 ds = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(ds.x - 430, 90), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(420, 620), ImGuiCond_FirstUseEver);
    ImGui::Begin("Town");

    auto& town = reg.towns[eng.TownID()];
    auto& res = reg.sect_resources[eng.TownID()];
    auto people = eng.LivingPeople();

    // --- Settlement overview: the numbers that decide survival ---
    if (ImGui::CollapsingHeader("Overview", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Mayor
        if (town.mayor_id != -1 && reg.renderables.count(town.mayor_id)) {
            ImGui::Text("Mayor: %s", reg.renderables[town.mayor_id].name.c_str());
            auto pc = reg.personalities.find(town.mayor_id);
            if (pc != reg.personalities.end()) {
                ImGui::SameLine();
                ImGui::TextDisabled("(wisdom %d: +%d%% production)", pc->second.wisdom, pc->second.wisdom / 4);
            }
        } else {
            ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "No Mayor - the town lacks direction.");
        }
        if (ImGui::Button("Hold Mayoral Election", ImVec2(-1, 0))) {
            ui.log.push_back(eng.HoldElection());
            ui.log_dirty = true;
        }
        ImGui::Spacing();

        // Food balance: rough estimate of production vs mouths
        int farms_n = 0, cottages = 0;
        for (auto& [bid, b] : reg.buildings) {
            if (b.under_construction) continue;
            if (b.type == "Farm") farms_n++;
            if (b.type == "Cottage") cottages++;
        }
        int farm_cap = farms_n * 4, farmers_seen = 0;
        float food_in = 0;
        for (auto& [eid, j] : reg.jobs) {
            auto st = reg.stats.find(eid);
            if (st == reg.stats.end() || st->second.is_dead || eng.IsOnMission(eid)) continue;
            if (j.job == "Farmer") {
                farmers_seen++;
                food_in += (farmers_seen <= farm_cap) ? 3.0f + eng.GetSkill(eid, "Farming") / 2.0f : 1.0f;
            } else if (j.job == "Forager") {
                food_in += 1.0f + eng.GetSkill(eid, "Food Gathering") / 3.0f;
            }
        }
        int eaters = 0;
        for (int eid : people) if (!eng.IsOnMission(eid)) eaters++;
        float net = food_in - (float)eaters;
        ImGui::Text("Food balance:");
        ImGui::SameLine();
        ImGui::TextColored(net >= 0 ? ImVec4(0.5f, 0.9f, 0.5f, 1) : ImVec4(1, 0.4f, 0.4f, 1),
                           "%+.0f/day  (%.0f produced, %d eaten)", net, food_in, eaters);
        if (net < 0) {
            int days_left = net < 0 ? (int)((float)town.food_reserves / -net) : 999;
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "- stores empty in ~%d days!", days_left);
        }

        // Housing
        int capacity = cottages * 4;
        bool crowded = (int)people.size() > capacity;
        ImGui::Text("Housing:");
        ImGui::SameLine();
        ImGui::TextColored(crowded ? ImVec4(1, 0.55f, 0.3f, 1) : ImVec4(0.5f, 0.9f, 0.5f, 1),
                           "%d / %d beds%s", (int)people.size(), capacity, crowded ? "  (CROWDED: morale suffers)" : "");

        // Defense
        int guards = 0;
        for (auto& [eid, j] : reg.jobs) {
            auto st = reg.stats.find(eid);
            if (st != reg.stats.end() && !st->second.is_dead && !eng.IsOnMission(eid) && j.job == "Guard") guards++;
        }
        ImGui::Text("Defense: %d", town.defense_rating);
        ImGui::SameLine();
        ImGui::TextDisabled("(%d guards on duty)", guards);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("5 base + guards (3 + Warfare each)\n+15 per Wall, +8 per Barracks.\nRaiders and beast tides test this.");
        ImGui::Text("Morale: %d/100", town.morale);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Average citizen mood. Shrines, full bellies and\ntournament glory raise it; deaths, hunger and\ncrowding drag it down.");
    }

    // --- Labor priorities (the task system) ---
    if (ImGui::CollapsingHeader("Labor Priorities", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextDisabled("0 = off, 5 = highest. Best-skilled are auto-assigned.");
        std::map<std::string, int> working;
        for (auto& [eid, j] : reg.jobs) {
            auto st = reg.stats.find(eid);
            if (st == reg.stats.end() || st->second.is_dead || eng.IsOnMission(eid)) continue;
            working[j.job]++;
        }
        static const std::vector<std::pair<std::string, std::string>> labor_rows = {
            {"Build", "Builder"}, {"Heal", "Healer"}, {"Farm", "Farmer"}, {"Forage", "Forager"},
            {"Woodcut", "Woodcutter"}, {"Mine", "Miner"}, {"Guard", "Guard"}};
        for (const auto& [cat, job] : labor_rows) {
            int pri = eng.LaborPriorities().count(cat) ? eng.LaborPriorities()[cat] : 3;
            ImGui::SetNextItemWidth(110);
            if (ImGui::SliderInt(("##pri" + cat).c_str(), &pri, 0, 5))
                eng.SetLaborPriority(cat, pri);
            ImGui::SameLine();
            ImGui::Text("%-8s", cat.c_str());
            if (ImGui::IsItemHovered()) {
                const std::string& skill = LaborSkillMap().at(cat);
                auto meta = SkillMetadata().find(skill);
                ImGui::SetTooltip("Governing skill: %s\n%s", skill.c_str(),
                                  meta != SkillMetadata().end() ? meta->second.effect_per_level.c_str() : "");
            }
            ImGui::SameLine();
            ImGui::TextDisabled("%d working", working.count(job) ? working[job] : 0);
        }
        ImGui::TextDisabled("Unassigned: %d cultivating, %d lounging",
                            working.count("Cultivator") ? working["Cultivator"] : 0,
                            working.count("Idle") ? working["Idle"] : 0);
    }

    // --- Travelling merchant ---
    if (eng.MerchantDaysLeft() > 0 && !eng.MerchantStock().empty()) {
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.45f, 0.35f, 0.1f, 1));
        bool open_merchant = ImGui::CollapsingHeader(
            ("Travelling Merchant (" + std::to_string(eng.MerchantDaysLeft()) + " days left)###Merchant").c_str(),
            ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::PopStyleColor();
        if (open_merchant) {
            ImGui::TextDisabled("Spirit stones: %d", res.spirit_stones);
            auto& stock = eng.MerchantStock();
            for (int i = 0; i < (int)stock.size(); ++i) {
                ImGui::PushID(i);
                bool affordable = res.spirit_stones >= stock[i].price;
                if (!affordable) ImGui::BeginDisabled();
                if (ImGui::Button("Buy")) {
                    ui.log.push_back(eng.BuyFromMerchant(i));
                    ui.log_dirty = true;
                    if (!affordable) ImGui::EndDisabled();
                    ImGui::PopID();
                    break; // Stock vector changed; redraw next frame
                }
                if (!affordable) ImGui::EndDisabled();
                ImGui::SameLine();
                ImGui::Text("%s", stock[i].label.c_str());
                if (!stock[i].item_id.empty() && ImGui::IsItemHovered()) {
                    const ItemDefinition* def = GameData::FindItem(stock[i].item_id);
                    if (def)
                        ImGui::SetTooltip("Tier %d %s\n%s%s", def->tier, def->category.c_str(),
                                          def->attack > 0 ? ("Attack +" + std::to_string(def->attack) + "\n").c_str() : "",
                                          def->description.c_str());
                }
                ImGui::SameLine(ImGui::GetWindowWidth() - 110);
                ImGui::TextColored(affordable ? ImVec4(0.4f, 0.8f, 1, 1) : ImVec4(1, 0.4f, 0.4f, 1),
                                   "%d stones", stock[i].price);
                ImGui::PopID();
            }
        }
    }

    // --- Construction: full catalog table with costs ---
    if (ImGui::CollapsingHeader("Construction", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Count owned per type for the table
        std::map<std::string, int> owned;
        for (auto& [bid, b] : reg.buildings)
            if (!b.under_construction) owned[b.type]++;

        if (ImGui::BeginTable("BuildTable", 5,
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("Building", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Cost", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Days", ImGuiTableColumnFlags_WidthFixed, 36);
            ImGui::TableSetupColumn("Own", ImGuiTableColumnFlags_WidthFixed, 30);
            ImGui::TableSetupColumn("##build", ImGuiTableColumnFlags_WidthFixed, 46);
            ImGui::TableHeadersRow();

            for (const auto& spec : SimEngine::BuildingCatalog()) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", spec.type.c_str());
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", spec.description.c_str());

                ImGui::TableSetColumnIndex(1);
                bool wood_ok = town.wood >= spec.wood_cost;
                bool stone_ok = town.stone >= spec.stone_cost;
                if (spec.wood_cost > 0) {
                    ImGui::TextColored(wood_ok ? ImVec4(0.8f, 0.65f, 0.4f, 1) : ImVec4(1, 0.35f, 0.35f, 1),
                                       "%dw", spec.wood_cost);
                    if (spec.stone_cost > 0) ImGui::SameLine();
                }
                if (spec.stone_cost > 0)
                    ImGui::TextColored(stone_ok ? ImVec4(0.7f, 0.7f, 0.75f, 1) : ImVec4(1, 0.35f, 0.35f, 1),
                                       "%ds", spec.stone_cost);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", spec.build_days);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Builder-days of work. Skilled builders\nfinish faster (+25%%/Construction level).");

                ImGui::TableSetColumnIndex(3);
                int n = owned.count(spec.type) ? owned[spec.type] : 0;
                if (n > 0) ImGui::Text("%d", n);
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(4);
                bool affordable = wood_ok && stone_ok;
                if (!affordable) ImGui::BeginDisabled();
                if (ImGui::SmallButton(("Build##" + spec.type).c_str())) {
                    ui.log.push_back(eng.Build(spec.type));
                    ui.log_dirty = true;
                }
                if (!affordable) ImGui::EndDisabled();
            }
            ImGui::EndTable();
        }

        // Queue with progress bars
        for (auto& [bid, b] : reg.buildings) {
            if (!b.under_construction) continue;
            const BuildingSpec* spec = nullptr;
            for (const auto& s : SimEngine::BuildingCatalog())
                if (s.type == b.type) { spec = &s; break; }
            float total = spec ? (float)spec->build_days : (float)std::max(b.build_days_left, 1);
            float frac = 1.0f - (float)b.build_days_left / std::max(total, 1.0f);
            ImGui::Text("%s", b.type.c_str());
            ImGui::SameLine(110);
            ImGui::ProgressBar(std::clamp(frac, 0.0f, 1.0f), ImVec2(-1, 14),
                               (std::to_string(b.build_days_left) + " builder-days left").c_str());
        }
    }

    ImGui::End();
}

void DrawMissions(SimEngine& eng, UiState& ui) {
    Registry& reg = eng.GetRegistry();
    ImVec2 ds = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(ds.x - 360, 480), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Missions & War");

    static const char* types[] = {"Hunt", "Gather", "Adventure", "Raid", "Invade"};
    ImGui::SetNextItemWidth(140);
    ImGui::Combo("Type", &ui.mission_type, types, 5);

    bool needs_target = ui.mission_type >= 3;
    if (needs_target) {
        auto& rivals = eng.Rivals();
        std::string preview = (ui.mission_target < (int)rivals.size()) ? rivals[ui.mission_target].name : "?";
        ImGui::SetNextItemWidth(220);
        if (ImGui::BeginCombo("Target", preview.c_str())) {
            for (int i = 0; i < (int)rivals.size(); ++i) {
                if (rivals[i].destroyed) continue;
                std::string label = rivals[i].name + "  (power " + std::to_string(rivals[i].power) + ")";
                if (ImGui::Selectable(label.c_str(), ui.mission_target == i)) ui.mission_target = i;
            }
            ImGui::EndCombo();
        }
    }

    // Sort candidates by the skill that matters for this mission, best first
    std::string rel_skill = MissionRelevantSkill(types[ui.mission_type]);
    std::vector<std::pair<int, int>> ranked; // (skill value, entity)
    for (int eid : eng.LivingPeople()) {
        auto cc = reg.cultivations.find(eid);
        if (cc == reg.cultivations.end() || cc->second.age < 14) continue;
        ranked.push_back({eng.GetSkill(eid, rel_skill), eid});
    }
    std::sort(ranked.rbegin(), ranked.rend());

    ImGui::Text("Party (%d selected)", (int)ui.party.size());
    ImGui::SameLine();
    ImGui::TextDisabled("- sorted by %s", rel_skill.c_str());
    ImGui::BeginChild("PartyPick", ImVec2(0, 160), ImGuiChildFlags_Borders);
    for (auto& [skill_v, eid] : ranked) {
        bool away = eng.IsOnMission(eid);
        bool checked = ui.party.count(eid) > 0;
        if (away) ImGui::BeginDisabled();
        std::string label = std::string(GenderSymbol(reg, eid)) + reg.renderables[eid].name +
                            "  -  " + rel_skill + " " + std::to_string(skill_v) +
                            "  (Atk " + std::to_string(eng.EffectiveAttack(eid)) + ")" +
                            (away ? " [away]" : "");
        ImGui::PushID(eid);
        if (ImGui::Checkbox(label.c_str(), &checked)) {
            if (checked) ui.party.insert(eid);
            else ui.party.erase(eid);
        }
        if (ImGui::IsItemHovered()) PersonTooltip(eng, eid);
        ImGui::PopID();
        if (away) ImGui::EndDisabled();
    }
    ImGui::EndChild();

    if (ImGui::Button("Launch Mission", ImVec2(-1, 30))) {
        std::vector<int> party(ui.party.begin(), ui.party.end());
        std::string result = eng.StartMission(types[ui.mission_type], party, needs_target ? ui.mission_target : -1);
        ui.log.push_back(result);
        ui.log_dirty = true;
        if (result == "The party has departed.") ui.party.clear();
    }

    if (!eng.Missions().empty()) {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.5f, 1), "Underway");
        for (auto& m : eng.Missions()) {
            std::string line = m.type;
            if (m.target_rival >= 0 && m.target_rival < (int)eng.Rivals().size())
                line += " vs " + eng.Rivals()[m.target_rival].name;
            line += "  -  " + std::to_string(m.party.size()) + " people, " + std::to_string(m.days_left) + "d left";
            ImGui::BulletText("%s", line.c_str());
        }
    }
    ImGui::End();
}

void DrawRivals(SimEngine& eng, UiState& ui) {
    ImGui::SetNextWindowPos(ImVec2(400, 90), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(480, 220), ImGuiCond_FirstUseEver);
    ImGui::Begin("Rival Settlements");
    if (ImGui::BeginTable("Rivals", 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Power", ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableSetupColumn("Wealth", ImGuiTableColumnFlags_WidthFixed, 55);
        ImGui::TableSetupColumn("Relation", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupColumn("Travel", ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableHeadersRow();
        auto& rivals = eng.Rivals();
        for (int i = 0; i < (int)rivals.size(); ++i) {
            auto& r = rivals[i];
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(i);
            if (ImGui::Selectable(r.name.c_str(), ui.mission_target == i, ImGuiSelectableFlags_SpanAllColumns))
                ui.mission_target = i;
            ImGui::PopID();
            ImGui::TableSetColumnIndex(1); ImGui::Text("%d", r.power);
            ImGui::TableSetColumnIndex(2); ImGui::Text("%d", r.wealth);
            ImGui::TableSetColumnIndex(3); ImGui::Text("%d", r.relation);
            ImGui::TableSetColumnIndex(4); ImGui::Text("%dd", r.distance_days);
            ImGui::TableSetColumnIndex(5);
            if (r.destroyed) ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1), "Ruins");
            else if (r.conquered) ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Tributary");
            else if (r.relation < -40) ImGui::TextColored(ImVec4(1, 0.35f, 0.35f, 1), "Hostile");
            else if (r.relation > 40) ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.4f, 1), "Friendly");
            else ImGui::Text("Neutral");
        }
        ImGui::EndTable();
    }
    ImGui::TextDisabled("Select a rival, then use Missions: Raid / Invade.");
    ImGui::End();
}

void DrawEventLog(UiState& ui) {
    ImVec2 ds = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(400, ds.y - 250), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(ds.x - 780, 240), ImGuiCond_FirstUseEver);
    ImGui::Begin("Chronicle");
    if (ImGui::SmallButton("Clear")) ui.log.clear();
    ImGui::Separator();
    ImGui::BeginChild("LogScroll");
    for (const auto& line : ui.log) {
        ImVec4 col(0.85f, 0.85f, 0.85f, 1);
        if (line.find("[DEATH]") != std::string::npos) col = ImVec4(1, 0.3f, 0.3f, 1);
        else if (line.find("BREAKTHROUGH") != std::string::npos) col = ImVec4(0.4f, 0.9f, 1, 1);
        else if (line.find("WEDDING") != std::string::npos || line.find("BIRTH") != std::string::npos)
            col = ImVec4(0.5f, 0.95f, 0.5f, 1);
        else if (line.find("!!!") != std::string::npos || line.find("RAID") != std::string::npos ||
                 line.find("INVASION") != std::string::npos || line.find("BEAST TIDE") != std::string::npos)
            col = ImVec4(1, 0.6f, 0.25f, 1);
        else if (line.find("ELECTION") != std::string::npos || line.find("VICTORY") != std::string::npos)
            col = ImVec4(0.95f, 0.85f, 0.3f, 1);
        ImGui::TextColored(col, "%s", line.c_str());
    }
    if (ui.log_dirty) {
        ImGui::SetScrollHereY(1.0f);
        ui.log_dirty = false;
    }
    ImGui::EndChild();
    ImGui::End();
}

// After-action reports: who went, what happened, who bled, what came home.
void DrawReports(SimEngine& eng, UiState& ui) {
    if (!ui.show_reports) return;
    auto& reports = eng.MissionReports();

    ImVec2 ds = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(ds.x * 0.5f, ds.y * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(680, 520), ImGuiCond_FirstUseEver);
    ImGui::Begin("After-Action Reports", &ui.show_reports);

    if (reports.empty()) {
        ImGui::TextDisabled("No missions have returned yet.");
        ImGui::End();
        return;
    }
    if (ui.report_view < 0 || ui.report_view >= (int)reports.size())
        ui.report_view = (int)reports.size() - 1;

    // Left: report list, newest first
    ImGui::BeginChild("RepList", ImVec2(200, 0), ImGuiChildFlags_Borders);
    for (int i = (int)reports.size() - 1; i >= 0; --i) {
        const auto& r = reports[i];
        std::string label = r.type + (r.target.empty() ? "" : " vs " + r.target) + "##rep" + std::to_string(i);
        ImVec4 col = r.success ? ImVec4(0.5f, 0.9f, 0.5f, 1) : ImVec4(1, 0.45f, 0.4f, 1);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        if (ImGui::Selectable(label.c_str(), ui.report_view == i)) ui.report_view = i;
        ImGui::PopStyleColor();
        ImGui::TextDisabled("  %s", r.date.c_str());
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // Right: the selected report
    const MissionReport& r = reports[ui.report_view];
    ImGui::BeginChild("RepDetail", ImVec2(0, 0), ImGuiChildFlags_Borders);
    ImGui::SetWindowFontScale(1.25f);
    ImGui::TextColored(r.success ? ImVec4(0.5f, 0.95f, 0.5f, 1) : ImVec4(1, 0.4f, 0.4f, 1),
                       "%s%s - %s", r.type.c_str(),
                       r.target.empty() ? "" : (" against " + r.target).c_str(),
                       r.success ? "SUCCESS" : "FAILURE");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextDisabled("%s", r.date.c_str());
    ImGui::Separator();

    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.5f, 1), "Sent (%d):", (int)r.party.size());
    for (const auto& n : r.party) ImGui::BulletText("%s", n.c_str());

    if (!r.casualties.empty()) {
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Killed in action:");
        for (const auto& n : r.casualties)
            ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "  + %s", n.c_str());
    }
    if (!r.injuries.empty()) {
        ImGui::TextColored(ImVec4(1, 0.6f, 0.25f, 1), "Wounded:");
        for (const auto& n : r.injuries)
            ImGui::TextColored(ImVec4(1, 0.7f, 0.45f, 1), "  %s", n.c_str());
    }
    if (r.casualties.empty() && r.injuries.empty())
        ImGui::TextColored(ImVec4(0.5f, 0.9f, 0.5f, 1), "Everyone returned without a scratch.");

    if (!r.loot.empty()) {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.95f, 0.85f, 0.3f, 1), "Brought back:");
        for (const auto& n : r.loot) ImGui::BulletText("%s", n.c_str());
    }
    if (!r.items.empty()) {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Items gained:");
        for (const auto& n : r.items) ImGui::BulletText("%s", n.c_str());
    }
    if (!r.xp.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.9f, 1, 1), "Experience:");
        for (const auto& n : r.xp) ImGui::BulletText("%s", n.c_str());
    }

    if (!r.events.empty()) {
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Full account")) {
            for (const auto& line : r.events) ImGui::TextWrapped("%s", line.c_str());
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void DrawGameMenu(sf::RenderWindow& window, SimEngine& eng, UiState& ui) {
    if (!ui.show_game_menu) return;
    ImVec2 ds = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(ds.x * 0.5f, ds.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(360, 0));
    ImGui::Begin("Game Menu", &ui.show_game_menu,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::InputText("##savename", ui.save_name, sizeof(ui.save_name));
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        ui.menu_feedback = eng.SaveGame(ui.save_name) ? "Saved." : "Save failed!";
    }
    ImGui::Spacing();
    if (ImGui::Button("Load Game", ImVec2(-1, 32))) ImGui::OpenPopup("LoadPopupInGame");
    if (ImGui::BeginPopup("LoadPopupInGame")) {
        for (auto& s : ListSaves()) {
            if (ImGui::Selectable(s.c_str())) {
                if (eng.LoadGame(s)) {
                    ui.log.clear();
                    EnterPlaying(eng, ui);
                    ui.menu_feedback = "Loaded " + s + ".";
                } else ui.menu_feedback = "Failed to load " + s;
            }
        }
        ImGui::EndPopup();
    }
    ImGui::Spacing();
    if (ImGui::Button("Return to Main Menu", ImVec2(-1, 32))) {
        ui.screen = Screen::MainMenu;
        ui.show_game_menu = false;
        ui.paused = true;
    }
    ImGui::Spacing();
    if (ImGui::Button("Exit Game", ImVec2(-1, 32))) window.close();
    if (!ui.menu_feedback.empty()) ImGui::TextColored(ImVec4(0.7f, 0.9f, 0.7f, 1), "%s", ui.menu_feedback.c_str());
    ImGui::End();
    if (!ui.show_game_menu) ui.menu_feedback.clear();
}

// Markers + name labels for the town and rivals, drawn over the map.
void DrawMapOverlays(sf::RenderWindow& window, SimEngine& eng, const sf::View& view) {
    GameMap& map = eng.Map();
    auto* dl = ImGui::GetBackgroundDrawList();

    auto label = [&](float wx, float wy, const std::string& text, ImU32 col) {
        sf::Vector2i px = window.mapCoordsToPixel({wx, wy}, view);
        dl->AddText(ImVec2((float)px.x + 1, (float)px.y + 1), IM_COL32(0, 0, 0, 220), text.c_str());
        dl->AddText(ImVec2((float)px.x, (float)px.y), col, text.c_str());
    };

    // Town marker
    {
        sf::RectangleShape r({TILE * 2.f, TILE * 2.f});
        r.setPosition({(map.town_x - 0.5f) * TILE, (map.town_y - 0.5f) * TILE});
        r.setFillColor(sf::Color(250, 210, 80));
        r.setOutlineColor(sf::Color::Black);
        r.setOutlineThickness(1.5f);
        window.draw(r);
        Registry& reg = eng.GetRegistry();
        std::string name = reg.towns.count(eng.TownID()) ? reg.towns[eng.TownID()].name : "Town";
        label((map.town_x + 2.0f) * TILE, (map.town_y - 1.0f) * TILE, name, IM_COL32(250, 220, 120, 255));
    }

    // Rivals
    for (auto& rv : eng.Rivals()) {
        sf::Color c = rv.destroyed ? sf::Color(90, 90, 90)
                      : rv.conquered ? sf::Color(80, 170, 240)
                      : rv.relation < -40 ? sf::Color(220, 60, 60)
                      : rv.relation > 40 ? sf::Color(90, 200, 90)
                                         : sf::Color(200, 160, 160);
        sf::RectangleShape r({TILE * 1.6f, TILE * 1.6f});
        r.setPosition({(rv.x - 0.3f) * TILE, (rv.y - 0.3f) * TILE});
        r.setFillColor(c);
        r.setOutlineColor(sf::Color::Black);
        r.setOutlineThickness(1.0f);
        window.draw(r);
        label((rv.x + 1.8f) * TILE, (rv.y - 1.0f) * TILE, rv.name, IM_COL32(230, 200, 200, 255));
    }
}

void MapHoverTooltip(sf::RenderWindow& window, SimEngine& eng, const sf::View& view) {
    if (ImGui::GetIO().WantCaptureMouse) return;
    GameMap& map = eng.Map();
    sf::Vector2i mp = sf::Mouse::getPosition(window);
    sf::Vector2f world = window.mapPixelToCoords(mp, view);
    int tx = (int)std::floor(world.x / TILE);
    int ty = (int)std::floor(world.y / TILE);
    if (tx < 0 || ty < 0 || tx >= map.Width() || ty >= map.Height()) return;

    const WorldTile& t = map.At(tx, ty);
    ImGui::BeginTooltip();
    ImGui::Text("%s  (%d, %d)", BiomeName((Biome)t.biome).c_str(), tx, ty);
    ImGui::Text("Qi density: %.0f%%", t.qi * 100.0f);
    if (tx == map.town_x && ty == map.town_y) ImGui::TextColored(ImVec4(1, 0.85f, 0.4f, 1), "Your settlement");
    for (auto& rv : eng.Rivals())
        if (std::abs(rv.x - tx) <= 1 && std::abs(rv.y - ty) <= 1)
            ImGui::TextColored(ImVec4(1, 0.6f, 0.6f, 1), "%s (power %d)", rv.name.c_str(), rv.power);
    ImGui::EndTooltip();
}

} // namespace

int main(int argc, char** argv) {
    std::srand((unsigned)std::time(nullptr));

    // Locate the project root (the markdown databases) when launched from build dirs
    for (int i = 0; i < 4 && !fs::exists("Master Bestiary List.md"); ++i)
        fs::current_path(fs::current_path().parent_path());

    // --selftest: render a real game unattended and dump screenshots for CI/inspection
    bool selftest = argc > 1 && std::string(argv[1]) == "--selftest";

    sf::RenderWindow window(sf::VideoMode({1600u, 900u}), "Cultivation Fortress");
    window.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window)) return -1;

    // Merge ♂ / ♀ glyphs into the default font (Segoe UI Symbol ships with Windows)
    {
        ImGuiIO& io = ImGui::GetIO();
        static const ImWchar gender_ranges[] = {0x2640, 0x2642, 0};
        ImFontConfig cfg;
        cfg.MergeMode = true;
        const char* symbol_font = "C:/Windows/Fonts/seguisym.ttf";
        if (fs::exists(symbol_font)) {
            io.Fonts->AddFontFromFileTTF(symbol_font, 14.0f, &cfg, gender_ranges);
            if (!ImGui::SFML::UpdateFontTexture()) return -1;
        }
    }

    SimEngine& eng = SimEngine::Get();
    eng.LoadDataFiles();
    UiState ui;

    int frame = 0;

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()) window.close();

            if (ui.screen == Screen::Playing) {
                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::Escape) ui.show_game_menu = !ui.show_game_menu;
                    if (key->code == sf::Keyboard::Key::Space && !ImGui::GetIO().WantTextInput)
                        ui.paused = !ui.paused;
                }
                if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    if (!ImGui::GetIO().WantCaptureMouse) {
                        ui.zoom *= (wheel->delta > 0) ? 1.15f : (1.0f / 1.15f);
                        ui.zoom = std::clamp(ui.zoom, 0.6f, 10.0f);
                    }
                }
            }
        }

        sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        if (ui.screen == Screen::MainMenu) {
            DrawMainMenu(window, eng, ui);
            window.clear(sf::Color(14, 18, 28));
        } else if (ui.screen == Screen::Embark) {
            DrawEmbark(eng, ui);
            window.clear(sf::Color(14, 18, 28));
        } else {
            // --- Playing ---
            // Camera input
            if (!ImGui::GetIO().WantTextInput) {
                float pan = 420.0f * dt.asSeconds() / ui.zoom;
                using K = sf::Keyboard::Key;
                if (sf::Keyboard::isKeyPressed(K::W) || sf::Keyboard::isKeyPressed(K::Up)) ui.cam.y -= pan;
                if (sf::Keyboard::isKeyPressed(K::S) || sf::Keyboard::isKeyPressed(K::Down)) ui.cam.y += pan;
                if (sf::Keyboard::isKeyPressed(K::A) || sf::Keyboard::isKeyPressed(K::Left)) ui.cam.x -= pan;
                if (sf::Keyboard::isKeyPressed(K::D) || sf::Keyboard::isKeyPressed(K::Right)) ui.cam.x += pan;
            }
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            if (!ImGui::GetIO().WantCaptureMouse &&
                (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) ||
                 sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))) {
                sf::Vector2i d = mouse - ui.last_mouse;
                ui.cam.x -= (float)d.x / ui.zoom;
                ui.cam.y -= (float)d.y / ui.zoom;
            }
            ui.last_mouse = mouse;

            // Clamp camera to map bounds
            GameMap& map = eng.Map();
            ui.cam.x = std::clamp(ui.cam.x, 0.f, map.Width() * TILE);
            ui.cam.y = std::clamp(ui.cam.y, 0.f, map.Height() * TILE);

            // Time advance
            bool game_over = eng.LivingPeople().empty() && eng.IsGameStarted();
            if (!ui.paused && !ui.show_game_menu && !game_over) {
                ui.accum += dt.asSeconds() * UiState::SPEED;
                int days = (int)ui.accum;
                if (days > 0) {
                    ui.accum -= (float)days;
                    eng.AdvanceDays(std::min(days, 30));
                    DrainLog(eng, ui);
                }
            }

            // A finished mission files its report: pause and present it
            if ((int)eng.MissionReports().size() > ui.reports_seen) {
                ui.reports_seen = (int)eng.MissionReports().size();
                ui.report_view = ui.reports_seen - 1;
                ui.show_reports = true;
                ui.paused = true;
            }

            // World render
            sf::Vector2u ws = window.getSize();
            sf::View view;
            view.setCenter(ui.cam);
            view.setSize({(float)ws.x / ui.zoom, (float)ws.y / ui.zoom});
            window.clear(sf::Color(8, 10, 16));
            window.setView(view);
            window.draw(ui.map_va);
            DrawMapOverlays(window, eng, view);
            window.setView(window.getDefaultView());

            // UI
            DrawTopBar(eng, ui);
            DrawCitizens(eng, ui);
            DrawInspector(eng, ui);
            DrawTown(eng, ui);
            DrawMissions(eng, ui);
            DrawRivals(eng, ui);
            DrawEventLog(ui);
            DrawReports(eng, ui);
            DrawGameMenu(window, eng, ui);
            MapHoverTooltip(window, eng, view);

            if (game_over) {
                ImVec2 ds = ImGui::GetIO().DisplaySize;
                ImGui::SetNextWindowPos(ImVec2(ds.x * 0.5f, ds.y * 0.45f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                ImGui::Begin("##GameOver", nullptr,
                             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::SetWindowFontScale(1.6f);
                ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "The fortress has fallen.");
                ImGui::SetWindowFontScale(1.0f);
                ImGui::TextDisabled("Every last soul has perished.");
                if (ImGui::Button("Return to Main Menu", ImVec2(-1, 32))) ui.screen = Screen::MainMenu;
                ImGui::End();
            }
        }

        ImGui::SFML::Render(window);

        if (selftest) {
            auto capture = [&](const char* file) {
                sf::Texture tex;
                if (tex.resize(window.getSize())) {
                    tex.update(window);
                    if (tex.copyToImage().saveToFile(file))
                        printf("[selftest] saved %s\n", file);
                }
            };
            frame++;
            if (frame == 10) capture("selftest_menu.png");
            if (frame == 12) { eng.NewGame(); ui.screen = Screen::Embark; }
            if (frame == 22) capture("selftest_embark.png");
            if (frame == 24) { eng.StartGame(); EnterPlaying(eng, ui); }
            if (frame == 30) {
                // Send the two best hunters out so a report comes back
                auto people = eng.LivingPeople();
                std::vector<std::pair<int, int>> ranked;
                for (int eid : people) ranked.push_back({eng.GetSkill(eid, "Hunting"), eid});
                std::sort(ranked.rbegin(), ranked.rend());
                std::vector<int> party;
                for (int i = 0; i < (int)ranked.size() && i < 2; ++i) party.push_back(ranked[i].second);
                eng.StartMission("Hunt", party);
                DrainLog(eng, ui);
            }
            if (frame == 40) { eng.AdvanceDays(45); DrainLog(eng, ui); }
            if (frame == 50) {
                auto people = eng.LivingPeople();
                if (!people.empty()) ui.selected_citizen = people[0];
            }
            if (frame == 80) {
                capture("selftest_game.png");
                window.close();
            }
        }

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
