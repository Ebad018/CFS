# **Design Document: Playable & NPC Races**

Project: Cultivation Fortress

Total Entries: 12 Unique Races

Parsing Rules:

* **Playable:** boolean (TRUE \= Selectable at start, FALSE \= Encounter/Recruit only).  
* **Affinity:** Which Element/Law they excel in.  
* **Base\_Stats:** Modifiers to the standard Human baseline (e.g., Str \* 1.5).

---

## **Part 1: The Mortal & God-Touched (Humanoids)**

| ID | Race Name | Playable | Origin Lore | Traits & Mechanics |
| :---- | :---- | :---- | :---- | :---- |
| **RACE\_01** | **Human (Mortal)** | TRUE | **Origin:** Born from the soil of the Mortal Plane. **Lore:** Weakest physique but possessing the highest "Dao Comprehension." They dominate the central continents due to their adaptability and social structures (Sects/Dynasties). | **Jack of All Trades:** No stat penalties or bonuses. **High Insight:** \+20% Learning Speed for all Scriptures. **Social:** Can enter any neutral city without reputation penalties. |
| **RACE\_02** | **God-Blood Scion (Phoenix)** | TRUE | **Origin:** Humans whose ancestors consumed **Phoenix Fire Essence**. **Lore:** A clan of elites who rule the volcanic regions. They view themselves as royalty and often hold tournaments to prove their blood purity. | **Rebirth:** On death, 10% chance to revive with 1 HP (Cooldown: 10 years). **Yang Body:** Immune to Heat/Fire. Weak to Cold. **Locked Talent:** Auto-learns MTH\_FIRE\_01 (Raging Flame) at start. |
| **RACE\_03** | **God-Blood Scion (Tiger)** | TRUE | **Origin:** Descendants of generals who bathed in the blood of the **White Tiger of Slaughter**. **Lore:** Born with innate killing intent. They usually run the mercenary guilds or assassination halls. | **Slaughter Aura:** Enemies with lower Cultivation tiers are briefly stunned at combat start. **Metal Affinity:** \+30% Damage with Sword/Spear weapons. **Bloodthirst:** Must kill 1 entity per month or suffer stat penalties. |
| **RACE\_04** | **Giant Demon (Orc)** | TRUE | **Origin:** Unknown. Rumored to have climbed out of the **Demon Abyss** layers. **Lore:** Hulking, green/grey-skinned humanoids. They despise "Magic/Qi" tricks and respect only physical strength. They keep Humans as slaves for farming/crafting. | **Physique Supreme:** Body\_Def \* 2.0, Str \* 2.0. **Qi Blocked:** Cannot learn Elemental Spells. Can ONLY use **Body Cultivation** methods like MTH\_BDY\_01. **Slaver:** Can capture defeated human enemies as "Laborer" units. |

---

## **Part 2: The Spirit & Element Born (Demihumans)**

| ID | Race Name | Playable | Origin Lore | Traits & Mechanics |
| :---- | :---- | :---- | :---- | :---- |
| **RACE\_05** | **Spirit-Wood Fey (Elf)** | TRUE | **Origin:** Born from the **Six-Eared Bodhi Ape's** forests. **Lore:** They guard Spirit Veins. Their bodies are plant-matter hybrids, allowing them to communicate with flora. They view Mining as "wounding the earth." | **Photosynthesis:** Does not need food if in Sunlight. **Herb Sense:** Can see **HRB** locations on the minimap. **Fragile:** Max\_HP \* 0.7. Fire damage received \+50%. |
| **RACE\_06** | **Abyssal Octoman** | TRUE | **Origin:** Servants of the **Void-Swallowing Leviathan**. **Lore:** Hybrid humans with cephalopod lower bodies or tentacle appendages. They live in underwater cities in the Deep Sea Trenches. Highly isolationist. | **Amphibious:** Bonuses in Water tiles; normal movement on Land. **Multi-Limb:** Can equip 4 Weapons (High DPS). **Dry Skin:** Takes constant damage in Desert/Volcano biomes. Weak to Fire/Lightning. |
| **RACE\_07** | **Stone Spirit (Golem)** | FALSE | **Origin:** Sentient fragments of the **World-Supporting Black Turtle**. **Lore:** They eat **ORE** items to grow. They do not die of old age but can be "mined" by greedy cultivators. | **Lithovore:** Eats Ores/Stones instead of Food. **Solid Body:** Base Defense \+50. Immune to Bleed/Poison. **Slow:** Movement speed \-50%. Cannot Fly until high tier. |
| **RACE\_08** | **Sky-Winged Celestial** | TRUE | **Origin:** Humans blessed by the **Solar Golden Crow**. **Lore:** They live on floating islands in the Sky Layer. They look down on "Ground Crawlers." Their bones are hollow, making them light but brittle. | **Flight:** Can fly over obstacles at Tier 1 (Mortal). **Wind Master:** \+20% Speed. **Glass Cannon:** Phys\_Def \* 0.5. Blunt damage is lethal to them. |

---

## **Part 3: The Exotic & Forbidden (Monstrous)**

| ID | Race Name | Playable | Origin Lore | Traits & Mechanics |
| :---- | :---- | :---- | :---- | :---- |
| **RACE\_09** | **Fox-Kin (Yao)** | TRUE | **Origin:** Relatives of the **Nine-Tailed Spirit Fox**. **Lore:** Beastkin who can shift between human and fox forms. Known as tricksters and illusionists. Often hunted for their fur/cores. | **Shapeshift:** Toggle between Human (Tool use) and Beast (Speed/Claw) modes. **Charm:** Starts with MTH\_MND\_01 (Fox-Spirit Charm). **Hunted:** "Righteous" Sects attack on sight. |
| **RACE\_10** | **Void Shade** | TRUE | **Origin:** Creatures leaked from the realm of the **Primordial Chaos Dragon**. **Lore:** They have no face, only a mask. They feed on the "Karma" of others. They are natural assassins who blend into the Void Layer. | **Phase:** 20% Chance to ignore physical attacks. **Space Affinity:** Can use Teleportation scrolls/skills without cost. **Soulless:** Cannot use "Holy" or "Light" items. |
| **RACE\_11** | **Hive-Born Insectoid** | FALSE | **Origin:** Evolved from the **Silver-Winged Ant** colonies. **Lore:** A hive mind race. Individuals are weak, but they can telepathically coordinate perfect attacks. They consume metal to build their carapace. | **Hive Mind:** If one is attacked, all Hive-Born in the chunk Aggro instantly. **Carapace:** High Defense against Slashing. **Short Life:** Lifespan is only 20 years, but Reproduction rate is 10x Humans. |
| **RACE\_12** | **Asura (Battle-Born)** | TRUE | **Origin:** Spontaneously generated from the blood on **Ancient Battlefields**. **Lore:** Males are hideous monsters; Females are bewildering beauties. Both exist only to fight. They cultivate by killing, not meditating. | **Battle Hunger:** Gains XP from Kills, NOT from Meditation. **Rage:** Damage increases as HP decreases. **Ugly/Beautiful:** Males have Charisma 0; Females have Charisma 100\. |

