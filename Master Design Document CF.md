# **Master Design Document: Cultivation Fortress**

Project Title: Cultivation Fortress

Genre: Text-Based Simulation / Roguelike / Strategy

Core Philosophy: "Numbers are Graphics." A world where depth, history, and simulation complexity take precedence over visual fidelity.

Target Experience: The narrative emergence of Dwarf Fortress combined with the power progression of Xianxia (Cultivation) novels.

---

## **1\. Technical Architecture & Tech Stack**

The engine is designed for high-performance simulation, capable of handling a 10,000-year history generation phase and thousands of active entities.

### **1.1 Core Technology**

* **Language:** **C++ (C++20)**. Chosen for manual memory management to prevent garbage collection stutters during massive simulation ticks.  
* **Windowing & Input:** **SFML** (Simple and Fast Multimedia Library). Handles the OS window, keyboard/mouse polling, and basic shape rendering for the map grid.  
* **UI Framework:** **Dear ImGui** (via ImGui-SFML). Used for the "Command Center" interface (text logs, stat panels, menus).  
* **Data Persistence:** **SQLite (Embedded)**.  
  * *Usage:* Stores the "History Log" (dead ancestors, past wars), World Chunks (offloaded from RAM), and Entity Relationships.

    ### **1.2 The Simulation Loop (Roguelike Time)**

The game operates on a **Synchronous Input-Driven Tick System**.

1. **State: IDLE:** The engine renders the UI at 60 FPS, but the World Simulation is paused.  
2. **Trigger:** Player input (Move W, Cultivate Space).  
3. **State: UPDATE:**  
   * **Player Tick:** Action points consumed. Stats (Hunger/Qi) updated.  
   * **Global Tick:** SimEngine::AdvanceTime(1).  
   * **NPC AI:** Local entities move; distant Sects calculate resource gathering/diplomacy.  
4. **State: RENDER:** The viewport updates tile colors and the narrative text log.

   ---

   ## **2\. World Design & Environment**

The world is a **Universe Container** consisting of vertically stacked MapLayers generated via Perlin Noise and Cellular Automata.

### **2.1 The Multiverse Architecture**

* **Void Layer (+9):** High-level hazard zone containing the **Chaos Dragon (GB\_01)** and **Void Shades**.  
* **Sky Layers (+1 to \+8):** Floating islands inhabited by **Sky-Winged Celestials**.  
* **Mortal Plane (0):** The primary world containing standard Biomes (Forest, Desert, Spirit Veins) and **Human** civilizations.  
* **Demon Abyss (-1 to \-9):** Underground layers with "Miasma" hazards, home to **Giant Demons** and **Ghost Butterflies**.

  ### **2.2 Procedural Generation Pipeline**

1. **Topography:** Generates Height, Moisture, and Qi Density maps.  
2. **Biome Resolution:** Assigns biomes (e.g., *Volcanic Waste*, *Bamboo Forest*) based on noise overlap.  
3. **Civilization:** Places **Sect Cores** on high-Qi tiles and **Cities** on low-Qi plains.  
4. **Population:** Spawns mobs and resources using the **Master Bestiary** and **Resource Database** loot tables.

   ---

   ## **3\. Gameplay Systems**

   ### **3.1 Cultivation & Progression**

* **Realms:** Progression from *Mortal* $\\to$ *Qi Condensation* $\\to$ *Foundation Establishment* $\\to$ *Golden Core* $\\to$ *Nascent Soul* $\\to$ *Saint*.  
* **Seclusion Engine:** A mechanic allowing the player to fast-forward time (years/decades) to convert Resources into XP.  
* **Roots & Affinity:** Players roll for elemental affinities (Fire, Void, etc.) which determine accessible *Cultivation Methods*.

  ### **3.2 Sect Mechanics**

* **Hierarchy:** Players climb ranks from *Outer Disciple* to *Sect Leader*. Higher ranks unlock commands (e.g., ordering NPC squads).  
* **Economy:** Sects have a "Vault" of resources (ORE/HRB). NPCs automatically form parties to mine resources or hunt beasts to refill the vault.  
* **Warfare:** Sects declare war based on "Relations" and "Resource Scarcity."

  ### **3.3 Dynamic Events**

* **Beast Tides:** Massive vector-based movements of monster swarms targeting cities.  
* **Inheritance Grounds:** Temporary dungeons generated for specific age/cultivation groups (e.g., "Under 50 years old only").

  ---

  ## **4\. Entity Component System (ECS)**

Entities are data containers composed of modular components, defined by external data files.

### **4.1 Core Components**

* **Identity:** Name (NameGen), Title (Reputation), Race (RACE\_ID).  
* **Stats:** HP, Qi, Hunger, Lifespan, Foundation Integrity.  
* **Cultivation:** Current Realm, XP, Root Quality.  
* **AI:** Ambition Goal (e.g., "Reach Golden Core"), Relationships (Parents/Rivals).

  ### **4.2 Data Driven Definitions**

The engine parses Markdown/CSV tables at startup to build the object database:

* **Races:** 12 definitions defining base stats (e.g., RACE\_04 Demon has Str \* 2.0).  
* **Items:** 160+ definitions for Ores, Herbs, and Artifacts.  
* **Bestiary:** 80 unique creatures with specific behaviors and drops.  
* **Skills:** 80+ Martial Arts and Methods linked to specific weapon types.

  ---

  ## **5\. User Interface (The Dashboard)**

The UI is divided into dockable panels:

1. **World View:** The central grid rendering the map tiles and entities.  
2. **Narrative Log:** A scrolling text history of combat and events (*"Ye Chen hits the Tiger for 50 damage"*).  
3. **Character Sheet:** Real-time stats (Qi/HP bars), current Realm, and Inventory.  
4. **Interaction Menu:** Context-sensitive buttons (Cultivate, Attack, Talk, Craft).

   ---

   ## **6\. Narrative & Legacy**

   ### **6.1 Procedural Storytelling**

* **Naming Engine:** Uses semantic tokens (e.g., "Ye" \+ "Chen" \= "Leaf Dawn") to generate meaningful names.  
* **Title Evolution:** NPCs gain titles based on actions (e.g., Killing 100 Wolves $\\to$ "Wolf Butcher").

  ### **6.2 The Roguelike Cycle**

* **Death:** Permanent.  
* **Succession:** The player can choose to reincarnate in a new world or possess a descendant in the *same* world after a 100-year time skip, allowing them to see the long-term consequences of their previous life.

  ---

  ## **7\. Sub-Document Reference Map**

For implementation details, refer to the specific module documents:

* **World Gen:** Technical Design Document: World Generation & Environment (v2.md  
* **Sect AI:** Game Design Document: Sect Mechanics & Hierarchy.md  
* **Player Loop:** Game Design Document: Player Experience & Living World.md  
* **Data Tables:**  
  * Master Bestiary List.md  
  * Design Document: Master Resource & Asset Database.md  
  * Design Document: Master Weapon Database (Polearms & Blades).md  
  * Design Document: Master Cultivation & Skills.md  
  * Design Document: Playable & NPC Races.md  
    

