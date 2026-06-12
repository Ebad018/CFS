// --- Cultivation Fortress: text-mode front-end ---
// Pure console client of the SimEngine. All game logic lives in the engine;
// this file only parses commands and prints state.
#include "SimEngine.h"
#include "HistoryGenerator.h"
#include "GameData.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string Lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::string Trim(const std::string& s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    auto b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

std::vector<std::string> SplitWords(const std::string& s) {
    std::vector<std::string> out;
    std::istringstream ss(s);
    std::string w;
    while (ss >> w) out.push_back(w);
    return out;
}

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

void DrainLog(SimEngine& eng) {
    for (const auto& line : eng.RecentLog()) std::cout << "  " << line << "\n";
    eng.RecentLog().clear();
}

std::string TopSkills(Registry& reg, int eid, int count) {
    auto it = reg.skill_sets.find(eid);
    if (it == reg.skill_sets.end()) return "-";
    std::vector<std::pair<std::string, int>> all(it->second.skills.begin(), it->second.skills.end());
    std::sort(all.begin(), all.end(), [](auto& a, auto& b) { return a.second > b.second; });
    std::string out;
    for (int i = 0; i < count && i < (int)all.size(); ++i) {
        if (all[i].second <= 0) break;
        if (!out.empty()) out += ", ";
        out += all[i].first + " " + std::to_string(all[i].second);
    }
    return out.empty() ? "-" : out;
}

void PrintCandidates(SimEngine& eng) {
    Registry& reg = eng.GetRegistry();
    std::cout << "\n=== Your 10 Founders ===\n";
    std::cout << std::left << std::setw(4) << "#" << std::setw(22) << "Name" << std::setw(8) << "Sex"
              << std::setw(5) << "Age" << std::setw(8) << "Talent" << "Best Skills\n";
    std::cout << std::string(85, '-') << "\n";
    int i = 1;
    for (int eid : eng.Candidates()) {
        auto& rc = reg.renderables[eid];
        auto& cc = reg.cultivations[eid];
        auto& rel = reg.relations[eid];
        std::ostringstream talent;
        talent << std::fixed << std::setprecision(2) << cc.talent;
        std::cout << std::left << std::setw(4) << i++ << std::setw(22) << rc.name << std::setw(8) << rel.gender
                  << std::setw(5) << cc.age << std::setw(8) << talent.str() << TopSkills(reg, eid, 3) << "\n";
    }
    std::cout << "\nCommands: view <#>, reroll <#>, reroll (all), embark\n";
}

void PrintBody(Registry& reg, int eid) {
    auto body = reg.bodies.find(eid);
    if (body == reg.bodies.end()) { std::cout << "No anatomy data.\n"; return; }
    const BodyComponent& b = body->second;
    std::string name = reg.renderables.count(eid) ? reg.renderables[eid].name : "Unknown";

    std::cout << "\n=== Anatomy of " << name << " (" << b.plan << ") ===\n";
    std::cout << "Blood: " << (int)b.blood << "/" << (int)b.max_blood
              << "   Pain: " << (int)b.pain << "/100\n";
    std::cout << std::string(60, '-') << "\n";

    // Print as an indented tree
    std::vector<int> depth(b.parts.size(), 0);
    for (size_t i = 0; i < b.parts.size(); ++i)
        if (b.parts[i].parent >= 0) depth[i] = depth[b.parts[i].parent] + 1;

    for (size_t i = 0; i < b.parts.size(); ++i) {
        const BodyPart& p = b.parts[i];
        std::string pad((size_t)depth[i] * 2, ' ');
        std::string status;
        if (p.IsGone()) status = " [" + Lower(WoundLevelName(p.Wound())) + "]";
        else if (p.wound != (int)WoundLevel::Fine)
            status = " [" + WoundLevelName(p.Wound()) + " " + std::to_string(p.hp) + "/" + std::to_string(p.max_hp) + "]";
        if (p.bleed > 0.5f) status += " (bleeding)";
        std::cout << pad << p.name << status << "\n";
    }
}

void PrintInspect(SimEngine& eng, int eid) {
    Registry& reg = eng.GetRegistry();
    auto& rc = reg.renderables[eid];
    auto& cc = reg.cultivations[eid];
    auto& rel = reg.relations[eid];
    auto& st = reg.stats[eid];

    std::cout << "\n=== " << rc.name << " ===\n";
    if (st.is_dead) {
        std::cout << "DECEASED (Year " << st.death_year << "): " << st.cause_of_death << "\n";
        return;
    }
    std::cout << rel.gender << ", age " << cc.age << "/" << cc.lifespan
              << "   Realm: " << RealmName(cc.realm_major) << " " << cc.realm_minor
              << "   Qi: " << (int)cc.qi << "/" << (int)cc.max_qi
              << "   Talent: " << std::fixed << std::setprecision(2) << cc.talent << "\n";

    auto job = reg.jobs.find(eid);
    std::cout << "Job: " << (job != reg.jobs.end() ? job->second.job : "None");
    if (eng.IsOnMission(eid)) std::cout << " (AWAY ON MISSION)";
    std::cout << "   Weapon: " << eng.EquippedWeaponName(eid)
              << "   Atk: " << eng.EffectiveAttack(eid) << "  Def: " << eng.EffectiveDefense(eid) << "\n";

    auto needs = reg.needs.find(eid);
    if (needs != reg.needs.end())
        std::cout << "Morale: " << needs->second.morale << "/100   Hunger: " << needs->second.hunger << " days\n";

    auto pc = reg.personalities.find(eid);
    if (pc != reg.personalities.end()) {
        auto& p = pc->second;
        std::vector<std::pair<std::string, int>> traits = {
            {"ambition", p.ambition}, {"greed", p.greed}, {"bravery", p.bravery}, {"loyalty", p.loyalty},
            {"ruthlessness", p.ruthlessness}, {"caution", p.caution}, {"wisdom", p.wisdom},
            {"social", p.social}, {"vengeance", p.vengeance}, {"diligence", p.diligence}};
        std::sort(traits.begin(), traits.end(), [](auto& a, auto& b) { return a.second > b.second; });
        std::cout << "Personality: ";
        for (int i = 0; i < 3; ++i) std::cout << (i ? ", " : "") << traits[i].first << " " << traits[i].second;
        std::cout << "\n";
    }

    std::cout << "Skills: " << TopSkills(reg, eid, 6) << "\n";

    if (rel.spouse_id != -1 && reg.renderables.count(rel.spouse_id))
        std::cout << "Spouse: " << reg.renderables[rel.spouse_id].name << "\n";
    if (!rel.children.empty()) {
        std::cout << "Children: ";
        for (size_t i = 0; i < rel.children.size(); ++i) {
            if (reg.renderables.count(rel.children[i]))
                std::cout << (i ? ", " : "") << reg.renderables[rel.children[i]].name;
        }
        std::cout << "\n";
    }
    // Strongest bonds
    std::vector<std::pair<int, int>> bonds;
    for (const auto& [tid, r] : rel.network) bonds.push_back({r.affinity, tid});
    if (!bonds.empty()) {
        std::sort(bonds.rbegin(), bonds.rend());
        std::cout << "Bonds: ";
        int shown = 0;
        for (auto& [aff, tid] : bonds) {
            if (shown >= 3 || !reg.renderables.count(tid)) break;
            std::cout << (shown ? ", " : "") << reg.renderables[tid].name << " (" << (aff > 0 ? "+" : "") << aff << ")";
            shown++;
        }
        std::cout << "\n";
    }

    auto body = reg.bodies.find(eid);
    if (body != reg.bodies.end())
        std::cout << "Condition: " << Body::Summarize(body->second) << "\n";
    std::cout << "(Use 'body " << rc.name << "' for the full anatomy.)\n";
}

void PrintPeople(SimEngine& eng) {
    Registry& reg = eng.GetRegistry();
    auto people = eng.LivingPeople();
    std::cout << "\n=== Population (" << people.size() << ") ===\n";
    std::cout << std::left << std::setw(5) << "#" << std::setw(22) << "Name" << std::setw(5) << "Age" << std::setw(12) << "Job"
              << std::setw(18) << "Realm" << std::setw(24) << "Weapon" << "Condition\n";
    std::cout << std::string(110, '-') << "\n";
    int idx = 0;
    for (int eid : people) {
        idx++;
        auto& rc = reg.renderables[eid];
        auto& cc = reg.cultivations[eid];
        std::string job = reg.jobs.count(eid) ? reg.jobs[eid].job : "-";
        if (eng.IsOnMission(eid)) job = "[Mission]";
        std::string realm = RealmName(cc.realm_major) + " " + std::to_string(cc.realm_minor);
        std::string cond = "fine";
        auto body = reg.bodies.find(eid);
        if (body != reg.bodies.end()) cond = Body::Summarize(body->second);
        if (cond.size() > 40) cond = cond.substr(0, 37) + "...";
        std::cout << std::left << std::setw(5) << ("#" + std::to_string(idx)) << std::setw(22) << rc.name
                  << std::setw(5) << cc.age << std::setw(12) << job
                  << std::setw(18) << realm << std::setw(24) << eng.EquippedWeaponName(eid).substr(0, 22) << cond << "\n";
    }
}

void PrintStatus(SimEngine& eng) {
    Registry& reg = eng.GetRegistry();
    int tid = eng.TownID();
    auto& town = reg.towns[tid];
    auto& res = reg.sect_resources[tid];

    std::cout << "\n=== " << town.name << " - " << eng.GetDateString() << " ===\n";
    std::cout << "Population: " << town.population << "   Morale: " << town.morale << "/100"
              << "   Defense: " << town.defense_rating << "\n";
    std::cout << "Mayor: " << (town.mayor_id != -1 && reg.renderables.count(town.mayor_id)
                                   ? reg.renderables[town.mayor_id].name : "(none - hold an election!)") << "\n";
    std::cout << "Food: " << town.food_reserves << "   Wood: " << town.wood << "   Stone: " << town.stone
              << "   Herbs: " << res.herbs << "   Ore: " << res.ores << "\n";
    std::cout << "Spirit Stones: " << res.spirit_stones << "   Wealth: " << town.wealth << "\n";

    // Buildings
    std::map<std::string, int> done;
    std::vector<std::string> wip;
    for (auto& [bid, b] : reg.buildings) {
        if (b.under_construction)
            wip.push_back(b.type + " (" + std::to_string(b.build_days_left) + " builder-days left)");
        else
            done[b.type]++;
    }
    std::cout << "Buildings: ";
    if (done.empty()) std::cout << "(none)";
    bool first = true;
    for (auto& [t, n] : done) { std::cout << (first ? "" : ", ") << t << " x" << n; first = false; }
    std::cout << "\n";
    for (auto& w : wip) std::cout << "  Under construction: " << w << "\n";

    // Jobs summary
    std::map<std::string, int> jobs;
    for (auto& [eid, j] : reg.jobs) {
        auto st = reg.stats.find(eid);
        if (st == reg.stats.end() || st->second.is_dead) continue;
        jobs[eng.IsOnMission(eid) ? "[On Mission]" : j.job]++;
    }
    std::cout << "Workforce: ";
    first = true;
    for (auto& [t, n] : jobs) { std::cout << (first ? "" : ", ") << t << " x" << n; first = false; }
    std::cout << "\n";

    if (!eng.Missions().empty()) {
        std::cout << "Active missions:\n";
        for (auto& m : eng.Missions()) {
            std::cout << "  " << m.type;
            if (m.target_rival >= 0 && m.target_rival < (int)eng.Rivals().size())
                std::cout << " vs " << eng.Rivals()[m.target_rival].name;
            std::cout << " - " << m.party.size() << " people, " << m.days_left << " days remaining\n";
        }
    }
}

void PrintRivals(SimEngine& eng) {
    std::cout << "\n=== Known Settlements ===\n";
    std::cout << std::left << std::setw(4) << "#" << std::setw(26) << "Name" << std::setw(8) << "Power"
              << std::setw(8) << "Wealth" << std::setw(10) << "Relation" << std::setw(10) << "Travel" << "Status\n";
    std::cout << std::string(78, '-') << "\n";
    int i = 0;
    for (auto& r : eng.Rivals()) {
        std::string status = r.destroyed ? "RUINS" : r.conquered ? "CONQUERED (pays tribute)"
                             : r.relation < -40 ? "HOSTILE" : r.relation > 40 ? "Friendly" : "Neutral";
        std::cout << std::left << std::setw(4) << i++ << std::setw(26) << r.name << std::setw(8) << r.power
                  << std::setw(8) << r.wealth << std::setw(10) << r.relation
                  << std::setw(10) << (std::to_string(r.distance_days) + "d") << status << "\n";
    }
    std::cout << "(Use 'raid <#> <names>' or 'invade <#> <names>' to attack.)\n";
}

void PrintArmory(SimEngine& eng) {
    Registry& reg = eng.GetRegistry();
    auto& stash = reg.sect_resources[eng.TownID()].stash;
    std::cout << "\n=== Town Armory ===\n";
    if (stash.empty()) { std::cout << "(empty - adventure, raid or build a Blacksmith to stock it)\n"; return; }
    for (auto& it : stash) {
        const ItemDefinition* def = GameData::FindItem(it.item_id);
        if (def)
            std::cout << "  " << def->name << " x" << it.count << "  (Tier " << def->tier
                      << ", Atk +" << def->attack << ") - " << def->description.substr(0, 50) << "\n";
        else
            std::cout << "  " << it.item_id << " x" << it.count << "\n";
    }
    std::cout << "(Use 'arm <person> with <weapon>' to equip someone.)\n";
}

void PrintBuildCatalog() {
    std::cout << "\n=== Building Catalog ===\n";
    for (const auto& s : SimEngine::BuildingCatalog()) {
        std::cout << "  " << std::left << std::setw(12) << s.type
                  << "wood " << std::setw(4) << s.wood_cost << "stone " << std::setw(4) << s.stone_cost
                  << std::setw(3) << s.build_days << " builder-days  " << s.description << "\n";
    }
    std::cout << "(Use 'build <type>' to start one. Builders work on it daily.)\n";
}

void PrintLogTail(int n) {
    const auto& hist = HistoryGenerator::GetHistory();
    int start = std::max(0, (int)hist.size() - n);
    std::cout << "\n=== Chronicle (last " << (int)hist.size() - start << " events) ===\n";
    for (int i = start; i < (int)hist.size(); ++i)
        std::cout << "  " << hist[i].text << "\n";
}

void PrintHelp(bool setup) {
    if (setup) {
        std::cout << "\nSetup commands:\n"
                  << "  view <#>          Full sheet of candidate #\n"
                  << "  reroll <#>        Replace one candidate\n"
                  << "  reroll            Replace all 10\n"
                  << "  embark            Found the settlement with these 10\n"
                  << "  quit              Exit\n";
        return;
    }
    std::cout << "\nCommands (the world advances only when you 'wait'):\n"
              << "  status                         Town overview\n"
              << "  people                         List everyone (refer to anyone by name or #N)\n"
              << "  inspect <name>                 Full character sheet\n"
              << "  body <name>                    Full anatomy readout\n"
              << "  build [type]                   List buildings / start construction\n"
              << "  job <name> <role>              Assign work (Farmer, Forager, Woodcutter, Miner,\n"
              << "                                 Builder, Guard, Healer, Cultivator, Idle)\n"
              << "  armory                         List weapons in the armory\n"
              << "  arm <name> with <weapon>       Equip a weapon from the armory\n"
              << "  disarm <name>                  Return weapon to the armory\n"
              << "  mission <type> <a, b, ...>     Send a party: Hunt, Gather or Adventure\n"
              << "  rivals                         List other settlements\n"
              << "  raid <#> <a, b, ...>           Raid a rival for loot\n"
              << "  invade <#> <a, b, ...>         Full invasion to conquer a rival\n"
              << "  elect                          Hold a mayoral election\n"
              << "  wait [days]                    Let time pass (default 1 day)\n"
              << "  log [n]                        Show the chronicle (default 15)\n"
              << "  save <slot> / load <slot>      Persistence\n"
              << "  help / quit\n";
}

// Parses "a, b, c" name list into entity ids; reports problems.
std::vector<int> ParseParty(SimEngine& eng, const std::string& list, std::string& error) {
    std::vector<int> out;
    std::istringstream ss(list);
    std::string name;
    while (std::getline(ss, name, ',')) {
        name = Trim(name);
        if (name.empty()) continue;
        int id = eng.FindPersonByName(name);
        if (id == -1) { error = "Nobody called '" + name + "' lives here."; return {}; }
        if (std::find(out.begin(), out.end(), id) != out.end()) continue;
        out.push_back(id);
    }
    if (out.empty()) error = "No party members given. Separate names with commas.";
    return out;
}

} // namespace

int main() {
    std::srand((unsigned)std::time(nullptr));
    SimEngine& eng = SimEngine::Get();

    std::cout << "==========================================\n"
              << "       C U L T I V A T I O N   F O R T R E S S\n"
              << "   A settlement simulation of flesh and qi\n"
              << "==========================================\n";

    eng.NewGame();
    std::cout << "\nTen souls stand ready to found a settlement in the wilds.\n";
    PrintCandidates(eng);

    bool setup = true;
    std::string line;
    while (true) {
        std::cout << (setup ? "\n[setup] > " : "\n[" + eng.GetDateString() + "] > ");
        if (!std::getline(std::cin, line)) break;
        line = Trim(line);
        if (line.empty()) continue;
        auto words = SplitWords(line);
        std::string cmd = Lower(words[0]);

        if (cmd == "quit" || cmd == "exit") break;

        // ------------- SETUP PHASE -------------
        if (setup) {
            if (cmd == "help") { PrintHelp(true); }
            else if (cmd == "view" && words.size() > 1) {
                int n = std::atoi(words[1].c_str());
                if (n >= 1 && n <= (int)eng.Candidates().size()) PrintInspect(eng, eng.Candidates()[n - 1]);
                else std::cout << "Pick 1-10.\n";
            }
            else if (cmd == "reroll") {
                if (words.size() > 1) {
                    int n = std::atoi(words[1].c_str());
                    if (n >= 1 && n <= 10) { eng.RerollCandidate(n - 1); PrintCandidates(eng); }
                    else std::cout << "Pick 1-10.\n";
                } else { eng.RerollCandidates(); PrintCandidates(eng); }
            }
            else if (cmd == "embark" || cmd == "start" || cmd == "begin") {
                eng.StartGame();
                DrainLog(eng);
                setup = false;
                std::cout << "\nThe settlement is founded! Everyone starts Idle - assign jobs.\n";
                PrintStatus(eng);
                PrintHelp(false);
            }
            else if (cmd == "load" && words.size() > 1) {
                if (eng.LoadGame(words[1])) { setup = false; std::cout << "Loaded.\n"; PrintStatus(eng); }
                else std::cout << "Could not load slot '" << words[1] << "'.\n";
            }
            else std::cout << "Unknown setup command. Try: view <#>, reroll [#], embark, load <slot>, quit\n";
            continue;
        }

        // ------------- MAIN GAME -------------
        if (cmd == "help") PrintHelp(false);
        else if (cmd == "status") PrintStatus(eng);
        else if (cmd == "people") PrintPeople(eng);
        else if (cmd == "rivals") PrintRivals(eng);
        else if (cmd == "armory") PrintArmory(eng);
        else if (cmd == "missions") PrintStatus(eng);
        else if (cmd == "inspect" || cmd == "body") {
            std::string name = Trim(line.substr(cmd.size()));
            if (name.empty()) { std::cout << "Whom?\n"; continue; }
            int id = eng.FindPersonByName(name);
            if (id == -1) { std::cout << "Nobody called '" << name << "' lives here.\n"; continue; }
            if (cmd == "inspect") PrintInspect(eng, id);
            else PrintBody(eng.GetRegistry(), id);
        }
        else if (cmd == "build") {
            if (words.size() < 2) PrintBuildCatalog();
            else std::cout << eng.Build(words[1]) << "\n";
        }
        else if (cmd == "job") {
            if (words.size() < 3) { std::cout << "Usage: job <name> <role>\n"; continue; }
            std::string role = words.back();
            std::string name = Trim(line.substr(cmd.size(), line.rfind(words.back()) - cmd.size()));
            int id = eng.FindPersonByName(name);
            if (id == -1) { std::cout << "Nobody called '" << name << "' lives here.\n"; continue; }
            std::cout << eng.AssignJob(id, role) << "\n";
        }
        else if (cmd == "arm") {
            std::string rest = Trim(line.substr(cmd.size()));
            auto pos = Lower(rest).find(" with ");
            if (pos == std::string::npos) { std::cout << "Usage: arm <name> with <weapon>\n"; continue; }
            std::string name = Trim(rest.substr(0, pos));
            std::string weapon = Trim(rest.substr(pos + 6));
            int id = eng.FindPersonByName(name);
            if (id == -1) { std::cout << "Nobody called '" << name << "' lives here.\n"; continue; }
            std::cout << eng.AssignWeapon(id, weapon) << "\n";
        }
        else if (cmd == "disarm") {
            std::string name = Trim(line.substr(cmd.size()));
            int id = eng.FindPersonByName(name);
            if (id == -1) { std::cout << "Nobody called '" << name << "' lives here.\n"; continue; }
            std::cout << eng.UnassignWeapon(id) << "\n";
        }
        else if (cmd == "mission") {
            if (words.size() < 3) { std::cout << "Usage: mission <Hunt|Gather|Adventure> <name, name, ...>\n"; continue; }
            std::string type = words[1];
            std::string rest = Trim(line.substr(line.find(words[1]) + words[1].size()));
            std::string error;
            auto party = ParseParty(eng, rest, error);
            if (!error.empty()) { std::cout << error << "\n"; continue; }
            std::cout << eng.StartMission(type, party) << "\n";
        }
        else if (cmd == "raid" || cmd == "invade") {
            if (words.size() < 3) { std::cout << "Usage: " << cmd << " <rival#> <name, name, ...>\n"; continue; }
            int rival = std::atoi(words[1].c_str());
            std::string rest = Trim(line.substr(line.find(words[1]) + words[1].size()));
            std::string error;
            auto party = ParseParty(eng, rest, error);
            if (!error.empty()) { std::cout << error << "\n"; continue; }
            std::cout << eng.StartMission(cmd == "raid" ? "Raid" : "Invade", party, rival) << "\n";
        }
        else if (cmd == "elect") std::cout << eng.HoldElection() << "\n";
        else if (cmd == "wait" || cmd == "advance" || cmd == "run") {
            int days = words.size() > 1 ? std::atoi(words[1].c_str()) : 1;
            days = std::clamp(days, 1, 3600);
            eng.AdvanceDays(days);
            DrainLog(eng);
            if (eng.LivingPeople().empty()) {
                std::cout << "\n*** GAME OVER: the fortress has fallen. ***\n";
            } else {
                std::cout << "-- " << eng.GetDateString() << " --\n";
            }
        }
        else if (cmd == "log") {
            int n = words.size() > 1 ? std::atoi(words[1].c_str()) : 15;
            PrintLogTail(std::clamp(n, 1, 500));
        }
        else if (cmd == "save") {
            if (words.size() < 2) { std::cout << "Usage: save <slot>\n"; continue; }
            std::cout << (eng.SaveGame(words[1]) ? "Saved to SaveData/" + words[1] + ".json" : "Save failed!") << "\n";
        }
        else if (cmd == "load") {
            if (words.size() < 2) { std::cout << "Usage: load <slot>\n"; continue; }
            std::cout << (eng.LoadGame(words[1]) ? "Loaded." : "Could not load slot '" + words[1] + "'.") << "\n";
            if (eng.IsGameStarted()) PrintStatus(eng);
        }
        else std::cout << "Unknown command. Type 'help'.\n";
    }

    std::cout << "Farewell.\n";
    return 0;
}
