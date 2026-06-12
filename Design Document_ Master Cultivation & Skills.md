# **Design Document: Master Cultivation & Skills**

Project: Cultivation Fortress

Total Entries: \~80 Unique Records

Parsing Rules:

* **Type:** MTH (Passive Cultivation Method) or SKL (Active Martial Skill).  
* **Req\_Resource:** The Item ID required to learn or upgrade this method.  
* **Scaling:** Which stat determines effectiveness (e.g., Atk, Spirit, Def).

---

## **Part 1: Cultivation Methods (Passive Scriptures)**

*The foundation of a character. Determines growth rates, Qi pool, and passive traits.*

| ID | Name | Tier | Focus | Req\_Resource (Breakthrough) | Passive Logic / Effect |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **MTH\_GEN\_01** | **Basic Breathing Scripture** | 1 | Balance | HRB\_001 (Spirit Grass) | Qi\_Regen \+= 0.5; Base method for mortals. |
| **MTH\_BDY\_01** | **Iron-Skin Manual** | 1 | Body | HRB\_006 (Iron Bark) | Phys\_Def \+= 10; Agi \-= 2; Hardens skin like wood. |
| **MTH\_FIRE\_01** | **Raging Flame Mantra** | 2 | Fire | CORE\_004 (Flame Lizard) | Fire\_Dmg \+= 10%; Body temperature rises. Immune to Cold. |
| **MTH\_WTR\_01** | **Tranquil River Sutra** | 2 | Water | HRB\_008 (Water Lotus) | Healing\_Eff \+= 20%; Qi acts as a passive heal over time. |
| **MTH\_SWD\_01** | **Azure Sword Heart** | 3 | Attack | WPN\_SWD\_01 (Any Sword) | Sword\_Mastery \+= 15%; Learning speed for Sword Skills doubled. |
| **MTH\_PSN\_01** | **Five-Venom Art** | 2 | Poison | CORE\_005 (Spider Core) | Psn\_Resist \+= 100%; Body blood becomes toxic to attackers. |
| **MTH\_SPD\_01** | **Wind-Chaser Steps** | 2 | Speed | CORE\_002 (Leopard Core) | Move\_Speed \+= 20%; Can traverse 2 tiles per tick. |
| **MTH\_ICE\_01** | **Glacial Bone Secret** | 4 | Ice | CORE\_026 (Bear Glacial) | Chill\_Aura \= True; Enemies within 1 tile slowed by 10%. |
| **MTH\_LGT\_01** | **Thunder-Forging Art** | 4 | Thunder | CORE\_012 (Sable Core) | Crit\_Rate \+= 10%; Qi attacks have chance to paralyze self. |
| **MTH\_BLD\_01** | **Blood Devil Scripture** | 5 | Forbidden | MAT\_024 (Leech Sac) | Life\_Steal \+= 5%; Drains HP on hit. Lowers Reputation daily. |
| **MTH\_MND\_01** | **Fox-Spirit Charm** | 5 | Illusion | MAT\_050 (Fox Pearl) | Charisma \+= 50; NPC aggression range reduced by 50%. |
| **MTH\_DRG\_01** | **Dragon-Transformation** | 7 | Body | MAT\_048 (Ape Blood) | Str \*= 2.0; Can equip Heavy Weapons without penalty. |
| **MTH\_VOID\_01** | **Void-Step Canon** | 6 | Space | CORE\_027 (Void Core) | Dodge\_Chance \+= 25%; Phasing in and out of reality. |
| **MTH\_NEC\_01** | **Yin-Corpse Way** | 4 | Necro | MAT\_014 (Ghost Skull) | Minion\_Cap \+= 3; Can control raised skeletons. |
| **MTH\_GLD\_01** | **Indestructible Golden Body** | 7 | Metal | SB\_07 (Mammoth Ivory) | Dmg\_Reduction \= 50%; Skin reflects metal weapons. |
| **MTH\_SUN\_01** | **Solar God Manual** | 9 | Yang | CORE\_028 (Sun Core) | AoE\_Burn \= True; Passive heat damage to all adjacent tiles. |
| **MTH\_TIME\_01** | **River of Time Flow** | 10 | Time | GOD\_009 (Time Scale) | Lifespan \= Infinite; Character stops aging. |
| **MTH\_CHS\_01** | **Primordial Chaos Sutra** | 10 | Chaos | GOD\_001 (Chaos Heart) | All\_Stats \*= 1.5; Can consume ANY item for XP. |
| **MTH\_DREAM\_01** | **Eternal Dream Art** | 8 | Soul | CORE\_029 (Moth Core) | Sleep\_Cultivation \= True; Gains XP while sleeping/offline. |
| **MTH\_STR\_01** | **Star-Constellation Body** | 5 | Cosmic | ORE\_008 (Star Steel) | Weight \+= 1000kg; Cannot be knocked back. |

---

## **Part 2: Martial Skills (Active Techniques)**

*Combat moves. These consume Qi/Stamina and often require specific weapons.*

### **2.1 Sword Skills (Requires WPN\_SWD)**

| ID | Name | Tier | Element | Cost | Logic / Effect |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **SKL\_SWD\_01** | **Iron Cleave** | 1 | Metal | 5 Qi | Dmg \= Atk \* 1.2; Simple downward slash. |
| **SKL\_SWD\_02** | **Flowing Water Strike** | 2 | Water | 8 Qi | Parry\_Chance \= 100%; Blocks next attack, then counters. |
| **SKL\_SWD\_03** | **Fire-Raven Slash** | 3 | Fire | 15 Qi | Range \= 3; Effect \= Burn; Shoots a bird of fire. |
| **SKL\_SWD\_04** | **Vacuum Blade** | 5 | Wind | 20 Qi | Area \= Line(5); Ignore\_Def \= 20%; Cuts through air and armor. |
| **SKL\_SWD\_05** | **Thousand Sword Rain** | 7 | Metal | 100 Qi | Area \= Circle(5); AoE nuke around player. |
| **SKL\_SWD\_06** | **Killing Intent Slash** | 10 | Death | 50% HP | Kill\_Threshold \= 20%; If enemy HP \< 20%, instant death. Linked to GOD\_005. |

### **2.2 Polearm Skills (Requires WPN\_SPR, WPN\_GLV, WPN\_HLB)**

| ID | Name | Tier | Element | Cost | Logic / Effect |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **SKL\_POL\_01** | **Boar Rush** | 1 | Earth | 10 Sta | Move(Target); Stun(1); Charge and stun. Linked to SD\_03. |
| **SKL\_POL\_02** | **Sweeping River** | 2 | Water | 15 Qi | Area \= Arc(3); Knockback(2); Hits all enemies in front and pushes them. |
| **SKL\_POL\_03** | **Viper Thrust** | 3 | Poison | 10 Qi | Dot\_Stack \+= 1; Applies poison from WPN\_SPR\_02. |
| **SKL\_POL\_04** | **Dragon Drill** | 5 | Metal | 30 Qi | Pierce \= 100%; Ignores all Shield/Defense stats. |
| **SKL\_POL\_05** | **Thunder-God Slam** | 6 | Thunder | 50 Qi | Radius \= 3; Stun(2); Linked to WPN\_HLB\_05. Massive AoE stun. |
| **SKL\_POL\_06** | **Void Anchor** | 7 | Space | 40 Qi | Root(Target, 5\_Turns); Prevents enemy movement. Linked to WPN\_HLB\_08. |

### **2.3 Unarmed/Body Skills (Requires Empty Hand or Fist Weapon)**

| ID | Name | Tier | Element | Cost | Logic / Effect |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **SKL\_FST\_01** | **Stone Breaking Fist** | 1 | Earth | 5 Sta | Dmg \= Str \* 1.5; Bonus damage to structures/walls. |
| **SKL\_FST\_02** | **White Tiger Claw** | 3 | Metal | 15 Qi | Bleed \= True; Rips armor. |
| **SKL\_FST\_03** | **Seismic Stomp** | 4 | Earth | 20 Qi | Knockdown(All\_Adjacent); Grounds flying enemies. |
| **SKL\_FST\_04** | **Heart-Stopping Palm** | 6 | Yin | 30 Qi | Bypass\_Armor; Dmg \= Soul\_Stat; Internal damage. |
| **SKL\_FST\_05** | **Devour Essence** | 8 | Void | 50 Qi | Heal \= Dmg\_Dealt; Lifesteal. Linked to GOD\_004. |

### **2.4 Movement & Utility Skills (No Weapon Req)**

| ID | Name | Tier | Type | Cost | Logic / Effect |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **SKL\_MOV\_01** | **Cloud Step** | 2 | Air | 5 Qi | Jump\_Distance \= 3; Hop over obstacles/water. Linked to SD\_34. |
| **SKL\_MOV\_02** | **Shadow Meld** | 3 | Dark | 10 Qi | Stealth \= True; Invisibility until attack. Linked to SD\_09. |
| **SKL\_MOV\_03** | **Blood Escape** | 5 | Blood | 20% HP | Teleport(Random, Range=50); Emergency flee. Drops MAT\_024 on use. |
| **SKL\_UTL\_01** | **Spirit Eye** | 1 | Mind | 1 Qi | Reveal\_Map(Radius=10); See Qi density/Herbs nearby. |
| **SKL\_UTL\_02** | **Soul Search** | 6 | Soul | 50 Qi | Get\_Info(Target\_Memory); Extract map data/secrets from defeated foe. |

### **2.5 Spell/Magic Arts (Requires Casting Tool or High Cultivation)**

| ID | Name | Tier | Element | Cost | Logic / Effect |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **SKL\_MAG\_01** | **Fireball** | 1 | Fire | 5 Qi | Projectile(Fire); Basic ranged attack. |
| **SKL\_MAG\_02** | **Earth Wall** | 2 | Earth | 10 Qi | Spawn\_Object(Wall, HP=100); Blocks movement/projectiles. |
| **SKL\_MAG\_03** | **Lightning Chain** | 4 | Thunder | 25 Qi | Bounce(3\_Targets); Hits multiple enemies. Linked to CORE\_015. |
| **SKL\_MAG\_04** | **Summon Golem** | 5 | Earth | 40 Qi | Summon(Mob\_ID=SD\_33); Summons Gargoyle ally. Requires MAT\_028. |
| **SKL\_MAG\_05** | **Blizzard Domain** | 7 | Ice | 10/Tick | Environment \= Tundra; Changes local biome to Ice temporarily. |

---

## **Part 3: Secret/Unique Manuals (Loot Only)**

*Dropped by specific Bosses. Cannot be learned from Sects.*

| ID | Name | Drop Source | Description |
| :---- | :---- | :---- | :---- |
| **MAN\_001** | **Nine-Revolutions Rebirth** | GB\_02 (Phoenix) | Allows user to craft CSM\_013 (Nirvana Pill). |
| **MAN\_002** | **Heaven-Swallowing Art** | GB\_04 (Leviathan) | Allows user to eat ORE and WPN items for XP. |
| **MAN\_003** | **Puppet Master Scripture** | SB\_14 (Bone Dragon) | Unlocks Crafting: Bone Golem and Skill: Raise Dead. |
| **MAN\_004** | **Myriad-Poison Manual** | SB\_13 (King Cobra) | Unlocks Crafting: God-Rot Poison. User becomes immune to all toxins. |
| **MAN\_005** | **Star-Fall Sword Array** | ORE\_008 (Star Steel Node) | Teaches a skill to control 7 Flying Swords simultaneously. |

