#include "Body.h"
#include <random>
#include <algorithm>

namespace {
    int RandInt(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

    // Helper used by factories to append a part and return its index.
    int Add(BodyComponent& b, const std::string& name, int parent, uint32_t funcs, int size, int hp) {
        BodyPart p;
        p.name = name;
        p.parent = parent;
        p.functions = funcs;
        p.size = size;
        p.hp = p.max_hp = std::max(1, hp);
        b.parts.push_back(p);
        return (int)b.parts.size() - 1;
    }

    void AddHand(BodyComponent& b, const std::string& side, int arm, float s) {
        using namespace BodyFunc;
        int hand = Add(b, side + " hand", arm, Grasp | Connector, 4, (int)(15 * s));
        Add(b, side + " thumb", hand, Grasp, 1, (int)(5 * s));
        Add(b, side + " index finger", hand, Grasp, 1, (int)(5 * s));
        Add(b, side + " middle finger", hand, Grasp, 1, (int)(5 * s));
        Add(b, side + " ring finger", hand, Grasp, 1, (int)(5 * s));
        Add(b, side + " little finger", hand, Grasp, 1, (int)(5 * s));
    }

    void AddFoot(BodyComponent& b, const std::string& side, int leg, float s) {
        using namespace BodyFunc;
        int foot = Add(b, side + " foot", leg, Stand | Connector, 4, (int)(15 * s));
        Add(b, side + " big toe", foot, Stand, 1, (int)(4 * s));
        Add(b, side + " second toe", foot, None, 1, (int)(4 * s));
        Add(b, side + " middle toe", foot, None, 1, (int)(4 * s));
        Add(b, side + " fourth toe", foot, None, 1, (int)(4 * s));
        Add(b, side + " little toe", foot, None, 1, (int)(4 * s));
    }
}

std::string WoundLevelName(WoundLevel w) {
    switch (w) {
        case WoundLevel::Fine: return "fine";
        case WoundLevel::Bruised: return "bruised";
        case WoundLevel::Cut: return "cut";
        case WoundLevel::Broken: return "broken";
        case WoundLevel::Mangled: return "mangled";
        case WoundLevel::Severed: return "severed";
        case WoundLevel::Destroyed: return "destroyed";
    }
    return "unknown";
}

BodyComponent Body::CreateHumanoid(float s) {
    using namespace BodyFunc;
    BodyComponent b;
    b.plan = "humanoid";
    b.max_blood = b.blood = 100.0f * s;

    int torso = Add(b, "upper body", -1, Breathe, 30, (int)(60 * s));
    Add(b, "heart", torso, Vital | Pump | Internal, 4, (int)(20 * s));
    Add(b, "left lung", torso, Breathe | Internal, 6, (int)(25 * s));
    Add(b, "right lung", torso, Breathe | Internal, 6, (int)(25 * s));
    Add(b, "liver", torso, Internal, 4, (int)(20 * s));
    Add(b, "spine", torso, Stand | Connector | Internal, 4, (int)(25 * s));
    int lower = Add(b, "lower body", torso, Digest, 25, (int)(50 * s));
    Add(b, "stomach", lower, Digest | Internal, 4, (int)(20 * s));
    Add(b, "guts", lower, Digest | Internal, 6, (int)(20 * s));

    int neck = Add(b, "neck", torso, Connector, 4, (int)(20 * s));
    int head = Add(b, "head", neck, Connector, 12, (int)(35 * s));
    Add(b, "brain", head, Vital | Thought | Internal, 4, (int)(15 * s));
    Add(b, "left eye", head, Sight, 1, (int)(6 * s));
    Add(b, "right eye", head, Sight, 1, (int)(6 * s));
    Add(b, "left ear", head, Hearing, 1, (int)(6 * s));
    Add(b, "right ear", head, Hearing, 1, (int)(6 * s));
    Add(b, "nose", head, Smell, 1, (int)(8 * s));
    Add(b, "mouth", head, None, 2, (int)(8 * s));

    int lArm = Add(b, "left upper arm", torso, Connector, 8, (int)(25 * s));
    int lFore = Add(b, "left lower arm", lArm, Connector, 7, (int)(22 * s));
    AddHand(b, "left", lFore, s);
    int rArm = Add(b, "right upper arm", torso, Connector, 8, (int)(25 * s));
    int rFore = Add(b, "right lower arm", rArm, Connector, 7, (int)(22 * s));
    AddHand(b, "right", rFore, s);

    int lLeg = Add(b, "left upper leg", lower, Stand | Connector, 10, (int)(30 * s));
    int lShin = Add(b, "left lower leg", lLeg, Stand | Connector, 8, (int)(25 * s));
    AddFoot(b, "left", lShin, s);
    int rLeg = Add(b, "right upper leg", lower, Stand | Connector, 10, (int)(30 * s));
    int rShin = Add(b, "right lower leg", rLeg, Stand | Connector, 8, (int)(25 * s));
    AddFoot(b, "right", rShin, s);

    return b;
}

BodyComponent Body::CreateQuadruped(float s) {
    using namespace BodyFunc;
    BodyComponent b;
    b.plan = "quadruped";
    b.max_blood = b.blood = 110.0f * s;

    int body = Add(b, "body", -1, Breathe, 40, (int)(70 * s));
    Add(b, "heart", body, Vital | Pump | Internal, 4, (int)(22 * s));
    Add(b, "left lung", body, Breathe | Internal, 5, (int)(25 * s));
    Add(b, "right lung", body, Breathe | Internal, 5, (int)(25 * s));
    Add(b, "guts", body, Digest | Internal, 6, (int)(22 * s));
    Add(b, "spine", body, Stand | Connector | Internal, 4, (int)(26 * s));

    int neck = Add(b, "neck", body, Connector, 5, (int)(22 * s));
    int head = Add(b, "head", neck, Connector, 10, (int)(32 * s));
    Add(b, "brain", head, Vital | Thought | Internal, 3, (int)(14 * s));
    Add(b, "left eye", head, Sight, 1, (int)(6 * s));
    Add(b, "right eye", head, Sight, 1, (int)(6 * s));
    Add(b, "left ear", head, Hearing, 1, (int)(6 * s));
    Add(b, "right ear", head, Hearing, 1, (int)(6 * s));
    Add(b, "snout", head, Smell, 2, (int)(10 * s));
    Add(b, "fangs", head, None, 2, (int)(12 * s));

    const char* legs[4] = {"left front leg", "right front leg", "left hind leg", "right hind leg"};
    const char* paws[4] = {"left front paw", "right front paw", "left hind paw", "right hind paw"};
    for (int i = 0; i < 4; ++i) {
        int leg = Add(b, legs[i], body, Stand | Connector, 7, (int)(24 * s));
        Add(b, paws[i], leg, Stand, 3, (int)(12 * s));
    }
    Add(b, "tail", body, None, 4, (int)(12 * s));
    return b;
}

BodyComponent Body::CreateAvian(float s) {
    using namespace BodyFunc;
    BodyComponent b;
    b.plan = "avian";
    b.max_blood = b.blood = 70.0f * s;

    int body = Add(b, "body", -1, Breathe, 35, (int)(50 * s));
    Add(b, "heart", body, Vital | Pump | Internal, 3, (int)(16 * s));
    Add(b, "lungs", body, Breathe | Internal, 5, (int)(20 * s));
    Add(b, "guts", body, Digest | Internal, 5, (int)(16 * s));

    int neck = Add(b, "neck", body, Connector, 4, (int)(14 * s));
    int head = Add(b, "head", neck, Connector, 8, (int)(20 * s));
    Add(b, "brain", head, Vital | Thought | Internal, 2, (int)(10 * s));
    Add(b, "left eye", head, Sight, 1, (int)(5 * s));
    Add(b, "right eye", head, Sight, 1, (int)(5 * s));
    Add(b, "beak", head, None, 2, (int)(12 * s));

    Add(b, "left wing", body, Flight | Connector, 12, (int)(20 * s));
    Add(b, "right wing", body, Flight | Connector, 12, (int)(20 * s));
    int lLeg = Add(b, "left leg", body, Stand | Connector, 4, (int)(14 * s));
    Add(b, "left talons", lLeg, Stand | Grasp, 2, (int)(8 * s));
    int rLeg = Add(b, "right leg", body, Stand | Connector, 4, (int)(14 * s));
    Add(b, "right talons", rLeg, Stand | Grasp, 2, (int)(8 * s));
    Add(b, "tail feathers", body, Flight, 4, (int)(8 * s));
    return b;
}

BodyComponent Body::CreateSerpent(float s) {
    using namespace BodyFunc;
    BodyComponent b;
    b.plan = "serpent";
    b.max_blood = b.blood = 80.0f * s;

    int body = Add(b, "body", -1, Breathe | Stand, 50, (int)(70 * s));
    Add(b, "heart", body, Vital | Pump | Internal, 4, (int)(18 * s));
    Add(b, "lungs", body, Breathe | Internal, 4, (int)(18 * s));
    Add(b, "guts", body, Digest | Internal, 6, (int)(18 * s));
    int head = Add(b, "head", body, Connector, 12, (int)(28 * s));
    Add(b, "brain", head, Vital | Thought | Internal, 3, (int)(12 * s));
    Add(b, "left eye", head, Sight, 1, (int)(5 * s));
    Add(b, "right eye", head, Sight, 1, (int)(5 * s));
    Add(b, "fangs", head, None, 2, (int)(12 * s));
    Add(b, "tail", body, Stand, 15, (int)(25 * s));
    return b;
}

BodyComponent Body::CreateForBeast(const std::string& beast_name, int tier) {
    float scale = 0.5f + 0.28f * (float)tier; // tier 1 ~ small dog, tier 10 ~ monster
    std::string n = beast_name;
    std::transform(n.begin(), n.end(), n.begin(), ::tolower);

    auto has = [&](const char* sub) { return n.find(sub) != std::string::npos; };
    if (has("snake") || has("serpent") || has("python") || has("wyrm") || has("centipede") || has("worm"))
        return CreateSerpent(scale);
    if (has("bird") || has("crane") || has("eagle") || has("hawk") || has("roc") || has("crow") || has("phoenix") || has("sparrow") || has("vulture") || has("owl") || has("peng") || has("bat"))
        return CreateAvian(scale);
    if (has("ape") || has("monkey") || has("demon") || has("ghoul") || has("puppet") || has("corpse") || has("yaksha"))
        return CreateHumanoid(scale);
    return CreateQuadruped(scale);
}

float Body::Capacity(const BodyComponent& body, uint32_t func) {
    float total = 0.0f;
    int count = 0;
    for (const auto& p : body.parts) {
        if (p.functions & func) {
            count++;
            if (!p.IsGone() && p.max_hp > 0)
                total += (float)std::max(0, p.hp) / (float)p.max_hp;
        }
    }
    if (count == 0) return 1.0f; // Body plan never had this function: don't penalize
    return total / (float)count;
}

bool Body::IsDead(const BodyComponent& body) {
    if (body.blood <= 0.0f) return true;
    for (const auto& p : body.parts) {
        if ((p.functions & BodyFunc::Vital) && (p.IsGone() || p.hp <= 0)) return true;
    }
    return false;
}

int Body::CountGraspingHands(const BodyComponent& body) {
    int n = 0;
    for (const auto& p : body.parts) {
        if ((p.functions & BodyFunc::Grasp) && (p.functions & BodyFunc::Connector) == 0) continue; // fingers don't count alone
        if ((p.functions & BodyFunc::Grasp) && !p.IsGone() && p.hp > 0) n++;
    }
    return n;
}

int Body::PickHitPart(const BodyComponent& body, bool through) {
    int total = 0;
    for (const auto& p : body.parts) {
        if (p.IsGone()) continue;
        if (!through && (p.functions & BodyFunc::Internal)) continue;
        total += p.size;
    }
    if (total <= 0) return -1;
    int roll = RandInt(1, total);
    for (int i = 0; i < (int)body.parts.size(); ++i) {
        const auto& p = body.parts[i];
        if (p.IsGone()) continue;
        if (!through && (p.functions & BodyFunc::Internal)) continue;
        roll -= p.size;
        if (roll <= 0) return i;
    }
    return -1;
}

std::string Body::Summarize(const BodyComponent& body) {
    std::vector<std::string> notes;
    for (const auto& p : body.parts) {
        if (p.wound == (int)WoundLevel::Fine) continue;
        notes.push_back(WoundLevelName(p.Wound()) + " " + p.name);
    }
    std::string out;
    int shown = 0;
    for (const auto& n : notes) {
        if (shown >= 4) { out += ", ..."; break; }
        if (!out.empty()) out += ", ";
        out += n;
        shown++;
    }
    if (out.empty()) out = "unharmed";
    float blood_pct = body.max_blood > 0 ? body.blood / body.max_blood : 1.0f;
    if (blood_pct < 0.5f) out += " [HEAVY BLOOD LOSS]";
    else if (blood_pct < 0.9f) out += " [bleeding]";
    return out;
}

// Marks a part and all its descendants as gone.
static void SeverSubtree(BodyComponent& body, int index, WoundLevel level) {
    body.parts[index].wound = (int)level;
    body.parts[index].hp = 0;
    body.parts[index].bleed = 0; // stump bleeding handled by caller on the root part
    for (int i = 0; i < (int)body.parts.size(); ++i) {
        if (body.parts[i].parent == index && !body.parts[i].IsGone())
            SeverSubtree(body, i, level);
    }
}

WoundResult Body::ApplyDamage(BodyComponent& body, int part_index, int damage,
                              const std::string& damage_type, const std::string& victim_name) {
    WoundResult res;
    res.part_index = part_index;
    if (part_index < 0 || part_index >= (int)body.parts.size()) {
        res.text = "the blow goes wide";
        return res;
    }
    BodyPart& p = body.parts[part_index];
    if (p.IsGone()) { res.text = "strikes at a missing " + p.name; return res; }

    p.hp -= damage;
    body.pain = std::min(100.0f, body.pain + (float)damage * 0.5f);
    float frac = (float)std::max(0, p.hp) / (float)p.max_hp;

    bool internal = (p.functions & BodyFunc::Internal) != 0;
    bool can_sever = (damage_type == "slash") && !internal;
    bool overkill = p.hp <= -(p.max_hp / 2);

    if (p.hp <= 0) {
        if (can_sever && (p.functions & BodyFunc::Connector) && overkill) {
            // Limb comes off, everything attached goes with it
            SeverSubtree(body, part_index, WoundLevel::Severed);
            p.wound = (int)WoundLevel::Severed;
            p.bleed = 4.0f + (float)p.size * 0.3f;
            res.severed = true;
            res.text = victim_name + "'s " + p.name + " is SEVERED, flying off in an arc of blood!";
        } else {
            p.wound = (int)WoundLevel::Mangled;
            if (overkill && damage_type == "blunt") p.wound = (int)WoundLevel::Destroyed;
            p.bleed = internal ? 5.0f : 2.0f + (float)p.size * 0.15f;
            res.text = victim_name + "'s " + p.name + " is " +
                       (p.wound == (int)WoundLevel::Destroyed ? "crushed into pulp!" : "mangled beyond use!");
        }
        if (p.functions & BodyFunc::Vital) {
            res.fatal = true;
            res.text += " It is a fatal wound.";
        }
    } else if (frac < 0.35f) {
        p.wound = std::max(p.wound, (int)(damage_type == "blunt" ? WoundLevel::Broken : WoundLevel::Mangled));
        p.bleed = std::max(p.bleed, damage_type == "blunt" ? 0.5f : 2.0f);
        res.text = victim_name + "'s " + p.name + " is " + (damage_type == "blunt" ? "broken!" : "torn open, gushing blood!");
    } else if (frac < 0.7f) {
        p.wound = std::max(p.wound, (int)(damage_type == "blunt" ? WoundLevel::Bruised : WoundLevel::Cut));
        p.bleed = std::max(p.bleed, damage_type == "blunt" ? 0.0f : 1.0f);
        res.text = victim_name + " is " + (damage_type == "blunt" ? "bruised on the " : "cut across the ") + p.name + ".";
    } else {
        res.text = victim_name + " is grazed on the " + p.name + ".";
    }
    return res;
}

bool Body::DailyTick(BodyComponent& body, float heal_mult) {
    // Bleeding
    float total_bleed = 0.0f;
    for (auto& p : body.parts) total_bleed += p.bleed;
    body.blood -= total_bleed;

    // Clotting: wounds close over time, faster with care
    for (auto& p : body.parts) {
        if (p.bleed > 0.0f) p.bleed = std::max(0.0f, p.bleed - 0.8f * heal_mult);
        // Healing: severed/destroyed parts never come back
        if (!p.IsGone() && p.hp < p.max_hp) {
            p.hp = std::min(p.max_hp, p.hp + (int)(1 + 2 * heal_mult));
            float frac = (float)p.hp / (float)p.max_hp;
            if (frac >= 1.0f) p.wound = (int)WoundLevel::Fine;
            else if (frac >= 0.7f && p.wound > (int)WoundLevel::Cut && p.wound < (int)WoundLevel::Severed)
                p.wound = (int)WoundLevel::Cut;
        }
    }
    // Blood regeneration (only when not actively bleeding much)
    if (total_bleed < 1.0f && body.blood < body.max_blood)
        body.blood = std::min(body.max_blood, body.blood + 2.0f * heal_mult);
    body.pain = std::max(0.0f, body.pain - 10.0f);

    return body.blood <= 0.0f;
}
