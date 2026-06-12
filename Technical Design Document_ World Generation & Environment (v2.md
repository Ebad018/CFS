# **Technical Design Document: World Generation & Environment (v2.0)**

Project: Cultivation Fortress

System: Procedural World Generator (PWG)

Scope: Multiverse Layers, Biome Ecology, Civilization Layouts, and Dynamic Dungeons.

Dependencies: Bestiary, Resource\_DB, Races\_DB.

---

## **1\. The Multiverse Architecture (The "Z-Axis")**

The world is a vertical stack of **Planes**. Movement requires specific "Gateways" or high-level Spells.

### **1.1 Layer Definitions & Ecology**

| Layer Index | Name | Environmental Hazard | Native Races/Entities | Key Resources |
| :---- | :---- | :---- | :---- | :---- |
| **\+9** | **Void Layer** | **Spatial Shear:** Deals 50 Dmg/Tick. Requires Space Law or CORE\_027 (Void Core) to negate. | **Void Shades (RACE\_10)**, **Chaos Dragon (GB\_01)**. | ORE\_022 (Void Crystal), HRB\_028 (Chaos Vine). |
| **\+5 to \+8** | **High Heaven (Sky)** | **Thin Air:** Qi Recovery \-50%. Stamina Drain x2. Flight required. | **Sky-Winged Celestials (RACE\_08)**, **Solar Crow (GB\_07)**. | ORE\_024 (Heaven Gold), HRB\_018 (Star Dust Pollen). |
| **\+1 to \+4** | **Spirit Peaks** | **None.** High Qi Density. | **Spirit-Wood Fey (RACE\_05)**, **Vermillion Crane (SB\_16)**. | HRB\_008 (Sky Lotus), CORE\_023 (Thunder Goat Core). |
| **0** | **Mortal Plane** | **None.** Standard Physics. | **Humans (RACE\_01)**, **Fox-Kin (RACE\_09)**. | Standard Ores/Herbs. |
| **\-1 to \-4** | **Underdark (Caves)** | **Darkness:** Vision Radius \= 2 tiles. | **Stone Spirits (RACE\_07)**, **Earth Dragon (SB\_04)**. | ORE\_009 (Mithril), ORE\_031 (Saltpeter). |
| **\-5 to \-9** | **Demon Abyss** | **Miasma:** Toxicity \+1/Tick. Requires CSM\_04 (Detox Pill). | **Giant Demons (RACE\_04)**, **Ghost Butterfly (GB\_08)**. | HRB\_030 (Demon Face Root), ORE\_025 (Abyss Iron). |
| **Special** | **Deep Sea** | **Pressure:** Crushing Dmg if Body Tier \< 3\. | **Octomen (RACE\_06)**, **Black Turtle (GB\_03)**. | CORE\_006 (Blue Gold), HRB\_037 (Air Bubble Algae). |

---

## **2\. Spirit Realms (Pocket Dimensions)**

These are **Temporary Maps** generated strictly on-demand. They do not persist when the player leaves (unless they own the realm).

### **2.1 Generation Triggers (The "When & Why")**

1. **Item Trigger:** Player uses a map fragment or key item (e.g., MAT\_039 Void Scale).  
   * *Result:* Spawns a portal to a "Treasure Grotto" (Size: 32x32).  
2. **Time Trigger:** Every 10 Years, the "Ancient Battlefield" opens for 1 month.  
   * *Why:* To create a "Gold Rush" event where all Sects send disciples.  
3. **Event Trigger:** A GB\_05 (White Tiger) dies. Its corpse generates a "Blood Spirit Realm" due to massive energy leakage.

### **2.2 Realm Archetypes**

* **Herb Garden:** High spawn rate of HRB\_015 (Seven-Color Fruit). Guarded by SD\_35 (Parasitic Vines).  
* **Trial Tower:** 10 floors of combat. No resource nodes. Loot chest at the end containing MAN\_005 (Secret Manuals).  
* **Broken World:** Shattered terrain requiring SKL\_MOV\_01 (Cloud Step) to navigate. High Void hazards.

---

## **3\. Biome Ecosystems & Connections**

Each Biome on the **Mortal Plane (Layer 0\)** is a container for specific asset lists.

### **3.1 Biome Mapping Matrix**

| Biome | Terrain Type | Flora (Herbs) | Fauna (Mobs) | Native Race Settlement |
| :---- | :---- | :---- | :---- | :---- |
| **Bamboo Forest** | Dense Vegetation | HRB\_031 (Bamboo Shoot), HRB\_006 (Iron-Bark) | SB\_09 (Sword Mantis), SD\_18 (Iron Panda) | **Fey (RACE\_05)** |
| **Volcanic Waste** | Lava/Ash | HRB\_009 (Fire Petal), ORE\_030 (Sulfur) | SD\_31 (Magma Beetle), SB\_08 (Ghost Shark) | **Phoenix Scions (RACE\_02)** |
| **Frozen Tundra** | Ice/Snow | HRB\_011 (Ice Crystal), ORE\_006 (Cold Iron) | SD\_45 (Glacial Bear), SB\_02 (Cryo Wyrm) | **Humans (Northern Tribes)** |
| **Swamp/Marsh** | Water/Mud | HRB\_007 (Mist Mushroom), HRB\_035 (Spores) | SD\_42 (Mosquito Swarm), SD\_30 (Arrow Frog) | **Fox-Kin (RACE\_09)** |
| **Spirit Vein** | Glowing Soil | HRB\_001 (Spirit Grass), ORE\_005 (Spirit Stone) | SD\_06 (Spirit Rat), GB\_06 (Qilin) | **Major Sects (Any Race)** |

---

## **4\. Civilization Architecture: Sects & Cities**

Cities and Sects are procedurally generated structures with hierarchies.

### **4.1 Sect Grading System (1-9 Scale)**

The Grade determines the Sect's population, resource output, and defensive arrays.

* **Grade 9 (Iron):** Population \< 50\. Controls 1 Village. No Array.  
  * *Leader:* Foundation Establishment.  
* **Grade 5 (Silver):** Population \~500. Controls 1 City \+ 2 Mines.  
  * *Leader:* Nascent Soul. *Defense:* "Cloud-Locking Array" (Blocks entry).  
* **Grade 1 (Saint):** Population \> 5000\. Controls a Continent.  
  * *Leader:* Saint/God Tier. *Defense:* "World-Suppressing Formation" (Insta-kills intruders).  
  * *Feature:* Has a **Gateway** to a private Spirit Realm.

### **4.2 Racial Layouts**

* **Human City:** Walled grid. Central "Palace". 4 Districts (Market, Slum, Residential, Noble).  
* **Merfolk (RACE\_06) City:** Vertical layout. Coral towers connected by swim-lanes. No walls (Ocean is the wall). Located in **Deep Sea**.  
* **Demon (RACE\_04) Camp:** Circular layout. Central "Fighting Pit" (Arena). Huts made of SD\_03 (Boar Skin). Located in **Abyss**.  
* **Celestial (RACE\_08) Spire:** Single massive tower on a floating island. Access requires Flight.

---

## **5\. Adventure Points (POIs)**

Points of Interest are spawned during generation or dynamically triggered.

### **5.1 Inheritance Grounds (The "Young Elite" Zone)**

* **Concept:** Ancient seniors leave their legacy behind, but place restrictions to find a "worthy successor."  
* **Restrictions (Logic Checks):**  
  * If (Player.Age \> 50\) \-\> "Barrier repels you. Only the young may enter."  
  * If (Player.Cultivation \> Golden Core) \-\> "Your power is too high; the dimension would collapse."  
  * If (Player.Root \!= Fire) \-\> "The door burns your hand. Only Fire roots accepted."  
* **Reward:** Unique Manuals or GOD tier items like GOD\_010 (Wisdom Essence).

### **5.2 Dungeons (Ruins)**

* **Spawning:** Placed in high-danger biomes (Desert, Swamp).  
* **Structure:** Procedural maze.  
  * *Traps:* SD\_48 (Termite) nests or Poison Gas.  
  * *Boss:* An Elite mob (e.g., SD\_50 Twin-Headed Ogre) guarding a chest.  
* **Difficulty Rating:** Displayed as "Recommended Realm: Foundation Phase".

### **5.3 Secret Opportunities (Lucky Chances)**

* **Hidden Caves:** Revealed only if player uses SKL\_UTL\_01 (Spirit Eye) on a mountain tile. Contains a skeleton with a storage ring.  
* **Natural Treasures:** A HRB\_020 (Golden Apple) spawns in a remote forest. It emits a "Qi Beacon" that attracts nearby beasts (SD\_01 Wolves) and NPCs. The player must race them to it.

---

## **6\. Gateways & Connections Logic**

Gateways are the "elevators" of the multiverse.

| Gateway | Connects | Visual / Interaction | Key Requirement |
| :---- | :---- | :---- | :---- |
| **Ascension Pillar** | Mortal $\\to$ Sky | Massive stone beanstalk. Climbable. | Athletics check or SD\_34 (Cloud Horse) Mount. |
| **Abyssal Vent** | Mortal $\\to$ Abyss | Cracks in the earth spewing green smoke. | CSM\_04 (Detox Pill) prevents instant death. |
| **Deep Whirlpool** | Mortal $\\to$ Deep Sea | Huge vortex. | Water Affinity or HRB\_037 (Air Bubble Algae). |
| **Spatial Tear** | Any $\\to$ Void | Glitching purple pixels. Unstable. | CORE\_027 (Void Core) or SKL\_MOV\_03 (Blood Escape). |
| **Dragon Gate** | River $\\to$ Spirit Realm | Waterfall flowing UP. | Only opens if player has "Dragon Blood" traits or items. |

---

## **7\. Implementation Data Structure (C++ Pseduo)**

C++  
struct BiomeData {  
    string name;  
    vector\<string\> herb\_table; // IDs like "HRB\_001"  
    vector\<string\> mob\_table;  // IDs like "SD\_01"  
    vector\<string\> race\_table; // IDs like "RACE\_05"  
    float danger\_rating;       // 1.0 to 10.0  
};

struct SectData {  
    string name;  
    int grade; // 1 (Saint) to 9 (Iron)  
    int territory\_radius;  
    string guardian\_beast\_id; // e.g. "SB\_16"  
    vector\<string\> restricted\_zones; // List of Inheritance Ground IDs  
};

// World Gen Logic  
void PopulateTile(Tile& tile, BiomeData biome) {  
    // 1\. Spawn Resources  
    if (Random() \< 0.1) tile.AddEntity(GetRandom(biome.herb\_table));  
      
    // 2\. Spawn Mobs  
    if (Random() \< 0.05) tile.AddEntity(GetRandom(biome.mob\_table));  
      
    // 3\. Spawn POI (Dungeon/Ruins)  
    if (Random() \< 0.001) tile.SetStructure("ANCIENT\_RUIN");  
}  
