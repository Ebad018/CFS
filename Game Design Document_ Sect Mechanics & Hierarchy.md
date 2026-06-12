# **Game Design Document: Sect Mechanics & Hierarchy**

Project: Cultivation Fortress

Scope: Faction Management, NPC AI Commands, World Layer Navigation, and Dynamic Warfare.

Dependencies: Requires MDD, Bestiary, Resource\_DB, and NPC\_Races to function.

---

## **1\. World Layer Navigation & Discovery**

To access the expanded world (Abyss/Sky/Void), players and NPCs must find specific "Gateways" generated during World Gen.

### **1.1 Layer Entrances & Clues**

Instead of a menu option, access is physical and requires exploration or information gathering.

| Layer | Entrance Type | Visual/Text Hint | Key Item/Requirement |
| :---- | :---- | :---- | :---- |
| **Sky Layer (+1)** | **Ascension Pedestal** | "A massive pillar of stone reaching above the clouds. Spiritual pressure crushes the weak." | Requires **Flight** (Tier 4 or SD\_34 Cloud Horse). |
| **Demon Abyss (-1)** | **Abyssal Vent** | "A tear in the earth emitting black fog. You hear the screams of RACE\_04 (Demons)." | Requires **Yang-Body** or CSM\_04 (Detox Pill) to survive the miasma. |
| **Deep Sea Trench** | **Whirlpool** | "A permanent vortex in the ocean. The water turns pitch black." | Requires **Water Affinity** or HRB\_037 (Air Bubble Algae). |
| **Void Layer (+9)** | **Spatial Rift** | "The air here shimmers like broken glass. Space is unstable." | Requires CORE\_027 (Chameleon Void Core) or SKL\_MOV\_03 (Blood Escape) to enter. |

### **1.2 Rumor System**

Players can acquire coordinates for these entrances by:

* **Interrogating NPCs:** Using SKL\_UTL\_02 (Soul Search) on defeated Elders.  
* **Sect Archives:** Spending "Contribution Points" to buy a Map Fragment.

---

## **2\. Hierarchy & Command System**

The Player's rank determines their control over the AI.

### **2.1 Rank Permissions Table**

| Rank | Authority Level | Available Commands |
| :---- | :---- | :---- |
| **Outer Disciple** | None | Can only accept missions. Interactions are limited to "Trade" or "Chat." |
| **Inner Disciple** | Low | Can hire **Outer Disciples** as temporary bodyguards (Cost: Spirit Stones). |
| **Core Disciple** | Medium | Can form a **Party** (Squad). Inner Disciples will follow "Move" and "Attack" orders without payment (Reputation check). |
| **Elder** | High | Can issue **Sect Directives** (e.g., "Patrol Sector A", "Gather Iron Ore"). Access to **Discipline Hall**. |
| **Sect Leader** | Absolute | Full control of **Vault**, **Diplomacy**, and **Personnel**. Can Banish/Promote anyone. |

### **2.2 The Elder Roles (Management)**

As a Sect Leader (or when interacting with them), these specific roles govern the Sect's AI efficiency.

1. **Discipline Elder (Policing):**  
   * **Function:** Automatically detects "Traitor" flags (Negative Loyalty).  
   * **Action:** Dispatches "Enforcement Squads" to hunt down players/NPCs who betrayed the sect.  
   * **Interaction:** Player can bribe them to ignore crimes.  
2. **Resource Elder (Logistics):**  
   * **Function:** Manages the **Vault**. Auto-generates "Collection Missions" when stocks of ORE or HRB are low.  
   * **Action:** Sends NPC parties to Mine tiles.  
3. **Transmission Elder (Library):**  
   * **Function:** Curates the MTH (Cultivation Methods) available to disciples.  
   * **Action:** If the Sect finds a high-tier manual like MAN\_005 (Star-Fall Array), this Elder decides if Outer Disciples are allowed to learn it.

---

## **3\. Sect Economy & Expeditions**

### **3.1 Autonomous Resource Gathering**

NPCs do not stay in the base. They form parties and head out.

* **Trigger:** Resource Elder posts a quest for ORE\_008 (Star Steel).  
* **Execution:** A squad (1 Elder \+ 4 Disciples) spawns and moves toward a **Impact Site** biome.  
* **Conflict:** If they meet a rival party or a beast like SD\_36 (Silver-Winged Ant), combat occurs.  
* **Resolution:**  
  * *Success:* They return. 80% of resources go to **Sect Vault**. 20% split among the squad.  
  * *Failure:* Squad wiped out. Sect power decreases.

### **3.2 Leader Vault Access**

If the Player is the Sect Leader:

* **Inventory:** You can open the "Sect Vault" menu. It acts as an Infinite Storage chest containing the collective wealth of centuries.  
* **Usage:** You can withdraw GOD\_006 (Blessing Elixir) for personal use, but this lowers **Sect Morale** massively (Corruption).

---

## **4\. Dynamic World Events: Beast Tides**

Beast Tides are vector-based simulation events, not just random spawns.

### **4.1 Tide Generation**

1. **Origin:** A high-danger zone (e.g., "Demon Abyss").  
2. **Vector:** The AI selects a random direction (e.g., "North-East").  
3. **Composition:** Thousands of SD\_xx mobs led by a SB\_xx (Saint Beast) Boss.

### **4.2 The Path of Destruction**

* **Movement:** The Tide moves 1 Chunk per day.  
* **Simulation:**  
  * *Vs Wilderness:* Local wildlife is killed/absorbed.  
  * *Vs Town:* Town is destroyed. Population \= 0\.  
  * *Vs Sect:* **Siege Event** triggers.

### **4.3 Defense Mechanics**

When a Tide hits the Player's Sect:

* **Defense Options (Leader):**  
  * *Activate Great Array:* Consumes CORE\_003 (Boar Earth Cores) from Vault to create a shield.  
  * *Sortie:* Deploy Elders to assassinate the Beast Leader.  
  * *Evacuate:* Abandon the Sect tile (Game Over for the Sect, Player survives as Ronin).  
* **Loot:** After a successful defense, the battlefield is littered with thousands of **Beast Cores** and **Materials**, resulting in an economic boom.

---

## **5\. Karma, Loyalty, and Treason**

### **5.1 The Call to Arms**

When a War or Beast Tide starts, the Sect Leader issues a **"Sect Summons."**

* **Player Choice:**  
  * *Accept:* Teleport to Sect. Participate in battle. Gain **Contribution** and **Loyalty**.  
  * *Ignore:* Stay in seclusion/adventure. **Loyalty \-20**.  
  * *Defect:* Attack own Sect members. **Loyalty \-100**.

### **5.2 Traitor Status**

If Loyalty drops below \-50:

* **Expulsion:** Player loses access to Library, Vault, and Cave. Title changes to "Rogue Cultivator."  
* **Kill Order:** The **Discipline Elder** dispatches Hunter NPCs. These NPCs track the player across the map using SKL\_UTL\_01 (Spirit Eye).  
* **Redemption:** Player can join a rival Sect (Enemy of my Enemy) or pay a massive fine to the Sect Leader (if Corruption is high).

---

## **6\. Founding & Ronin Gameplay**

### **6.1 The Ronin (Sectless) Path**

* **Progression:** Harder. Must buy manuals at **Auction Houses** or find them in **Dungeons**.  
* **Recruitment:** Can join a new Sect by winning their **Entry Tournament** (Stat check).

### **6.2 Founding a Sect**

To transition from Player to Leader:

1. **Territory:** Clear a tile of all hostile mobs (must be a Spirit Vein tile for Qi).  
2. **Resources:** \* 10,000 ORE\_005 (Spirit Stones).  
   * 500 ORE\_034 (Limestone) for construction.  
   * 1 **Sect Core** (e.g., GOD\_001 Chaos Heart or SB\_02 Wyrm Ice Crystal) to power the main array.  
3. **Naming:** Use the Semantic Token generator (e.g., "Chaos" \+ "Dragon" \+ "Pavilion").  
4. **Recruitment:** Post notices in Towns. NPCs will join based on the Player's **Reputation**.

