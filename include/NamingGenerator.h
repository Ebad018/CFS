#pragma once
#include <string>
#include <vector>
#include <cstdlib>

class NamingGenerator {
public:
    // --- 1. Surnames (Clan Names) ---
    static std::string GenerateSurname() {
        static std::vector<std::string> surnames = {
            "Ye", "Li", "Wang", "Zhang", "Chen", "Liu", "Yang", "Huang", "Zhao", "Wu", // Common
            "Long", "Feng", "Bai", "Hei", "Mo", "Gu", "Tie", "Shi", "Murong", "Nangong", "Sima", "Dugu", "Xue", "Lei", "Yun" // Rare/Epic
        };
        return surnames[rand() % surnames.size()];
    }

    // --- 1.2 Given Names ---
    static std::string GenerateGivenName(bool male = true) {
        static std::vector<std::string> male_names = {
            "Chen", "Feng", "Wei", "Jian", "Lei", "Huo", "Gang", "Ming", "Tao", "Jun", 
            "Tian", "Shan", "Hai", "Yi", "Bo"
        };
        static std::vector<std::string> female_names = {
            "Yan", "Ling", "Xue", "Yue", "Hua", "Lan", "Qing", "Mei", "Xin", "Yu", 
            "Lian", "Shuang", "Ying", "Zhu", "Rou"
        };
        
        if (male) return male_names[rand() % male_names.size()];
        else return female_names[rand() % female_names.size()];
    }

    static std::string GenerateNPCName(bool male = true) {
        return GenerateSurname() + " " + GenerateGivenName(male);
    }

    // --- 2. Sect Naming ---
    static std::string GenerateSectName() {
        static std::vector<std::string> prefixes = {
            "Xuan", "Tian", "Xue", "Zi", "Qing", "Jin", "Han", "Lie", "You", "Ling", "Ku", "Tai", "Chi", "Bai", "Yin"
        };
        static std::vector<std::string> cores = {
            "Jian", "Dao", "Dan", "Qi", "Shou", "Long", "Feng", "Hu", "Gui", "Xing", "Yue", "Ri", "Yun", "Lei", "Hai", "Shan", "Gu", "Hun"
        };
        static std::vector<std::string> suffixes = {
            "Zong", "Men", "Gu", "Ge", "Dian", "Tang", "Dong", "Shan", "Yuan", "Jiao", "Bang", "Lou"
        };

        std::string p = prefixes[rand() % prefixes.size()];
        std::string c = cores[rand() % cores.size()];
        std::string s = suffixes[rand() % suffixes.size()];
        
        return p + " " + c + " " + s;
    }

    // --- 3. Dynamic Title Generation ---
    // In a real system, this would take an Entity& and check stats.
    // For now, we provide a helper to generate random cool titles for population.
    static std::string GenerateTitleRandom() {
        static std::vector<std::string> adjectives = {
            "Blood-Soaked", "Merciless", "Iron", "Swift", "Divine", "Vile", "One-Armed", "Undying", "Wealthy", "Crazed", "Jade", "Ghost", "Thunder", "Frost", "Blazing", "Void"
        };
        static std::vector<std::string> nouns = {
            "Butcher", "Slayer", "Hunter", "Emperor", "Scholar", "Sage", "Demon", "Fairy", "Goddess", "Ancestor", "Venerable", "Prodigy", "Trash", "Thief", "Traitor", "Warlord"
        };
        
        return adjectives[rand() % adjectives.size()] + " " + nouns[rand() % nouns.size()];
    }
};
