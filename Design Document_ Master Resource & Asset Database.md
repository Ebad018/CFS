# **Design Document: Master Resource & Asset Database**

Project: Cultivation Fortress

Total Entries: 160 Unique Assets

Parsing Rules:

* **Tier:** 1 (Mortal) to 10 (God).  
* **Tags:** Define crafting interactions (e.g., Alchemy\_Main means it's a primary ingredient for pills).  
* **Source:** References the exact ID from your Bestiary or Biome type.

---

## **Part 1: Beast Cores (Cultivation Batteries)**

*Used to power formations, refine artifacts, or absorb directly for XP.*

| ID | Name | Tier | Source | Effect Tag | Description |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **CORE\_001** | **Iron-Back Wolf Core** | 1 | SD\_01 | Metal\_Qi | Basic metal essence. Used for low-tier weapon forging. |
| **CORE\_002** | **Wind Leopard Core** | 2 | SD\_02 | Speed\_Qi | Used in speed-enhancing formations. |
| **CORE\_003** | **Boar Earth Core** | 1 | SD\_03 | Defense\_Qi | Stabilizes defensive arrays. |
| **CORE\_004** | **Flame Lizard Core** | 1 | SD\_04 | Fire\_Qi | Heat source for basic alchemy furnaces. |
| **CORE\_005** | **Venom Spider Core** | 1 | SD\_05 | Poison\_Qi | Used to craft toxic artifacts. |
| **CORE\_006** | **Python Wood Core** | 2 | SD\_07 | Wood\_Qi | Accelerates plant growth in Spirit Gardens. |
| **CORE\_007** | **Lion Golden Core** | 3 | SD\_08 | Sonic\_Qi | Enhances sound-based martial arts. |
| **CORE\_008** | **Shadow Cat Core** | 2 | SD\_09 | Dark\_Qi | Used for concealment arrays. |
| **CORE\_009** | **Bear Vitality Core** | 3 | SD\_10 | Life\_Qi | Massive vitality boost if consumed raw (Risk: Explode). |
| **CORE\_010** | **Scorpion Earth Core** | 2 | SD\_11 | Earth\_Toxin | Mixed element core; hard to refine. |
| **CORE\_011** | **Eagle Fire Core** | 2 | SD\_12 | Yang\_Qi | Pure Yang energy; counters Yin poisons. |
| **CORE\_012** | **Sable Lightning Core** | 4 | SD\_13 | Thunder\_Qi | Rare. Used to power Tribulation-crossing artifacts. |
| **CORE\_013** | **Monkey Soul Core** | 3 | SD\_14 | Soul\_Qi | Used to repair damaged souls. |
| **CORE\_014** | **Toad Illusion Core** | 2 | SD\_15 | Mind\_Qi | Creates illusion barriers around Sects. |
| **CORE\_015** | **Eel Thunder Core** | 3 | SD\_18 | Shock\_Qi | Unstable lightning energy. |
| **CORE\_016** | **Squid Ink Core** | 2 | SD\_22 | Blind\_Qi | Dark element; absorbs light. |
| **CORE\_017** | **Angler Lure Core** | 4 | SD\_26 | Light\_Qi | Glows eternally. |
| **CORE\_018** | **Shark Frost Core** | 3 | SD\_27 | Ice\_Qi | Freezes liquids on contact. |
| **CORE\_019** | **Beetle Magma Core** | 3 | SD\_31 | Lava\_Qi | Burning hot. Needs magnetic storage. |
| **CORE\_020** | **Bat Sonic Core** | 2 | SD\_32 | Echo\_Qi | Used in communication talismans. |
| **CORE\_021** | **Wisp Spirit Core** | 4 | SD\_38 | Ghost\_Qi | Allows interaction with the spirit realm. |
| **CORE\_022** | **Obsidian Heart Core** | 5 | SD\_39 | Magma\_Qi | A beating heart of stone and fire. High energy density. |
| **CORE\_023** | **Goat Thunder Core** | 3 | SD\_41 | Cloud\_Qi | Harvested from peaks. |
| **CORE\_024** | **Mosquito Blood Core** | 2 | SD\_42 | Blood\_Qi | Contains stolen cultivation base from other beasts. |
| **CORE\_025** | **Deer Jade Core** | 4 | SD\_43 | Heal\_Qi | Naturally heals those near it. |
| **CORE\_026** | **Bear Glacial Core** | 4 | SD\_45 | Frost\_Qi | Used to forge Ice-attribute flying swords. |
| **CORE\_027** | **Chameleon Void Core** | 5 | SD\_46 | Space\_Qi | Critical for crafting Storage Rings. |
| **CORE\_028** | **Sprite Solar Core** | 5 | SD\_47 | Sun\_Qi | Condensed sunlight. Extremely hot. |
| **CORE\_029** | **Moth Dream Core** | 4 | SD\_49 | Sleep\_Qi | Induces deep cultivation trance. |
| **CORE\_030** | **Ogre Chaos Core** | 5 | SD\_50 | Chaos\_Qi | Dirty, mixed energy. Requires heavy refining. |

---

## **Part 2: Beast Materials (Crafting & Alchemy)**

*Physical parts dropped by beasts for Smithing/Alchemy.*

| ID | Name | Tier | Source | Use Tag | Description |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **MAT\_001** | **Iron Claw** | 1 | SD\_01 | Weapon\_Edge | Sharp metal claw. Standard dagger material. |
| **MAT\_002** | **Agility Tendon** | 2 | SD\_02 | Bow\_String | Elastic tendon for spirit bows. |
| **MAT\_003** | **Rock Boar Skin** | 1 | SD\_03 | Armor\_Leather | Tough hide for basic armor. |
| **MAT\_004** | **Flame Sac** | 1 | SD\_04 | Alch\_Fire | Fluid burns when exposed to air. |
| **MAT\_005** | **Venom Gland** | 1 | SD\_05 | Alch\_Poison | Potent neurotoxin. |
| **MAT\_006** | **Rat Whiskers** | 2 | SD\_06 | Divination | Used in dowsing rods to find treasure. |
| **MAT\_007** | **Python Scale** | 2 | SD\_07 | Armor\_Flex | Flexible armor material. |
| **MAT\_008** | **Lion Mane Fur** | 3 | SD\_08 | Talisman\_Ink | Used to make brushes for talisman writing. |
| **MAT\_009** | **Shadow Fur** | 2 | SD\_09 | Cloak\_Mat | Absorbs light. Used for stealth cloaks. |
| **MAT\_010** | **Bear Gallbladder** | 3 | SD\_10 | Alch\_Vitality | Major ingredient for healing pills. |
| **MAT\_011** | **Scorpion Stinger** | 2 | SD\_11 | Weapon\_Tip | Natural poison injector for spear tips. |
| **MAT\_012** | **Crimson Beak** | 2 | SD\_12 | Arrow\_Head | Armor-piercing material. |
| **MAT\_013** | **Lightning Pelt** | 4 | SD\_13 | Boots\_Mat | Static charged fur. Increases movement speed. |
| **MAT\_014** | **Ghost Skull** | 3 | SD\_14 | Artifact\_Base | Skull retains soul energy. Used for necromancy. |
| **MAT\_015** | **Toad Eye** | 2 | SD\_15 | Alch\_Mind | Ingredient for "Clear Mind" pills. |
| **MAT\_016** | **Piranha Tooth** | 1 | SD\_16 | Ammo\_Mat | Used as shrapnel in explosives. |
| **MAT\_017** | **Crab Chitin** | 2 | SD\_17 | Shield\_Mat | Lightweight but hard as steel. |
| **MAT\_018** | **Eel Electric Organ** | 3 | SD\_18 | Trap\_Comp | Biological battery. |
| **MAT\_019** | **Makara Scale** | 3 | SD\_20 | Ship\_Hull | Waterproof and tough. Best for boat building. |
| **MAT\_020** | **Swordfish Bill** | 3 | SD\_21 | Weapon\_Base | Natural sword. Just needs a hilt. |
| **MAT\_021** | **Siren Pearl** | 3 | SD\_24 | Mental\_Def | Worn as a necklace to resist sonic attacks. |
| **MAT\_022** | **Flying Fin** | 2 | SD\_25 | Glider\_Wing | Membrane used for low-level flight tools. |
| **MAT\_023** | **Shark Fin (Ice)** | 3 | SD\_27 | Alch\_Cooling | Cools down "Fire Deviation" symptoms. |
| **MAT\_024** | **Leech Blood Sac** | 2 | SD\_28 | Alch\_Blood | Concentrated blood essence. |
| **MAT\_025** | **Turtle Shell** | 4 | SD\_29 | Shield\_Heavy | Massive shield material. Heavy but unbreakable. |
| **MAT\_026** | **Frog Leg Muscle** | 1 | SD\_30 | Alch\_Speed | Consumed for temporary jump boost. |
| **MAT\_027** | **Bat Wing Membrane** | 2 | SD\_32 | Scroll\_Mat | Used to write sonic-attribute scriptures. |
| **MAT\_028** | **Gargoyle Stone** | 3 | SD\_33 | Golem\_Core | Living stone. Used to craft guardian golems. |
| **MAT\_029** | **Cloud Mane** | 4 | SD\_34 | Fabric\_Light | Weighs nothing. Used for Daoist robes. |
| **MAT\_030** | **Parasite Vine** | 2 | SD\_35 | Whip\_Mat | Living whip. Constricts on impact. |
| **MAT\_031** | **Silver Ant Mandible** | 2 | SD\_36 | Refining\_Flux | Dissolves impurities in ore. |
| **MAT\_032** | **Vulture Feather** | 2 | SD\_37 | Alch\_Plague | Carrier of disease. Used in poison arrays. |
| **MAT\_033** | **Obsidian Plate** | 5 | SD\_39 | Armor\_Plate | Highest heat resistance. For lava diving suits. |
| **MAT\_034** | **Butterfly Wing** | 2 | SD\_40 | Decor\_Rare | Extremely beautiful. Trade good for charisma. |
| **MAT\_035** | **Thunder Horn** | 3 | SD\_41 | Wand\_Tip | Focuses lightning spells. |
| **MAT\_036** | **Jade Antler** | 5 | SD\_43 | Alch\_Panacea | Cures almost any mortal disease. |
| **MAT\_037** | **Worm Segment** | 2 | SD\_44 | Bait\_High | Irresistible bait for Spirit Beasts. |
| **MAT\_038** | **Glacial Fur** | 4 | SD\_45 | Cloak\_Cold | Immune to freezing environments. |
| **MAT\_039** | **Void Scale** | 5 | SD\_46 | Talisman\_Void | Single-use teleportation material. |
| **MAT\_040** | **Sun Petal** | 5 | SD\_47 | Alch\_Yang | Ingredient for "Sun God" body tempering. |
| **MAT\_041** | **Termite Acid** | 2 | SD\_48 | Trap\_Acid | Melts metal locks/doors. |
| **MAT\_042** | **Silk Cocoon** | 3 | SD\_49 | Fabric\_Dream | Robes made of this prevent nightmares/demons. |
| **MAT\_043** | **Ogre Bone** | 4 | SD\_50 | Club\_Mat | Dense bone. Good for blunt weapons. |
| **MAT\_044** | **Dragon Bone (Fragment)** | 7 | SB\_14 | Artifact\_Legend | Dropped by Spectral Bone Dragon. |
| **MAT\_045** | **Netherfire Scale** | 7 | SB\_08 | Armor\_God | Dropped by Ghost Shark. Immunity to magma. |
| **MAT\_046** | **Thunder Roc Feather** | 8 | SB\_01 | Wing\_Artifact | Dropped by Roc. Used to craft "Cloud Stepping Wings". |
| **MAT\_047** | **Wyrm Ice Crystal** | 7 | SB\_02 | City\_Core | Dropped by Cryo Wyrm. Can cool an entire city. |
| **MAT\_048** | **Demon Ape Blood** | 7 | SB\_03 | Body\_Refine | Dropped by Blood Ape. Violent body tempering fluid. |
| **MAT\_049** | **Earth Dragon Poison** | 7 | SB\_04 | Poison\_God | Dropped by Thousand-Legged Dragon. Melts Golden Cores. |
| **MAT\_050** | **Fox Spirit Pearl** | 7 | SB\_06 | Soul\_Artifact | Dropped by Nine-Tailed Fox. Boosts charisma to supernatural levels. |

---

## **Part 3: Natural Resources (Herbs, Plants, Trees)**

*Harvested from the environment based on Biome.*

| ID | Name | Tier | Biome | Alchemy Tag | Effect |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **HRB\_001** | **Spirit Grass** | 1 | Plains | Qi\_Base | Basic XP gain. |
| **HRB\_002** | **Blood Ginseng** | 1 | Forest | Healing | Restores HP. |
| **HRB\_003** | **Silver Leaf** | 1 | Forest | Energy | Restores Stamina. |
| **HRB\_004** | **Moon Orchid** | 2 | Night/Cave | Yin | Cools body. |
| **HRB\_005** | **Sun Flower** | 2 | Plains | Yang | Warms body. |
| **HRB\_006** | **Iron-Bark Root** | 2 | Forest | Body | Hardens skin. |
| **HRB\_007** | **Mist Mushroom** | 2 | Swamp | Illusion | Causes hallucinations. |
| **HRB\_008** | **Water Lotus** | 2 | Lake | Water | Water breathing pill base. |
| **HRB\_009** | **Fire Petal** | 3 | Volcano | Fire | Fire resistance pill base. |
| **HRB\_010** | **Thunder Fruit** | 3 | Peak | Thunder | Numbing effect. |
| **HRB\_011** | **Ice Crystal Flower** | 3 | Tundra | Ice | Preserves corpse freshness. |
| **HRB\_012** | **Wind Chime Vine** | 3 | Peak | Wind | Speed pill base. |
| **HRB\_013** | **Earth Potato** | 1 | Plains | Food | High satiety food. |
| **HRB\_014** | **Golden Wheat** | 1 | Plains | Food | Spirit grain. |
| **HRB\_015** | **Seven-Color Fruit** | 5 | Spirit Vein | All\_Elem | Boosts all stats. |
| **HRB\_016** | **Ghost Pepper** | 2 | Abyss | Soul | Burns the soul. |
| **HRB\_017** | **Dragon Blood Tree Sap** | 6 | Rare | Mutation | Random positive mutation. |
| **HRB\_018** | **Star Dust Pollen** | 4 | Sky | Space | Void pill base. |
| **HRB\_019** | **Corpse Flower** | 3 | Graveyard | Necro | Zombie control pill. |
| **HRB\_020** | **Golden Apple** | 5 | Rare | Longevity | Extends life by 10 years. |
| **HRB\_021** | **Vermilion Fruit** | 4 | Volcano | Fire\_Exp | Massive Fire XP. |
| **HRB\_022** | **Azure Root** | 4 | Lake | Water\_Exp | Massive Water XP. |
| **HRB\_023** | **Topaz Melon** | 4 | Mountain | Earth\_Exp | Massive Earth XP. |
| **HRB\_024** | **Emerald Leaf** | 4 | Forest | Wood\_Exp | Massive Wood XP. |
| **HRB\_025** | **Platinum Berry** | 4 | Mine | Metal\_Exp | Massive Metal XP. |
| **HRB\_026** | **Soul Severing Grass** | 7 | Abyss | Poison | Kills soul, leaves body. |
| **HRB\_027** | **Nirvana Seed** | 8 | Phoenix Nest | Rebirth | Cheat death once. |
| **HRB\_028** | **Chaos Vine** | 9 | Void | Chaos | Unknown effects. |
| **HRB\_029** | **Buddha Hand Citron** | 6 | Temple | Calm | Removes Heart Demons. |
| **HRB\_030** | **Demon Face Root** | 6 | Abyss | Berserk | Trigger berserk mode. |
| **HRB\_031** | **Bamboo Shoot (Spirit)** | 2 | Bamboo Forest | Food | Slight Qi recovery. |
| **HRB\_032** | **Sword Grass** | 3 | Battlefields | Attack | Cuts fingers if touched. |
| **HRB\_033** | **Shadow Moss** | 2 | Cave | Stealth | Masks scent. |
| **HRB\_034** | **Light Bulb Plant** | 1 | Cave | Light | Natural torch. |
| **HRB\_035** | **Exploding Spore** | 2 | Swamp | Trap | Explodes on contact. |
| **HRB\_036** | **Sticky Trap Plant** | 2 | Jungle | Glue | Immobilizes beasts. |
| **HRB\_037** | **Air Bubble Algae** | 2 | Ocean | Breath | Oxygen source underwater. |
| **HRB\_038** | **Salt Crystal Plant** | 1 | Ocean | Food | Seasoning. |
| **HRB\_039** | **Sugar Cane (Spirit)** | 1 | Jungle | Food | Energy boost. |
| **HRB\_040** | **Tea Leaf (Spirit)** | 2 | Mountain | Mental | Increases learning speed. |

---

## **Part 4: Ores & Minerals (Mining)**

*Harvested from Mines, Caves, and the Underground.*

| ID | Name | Tier | Biome | Refining Tag | Properties |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **ORE\_001** | **Iron Ore** | 1 | Mine | Base | Common metal. |
| **ORE\_002** | **Copper Ore** | 1 | Mine | Conductive | Energy transfer. |
| **ORE\_003** | **Silver Ore** | 2 | Mine | Holy | Harmful to undead. |
| **ORE\_004** | **Gold Ore** | 2 | Mine | Currency | Trade value. |
| **ORE\_005** | **Spirit Stone (Raw)** | 1 | Spirit Vein | Energy | Unrefined currency. |
| **ORE\_006** | **Cold Iron** | 2 | Tundra | Cold | Retains cold. |
| **ORE\_007** | **Fire Copper** | 2 | Volcano | Heat | Retains heat. |
| **ORE\_008** | **Star Steel** | 3 | Impact Site | Heavy | Very dense. |
| **ORE\_009** | **Mithril** | 4 | Deep Mine | Magic | High Mana flow. |
| **ORE\_010** | **Adamantite** | 5 | Deep Mine | Hard | Unbreakable. |
| **ORE\_011** | **Obsidian** | 2 | Volcano | Sharp | Natural glass edge. |
| **ORE\_012** | **Crystal (Clear)** | 1 | Cave | Storage | Stores data/light. |
| **ORE\_013** | **Ruby (Fire)** | 3 | Volcano | Gem\_Fire | Fire focus. |
| **ORE\_014** | **Sapphire (Water)** | 3 | Ocean | Gem\_Water | Water focus. |
| **ORE\_015** | **Emerald (Wood)** | 3 | Jungle | Gem\_Wood | Wood focus. |
| **ORE\_016** | **Topaz (Earth)** | 3 | Mountain | Gem\_Earth | Earth focus. |
| **ORE\_017** | **Amethyst (Thunder)** | 3 | Peak | Gem\_Thunder | Thunder focus. |
| **ORE\_018** | **Onyx (Dark)** | 3 | Abyss | Gem\_Dark | Dark focus. |
| **ORE\_019** | **Diamond (Light)** | 4 | Deep Mine | Gem\_Light | Light focus. |
| **ORE\_020** | **Soul Stone** | 5 | Graveyard | Soul\_Trap | Captures souls. |
| **ORE\_021** | **Blood Stone** | 4 | Battlefield | Blood | Enhances bleed. |
| **ORE\_022** | **Void Crystal** | 6 | Void | Space | Spatial expansion. |
| **ORE\_023** | **Time Sand** | 8 | Rare | Time | Slows degradation. |
| **ORE\_024** | **Heaven Gold** | 9 | Sky | Divine | God weapon base. |
| **ORE\_025** | **Abyss Iron** | 6 | Abyss | Cursed | Corrupts user. |
| **ORE\_026** | **Jade (White)** | 2 | Mountain | Storage | Stores knowledge. |
| **ORE\_027** | **Jade (Green)** | 2 | Mountain | Heal | Passive healing. |
| **ORE\_028** | **Jade (Blood)** | 4 | Battlefield | Cursed | Drinking vessel. |
| **ORE\_029** | **Mercury** | 2 | Mine | Fluid | Fluid metal. |
| **ORE\_030** | **Sulfur** | 1 | Volcano | Explosive | Gunpowder base. |
| **ORE\_031** | **Saltpeter** | 1 | Cave | Explosive | Gunpowder base. |
| **ORE\_032** | **Coal** | 1 | Mine | Fuel | Furnace fuel. |
| **ORE\_033** | **Charcoal (Spirit)** | 2 | Forest | Fuel | High grade fuel. |
| **ORE\_034** | **Limestone** | 1 | Mine | Build | Construction. |
| **ORE\_035** | **Marble** | 2 | Mine | Build | Statue material. |

---

## **Part 5: God-Tier Unique Resources**

*Dropped ONLY by the 10 God Beasts.*

| ID | Name | Source | Use Tag | Description |
| :---- | :---- | :---- | :---- | :---- |
| **GOD\_001** | **Chaos Heart** | GB\_01 | World\_Ender | Core of the Chaos Dragon. Can rewrite map chunks. |
| **GOD\_002** | **Phoenix Fire Essence** | GB\_02 | Immortality | Grants the "Rebirth" trait to the player. |
| **GOD\_003** | **World-Shell Fragment** | GB\_03 | Invincible | Material for armor that negates 99% damage. |
| **GOD\_004** | **Devour Core** | GB\_04 | Infinite\_Qi | Consume to instantly gain 100 years of cultivation. |
| **GOD\_005** | **Killing Law Fragment** | GB\_05 | Insta\_Kill | Weapon enchantment. 5% chance to execute enemies. |
| **GOD\_006** | **Blessing Elixir** | GB\_06 | Luck\_Max | Sets player Luck stat to Maximum forever. |
| **GOD\_007** | **Sunflame Core** | GB\_07 | Sun\_God | Grants permanent flight and fire immunity. |
| **GOD\_008** | **Soul Guide Silk** | GB\_08 | Dream\_Walk | Allows player to enter NPC dreams to steal secrets. |
| **GOD\_009** | **Time-River Scale** | GB\_09 | Save\_Scum | Allows 3 "Undo" actions per in-game year. |
| **GOD\_010** | **Wisdom Essence** | GB\_10 | Skill\_Master | Instantly sets any one Martial Art to "Max Level". |

