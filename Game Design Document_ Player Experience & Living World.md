# **Game Design Document: Player Experience & Living World**

Project: Cultivation Fortress

Core Concept: A simulation-driven RPG where the world evolves while the player meditates.

Primary Loop: Cultivate (Seclusion) $\\rightarrow$ Compete (Sect Politics) $\\rightarrow$ Explore (Adventure) $\\rightarrow$ Transcend or Die.

---

## **1\. The Player Lifecycle (The Macro Loop)**

### **1.1 Initialization (The "Transmigration")**

* **Spawn Logic:** The player is dropped into a procedurally generated world.  
* **The "Talent Roll":** Before spawning, the player rolls for their **Root Quality**:  
  * *Heavenly Spirit Root:* 50% Exp Bonus (Very Rare).  
  * *Dual Element Root:* 20% Exp Bonus (Uncommon).  
  * *Mortal Root:* Baseline Speed (Common).  
* **Sect Assignment:** The WorldGen selects a Random Sect (e.g., "Mystic Sword Valley") and assigns the player as an **Outer Disciple**.  
* **Starter Package:**  
  * 1x WPN\_SWD\_01 (Iron Wolf Sword).  
  * 1x MTH\_GEN\_01 (Basic Breathing Scripture).  
  * 10x ORE\_005 (Spirit Stones).

### **1.2 The Seclusion Engine (Time Management)**

This is the heart of the game.

* **Action:** Player selects "Enter Seclusion" in their quarters.  
* **Input:** Player sets duration (e.g., "Cultivate for 5 Years").  
* **Simulation:** The screen fades. The SimEngine runs at max speed.  
  * **Player:** Gains XP per tick based on Qi\_Regen. Hunger increases.  
  * **World:** NPCs age, fight, die, and rank up. Sects go to war.  
  * **Interrupts:** Seclusion breaks if:  
    * Hunger reaches 100% (Starvation).  
    * Sect is attacked.  
    * A "Heart Demon" event occurs.

### **1.3 Death & Legacy (The Roguelike Element)**

When HP hits 0:

* **Option A: Reincarnate (New World):** Generates a fresh map seed.  
* **Option B: Possess/Descendant (Same World):**  
  * **Time Skip:** The engine fast-forwards **100 Years**.  
  * **Consequence:** The Sect Leader you knew is dead. Your rival might be the Grand Elder. The Sect might have been destroyed and is now ruins.  
  * **New Character:** You spawn as a fresh disciple in this "Future State" of the world.

---

## **2\. Sect Hierarchy & Politics (The "Job")**

The Sect is an entity with limited resources. You must fight for your share.

### **2.1 The Ladder**

| Rank | Requirement | Monthly Stipend | Privileges |
| :---- | :---- | :---- | :---- |
| **Servant** | Mortal | Food Only | Mining/Farming duties. No library access. |
| **Outer Disciple** | Qi Condensation (1-3) | 5 Spirit Stones | Access to Tier 1 Skills. |
| **Inner Disciple** | Qi Condensation (4-9) | 20 Stones \+ 1 Pill | Access to Tier 2 Skills. Private Cave. |
| **Core Disciple** | Foundation Establishment | 100 Stones \+ Rare Mats | Can accept "Sect Missions." |
| **Holy Son/Saintess** | Top Ranked Core | 500 Stones \+ Artifact | Heir to the Leader. Access to "Forbidden Area." |
| **Elder** | Golden Core | Share of Sect Profit | Can recruit Disciples. |
| **Sect Leader** | Nascent Soul | Control of Vault | Declares War/Alliances. |

### **2.2 Proving Your Worth**

* **The Monthly Tournament:** An auto-generated bracket tournament.  
  * *Reward:* Winning grants "Contribution Points" and ranking promotion.  
  * *Rivalry:* Beating an NPC here generates a "Grudge" relationship.  
* **Contribution Points:** Earned by donating resources (Herbs/Ores) or completing missions. Used to "buy" techniques from the Sect Library (e.g., buying SKL\_SWD\_02 Flowing Water Strike).

---

## **3\. Player Stats & Progression**

### **3.1 Primary Attributes**

* **Lifespan:** The hard limit. If you don't breakthrough before this hits 0, you die of old age.  
* **Hunger:** Must eat Food (HRB\_013) or Fasting Pills (CSM\_01).  
* **Cultivation Base:** The XP bar. Fills via Seclusion or consuming Cores.  
* **Foundation Integrity:** (0.0 \- 1.0). Determined by the quality of resources used during breakthroughs. Affects max stats.

### **3.2 Esoteric Stats**

* **Elemental Affinity:** (Fire, Water, Wood, etc.). Determines which Manuals you can learn.  
* **Martial Intent:** (e.g., Sword Intent). Gained by using the same weapon type repeatedly. Adds flat damage % to that weapon type.  
* **Dao Grasp:** (Space, Time, Karma). Only unlockable at high realms. Required for God-Tier breakthroughs.  
* **Ambition:** A tracker for the player's goal (e.g., "Destroy the Blood Sect"). Completing it grants massive "Mental State" bonuses.

---

## **4\. Combat & Exploration**

### **4.1 Exploration Mode**

* **Travel:** Moving between tiles consumes "Days."  
* **Scanning:** Use SKL\_UTL\_01 (Spirit Eye) to reveal nearby resource nodes or hidden dungeons.  
* **Lucky Chances:** Random events based on "Luck" stat.  
  * *Example:* "You fall into a crevice and find the skeleton of an ancient senior holding a MAN\_005 (Star-Fall Sword Array) manual".

### **4.2 Turn-Based Battle System**

When encountering an Entity:

1. **UI:** Displays Enemy Name, Realm, and "Danger Level" (Trivial, Even, Deadly).  
2. **Player Options:**  
   * **\[Attack\]:** Select a Martial Skill (e.g., SKL\_SWD\_04 Vacuum Blade). Consumes Qi.  
   * **\[Item\]:** Use a consumable (e.g., CSM\_02 Healing Pill) or a Talisman (CSM\_10 Escape Talisman).  
   * **\[Talk\]:** Attempt Diplomacy (Relies on Charisma/Reputation).  
   * **\[Flee\]:** Speed check vs Enemy Speed.  
3. **Resolution:** Text log describes the exchange. Damage is applied.

---

## **5\. Economy & Crafting (NPC Interaction)**

The player is a cultivator, not a laborer. Crafting is done via services.

### **5.1 The Refiner / Blacksmith**

* **Location:** Found in Cities or Sect "Refining Halls."  
* **Interaction:**  
  1. **Provide Blueprint:** Select target weapon (e.g., WPN\_SPR\_04 Lightning Rod).  
  2. **Provide Materials:** Inventory must contain MAT\_035 (Thunder Horn).  
  3. **Pay Fee:** Cost in Spirit Stones.  
  4. **Wait:** Crafting takes in-game days.

### **5.2 The Auction House**

A monthly event in major cities.

* **Bidding War:** Rare items (e.g., GOD\_002 Phoenix Essence) appear.  
* **NPC AI:** "Young Masters" (Arrogant NPCs) will bid irrationally high just to spite the player.  
* **Heist Option:** Player can choose to *not* bid, mark the winner, and ambush them outside the city (Risk: High Reputation Loss/Pursuit).

---

## **6\. NPC Design (The Ambition System)**

NPCs are not static quest givers. They are playing the game too.

### **6.1 NPC Data Structure**

* **Archetype:** Defines personality.  
  * *Righteous:* Hates demonic cultivators. Loyal.  
  * *Demonic:* Kills for loot. Betrays easily.  
  * *Arrogant:* High "Face" requirement. Easily offended.  
* **Relationships:**  
  * *Parents/Master:* Will hunt the player if the NPC is killed.  
  * *Friends:* Will join battles to help.  
* **Ambition Engine:** Every NPC has a current goal.  
  * *Goal:* "Reach Foundation Establishment."  
  * *Action:* NPC goes to a "Secret Realm" to find herbs.  
  * *Interaction:* Player meets NPC in the dungeon. They compete for the same herb.

### **6.2 Dynamic Titles**

NPC names evolve based on their history log.

* *Start:* "Disciple Wang."  
* *Event:* Wang kills a SD\_01 (Iron-Back Wolf).  
* *New Title:* "Wolf-Slayer Wang."  
* *Event:* Wang reaches Golden Core.  
* *New Title:* "Golden Wolf Elder."

### **6.3 The "Young Master" Loop**

1. **Offense:** Player wins an auction or refuses to give up a seat.  
2. **Threat:** NPC "Mo Long" (Arrogant) demands an apology.  
3. **Escalation:** If Player refuses, Mo Long attacks.  
4. **Consequence:**  
   * If Player kills Mo Long \-\> Mo Long's Father (Elder) adds Player to "Kill List."  
   * If Player spares Mo Long \-\> Mo Long gains "Humiliated" trait and trains 2x faster to seek revenge.

