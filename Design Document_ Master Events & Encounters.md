# **Design Document: Master Events & Encounters**

**Project:** Cultivation Fortress **Usage:** Parsed by the `EventManager` and `WorldGen` systems to populate the world with content.

---

## **Part 1: Points of Interest (Static & Procedural Dungeons)**

*Locations spawned during World Gen or by Secret Maps.*

1. **The Sword Tomb:** A graveyard of rusted swords. High `Metal Qi`. Contains `WPN_SWD_01` loot. Guarded by a `SD_38` (Phantom Wisp) formed from sword intent.  
2. **Dragon-Fall Ridge:** A mountain shaped like a spine. Mining here yields `ORE_008` (Star Steel).  
3. **Cryo-Wyrm's Lair:** A cave in the Tundra sealed by ice. Boss: `SB_02` (Cryo-Jade Wyrm). Reward: `MAT_047` (Wyrm Ice Crystal).  
4. **The Burning Orchard:** A forest in a Volcano biome where `HRB_009` (Fire Petals) grow abundantly. Hazard: Constant heat damage.  
5. **Shattered Void Pocket:** A mini-dimension accessible via `MAT_039` (Void Scale). Contains `ORE_022` (Void Crystal) but deals Spatial Shear damage.  
6. **The Ant Hive:** A procedural dungeon inside a desert mesa. Infinite spawns of `SD_36` (Silver-Winged Ants). The Queen drops `MAT_031` (Mandible).  
7. **Ancient Battlefield:** A zone of red soil. Spawns `RACE_12` (Asuras). High chance to find `MAT_021` (Blood Stone).  
8. **The Medicine Garden:** A hidden valley protected by a natural array. Contains `HRB_006` (Iron-Bark) and `HRB_002` (Blood Ginseng).  
9. **Abyssal Vent:** A crack in the earth leading to Layer \-1. Spews Miasma. Requires `CSM_04` (Detox Pill) to approach.  
10. **The Floating Pavilion:** A structure in the Sky Layer (+1). Home to `RACE_08` (Celestials). Trades `ORE_024` (Heaven Gold).  
11. **Gargoyle Ruins:** An abandoned city where the statues are actually `SD_33` (Stone Gargoyles).  
12. **Thunder Peaks:** A mountain range where lightning strikes every 10 ticks. Ideal for cultivating `MTH_LGT_01` (Thunder-Forging Art).  
13. **The Coral Palace:** Located in the Deep Sea. Territory of `RACE_06` (Octomen). Neutral zone for trading.  
14. **Demon Pit:** A gladiatorial arena in the Abyss Layer. Win 10 rounds to earn the "Champion" title.  
15. **The Scholar's Grotto:** A cave with walls covered in text. Meditating here grants \+100% Skill XP gain.  
16. **Venom Swamp:** Home to `SB_13` (Venom-Crown King Cobra). The water itself applies the Poison status.  
17. **Fox-Fire Shrine:** A mystic temple in the forest. If you donate `ORE_005` (Spirit Stones), `RACE_09` (Fox-Kin) grant you a "Lucky" buff.  
18. **The Iron Forest:** Trees made of metal. Drops `ORE_001` (Iron Ore) instead of wood when cut.  
19. **Sun-Crow Perch:** A massive dead tree in the desert. `GB_07` (Solar Golden Crow) lands here once every 10 years.  
20. **The Sealed Gate:** A massive door requiring a specific Key Item (e.g., `GOD_001` Chaos Heart) to open. Behind it is a `Grade 1` Spirit Realm.  
21. **Ghost City:** Appears only at night. Inhabited by souls. Currency: `MAT_014` (Ghost Skull).  
22. **The Mirror Lake:** Reflects attacks. Combat here is dangerous as damage is shared between attacker and target.  
23. **Termite Mound:** A massive structure. If destroyed, drops tons of `MAT_041` (Termite Acid).  
24. **The Whisper Canyon:** Sound-based hazard. Requires `MAT_032` (Crystal Wing) to dampen the noise or take Soul Damage.  
25. **Primordial Hot Spring:** Resting here restores 100% HP/Qi and cures "Hidden Injuries" to Foundation.

---

## **Part 2: Dynamic World Events (Global Simulation)**

*Large-scale occurrences that change the state of the world.*

26. **The Beast Tide:** A swarm of 500 `SD_01` (Iron-Back Wolves) moves toward the nearest Human City.  
27. **Sect War:** Sect A declares war on Sect B. Battles occur at border tiles. Supply caravans appear.  
28. **Heavenly Tribulation:** An NPC is attempting a breakthrough to Saint Tier. Lightning strikes a specific tile for 3 days. Looting the corpse is possible if they fail.  
29. **Auction Festival:** The central city holds a massive auction. `GOD_002` (Phoenix Fire Essence) is listed.  
30. **Spirit Vein Eruption:** A random tile suddenly gains "High Qi Density." Sects immediately rush to claim it.  
31. **The Blood Moon:** For 30 days, all Demonic Cultivators and Beasts gain \+50% ATK. Righteous NPCs stay indoors.  
32. **Ancient Inheritance Opening:** A notification appears: "The Tomb of the Sword Saint will open in 3 months."  
33. **Plague Outbreak:** A city is infected. NPCs slowly die. Selling `CSM_04` (Detox Pills) yields 5x profit.  
34. **Drought:** Caused by `GB_07` (Golden Crow). Rivers dry up. Water-element cultivators lose combat stats.  
35. **Meteor Shower:** `ORE_008` (Star Steel) nodes spawn randomly across the map.  
36. **Demon Invasion:** An Abyssal Vent widens. `RACE_04` (Demons) establish a forward operating base on the Surface.  
37. **The Grand Tournament:** A global ranking battle. Player can sign up to win "Reputation" and `CORE_007` (Lion Golden Core).  
38. **Merchant Fleet Arrival:** A massive fleet from another continent docks. Sells exotic `HRB` not found locally.  
39. **Bounty Hunt:** The "Discipline Hall" posts a high reward for a rogue cultivator (NPC).  
40. **Falling Star:** A massive projectile hits a chunk, destroying it and creating a "Crater" biome filled with fire.  
41. **Qi Famine:** Global Qi density drops by 50% for a year. Cultivation speed slows. Pills become expensive.  
42. **Beast King Ascension:** A `SD_02` (Wind Leopard) evolves into a `SB_xx` (Saint Beast). A new Boss icon appears on the map.  
43. **The Fog of Lost Souls:** Covers a region in fog. Minimap is disabled in that zone.  
44. **Diplomatic Marriage:** Two rival sects merge. Their territory combines, creating a superpower.  
45. **God Beast Awakening:** `GB_03` (Black Turtle) wakes up, causing earthquakes. All caves have a chance to collapse.

---

## **Part 3: Lucky Chances (Opportunities & Loot)**

*Positive events triggered by Luck stat or exploration.*

46. **Dying Elder:** You find an injured Elder. Heal him (`CSM_02`) to gain a powerful ally or kill him to steal his Storage Ring.  
47. **Spirit Fruit Ripening:** You stumble upon a `HRB_015` (Seven-Color Fruit) just as it matures. Must harvest before beasts smell it.  
48. **Sudden Enlightenment:** While meditating near a waterfall, you gain 5000 XP instantly.  
49. **Remnant Soul:** You find a ring with a "Grandpa" soul inside. He offers to teach you a `Tier 7` Skill.  
50. **Duel of Titans:** You witness two `Saint Beasts` fighting. Wait for both to be low HP, then kill both for double loot.  
51. **Hidden Cave:** `SKL_UTL_01` (Spirit Eye) reveals an illusory wall. Behind it is a chest with 500 Spirit Stones.  
52. **Abandoned Caravan:** You find a wrecked wagon containing crates of `ORE_001` (Iron Ore).  
53. **Rare Pet Egg:** You find an egg that hatches into a `SD_13` (Lightning Sable).  
54. **Qi Geyser:** You step on a pressure plate, releasing a burst of pure Qi. Full MP restore \+ temporary buff.  
55. **Auction Mistake:** A vendor sells a "Dirty Rock" for 10 coins. It's actually `ORE_020` (Soul Stone).  
56. **Weather Insight:** Being struck by lightning (and surviving) grants \+10% Thunder Affinity.  
57. **The Drunken Master:** An old beggar asks for wine. Give him `HRB_029` (Buddha Hand Citron) to learn a drunken sword art.  
58. **Spatial Rift Loot:** A `MAT_039` (Void Scale) falls out of a rift near you.  
59. **Medicinal Bath:** You find a natural hot spring infused with sulfur. Permanently increases `Body Def` by \+1.  
60. **Lost Disciple:** You find a lost child of a Sect Leader. Returning them grants massive Reputation.  
61. **Meteorite Fragment:** You find a piece of `ORE_024` (Heaven Gold) smoking in a crater.  
62. **Ancient Stele:** Reading the text requires `Intelligence > 50`. Grants the "Scholar" title.  
63. **Beast Infighting:** A pack of `SD_01` Wolves is fighting a `SD_10` Bear. You can sneak past them to the resource node they were guarding.  
64. **The Golden Carp:** Fishing in a lake yields a `GB_09` (Time-River Spirit Carp) scale.  
65. **Alchemist's Failure:** You find a trash heap of "Ruined Pills." Eating them is risky but might give \+Stats.

---

## **Part 4: Random Encounters (NPCs & Mobs)**

*Interactions that happen while moving between tiles.*

66. **The Arrogant Young Master:** An NPC demands you give up your table at a restaurant. Refusing starts a duel.  
67. **Bandit Ambush:** "This road was opened by me\!" 5 Rogues demand toll.  
68. **Sect Inspection:** A patrol stops you. If you are a "Demonic Cultivator," they attack.  
69. **Injured Beast:** A `SD_43` (Jade-Horned Deer) is caught in a trap. Free it for Good Karma, kill it for `MAT_036` (Jade Antler).  
70. **The Mysterious Merchant:** Sells items at 2x price but has stock not available in the region.  
71. **Rival Challenge:** An NPC with the same Title as you (e.g., "Sword Saint") challenges you to prove who is better.  
72. **Begging Orphan:** If you adopt them, they become a Disciple. If you ignore them, they might grow up to be a villain.  
73. **Corpse Discovery:** You find a dead cultivator. Looting him gives `MAT_004` (Flame Sac) but leaves "Scent of Death" on you (relatives will track you).  
74. **Wild Beast Pack:** 5 `SD_01` (Iron-Back Wolves) encircle you.  
75. **The Seducer:** A `RACE_09` (Fox-Kin) tries to charm you. Resistance check vs `Mental State`.  
76. **Mining Dispute:** Two NPCs are arguing over an `ORE_002` (Copper) node. You can mediate, kill both, or walk away.  
77. **Tribulation Bystander:** You are too close to someone ascending. You take Lightning Damage.  
78. **The Scammer:** An NPC tries to sell you a "Map to the Void Layer" (It's fake).  
79. **Escaped Slave:** A `RACE_01` Human fleeing a `RACE_04` Demon. Helping spawns a Demon Hunter enemy.  
80. **Soul Search Victim:** You find a vegetable (brain-dead) NPC. Using `SKL_UTL_02` reveals who did it.  
81. **The Glutton:** A `SD_18` (Iron-Eating Panda) starts eating your weapon if you are AFK/Meditation.  
82. **Assassination Attempt:** A `RACE_10` (Void Shade) attacks from stealth.  
83. **Territorial Dispute:** You enter the range of a `SB_07` (Golden Mammoth). It charges immediately.  
84. **Friendly Spar:** A wandering monk asks to exchange moves. Non-lethal combat. XP reward.  
85. **The Gambler:** Bets you 100 Spirit Stones on a coin toss.  
86. **Pill Tester:** An Alchemist asks you to test a prototype. 50% chance of Stat Boost, 50% chance of Poison.  
87. **The Collector:** An NPC offers 5x market price for `MAT_034` (Butterfly Wing).  
88. **Spirit Beast Tamer:** Challenges you to a Pokémon-style battle using his pet `SD_13` (Lightning Sable).  
89. **Dao Debate:** A scholar challenges your philosophy. "Dao Grasp" stat check.  
90. **The Tax Collector:** A Sect demands resources for being in their territory.

---

## **Part 5: Environmental Hazards & Anomalies**

*Passive dangers in specific tiles.*

91. **Gravity Well:** Found in Void Layer. Movement costs 3x Stamina.  
92. **Poison Mist:** Found in Swamp. Applies "Toxic" stack every turn.  
93. **Razor Grass:** Found in some Plains. Deals 10 Bleed dmg if moving without boots.  
94. **Magnetic Storm:** Found in Thunder Peaks. Metal weapons cannot be equipped.  
95. **Heat Wave:** In Deserts, water consumption doubles.  
96. **Frozen Soil:** Cannot use "Earth" element skills or dig.  
97. **Null-Qi Zone:** No Qi regeneration allowed here. Consumables only.  
98. **Illusion Forest:** Movement controls are randomized (North becomes West).  
99. **Blood Rain:** Lowers "Mental State" of all characters in the chunk.  
100. **Time Distortion:** 1 Day inside this tile \= 1 Year outside (Reverse Seclusion). Useful for rapid aging, bad for deadlines.

