# **Design Document: Master Weapon Database (Polearms & Blades)**

Project: Cultivation Fortress

Total Entries: 60 Unique Weapons

Parsing Rules:

* **Recipe:** Lists the primary ID required to craft.  
* **Scaling:** Referenced by Tier (1-10).  
* **Type:** SWD (Sword), SPR (Spear), GLV (Glaive), HLB (Halberd).

---

## **Part 1: Swords (The Dao of the Blade)**

*Balanced weapons favoring speed and technique.*

| ID | Name | Tier | Type | Recipe / Core Material | Stats / Effect Tag | Description |
| :---- | :---- | :---- | :---- | :---- | :---- | :---- |
| **WPN\_SWD\_01** | **Iron Wolf Sword** | 1 | SWD | MAT\_001 (Iron Claw) | Bleed\_Chance | Basic sword forged with wolf claws for a serrated edge. |
| **WPN\_SWD\_02** | **Cold Iron Blade** | 2 | SWD | ORE\_006 (Cold Iron) | Cold\_Dmg | Retains a chill that slows enemy Qi circulation. |
| **WPN\_SWD\_03** | **Snake-Belly Jian** | 2 | SWD | MAT\_007 (Python Scale) | Flexibility | Can bend around shields to strike the user. |
| **WPN\_SWD\_04** | **Crimson Fire Saber** | 2 | SWD | MAT\_004 (Flame Sac) | Fire\_Dmg | The blade is coated in oil that ignites on impact. |
| **WPN\_SWD\_05** | **Swordfish Rapier** | 3 | SWD | MAT\_020 (Swordfish Bill) | Pierce\_High | Natural bone weapon. Penetrates heavy armor. |
| **WPN\_SWD\_06** | **Thunder-Spark Sword** | 3 | SWD | CORE\_015 (Eel Core) | Stun\_Chance | Shocks enemies on contact. Unstable. |
| **WPN\_SWD\_07** | **Shadow Fang** | 3 | SWD | MAT\_009 (Shadow Fur) | Stealth\_Atk | Invisible in low light. Good for assassins. |
| **WPN\_SWD\_08** | **Mantis Wing Blade** | 7 | SWD | SB\_09 (Mantis Drop) | Wind\_Slash | Swings release vacuum blades (Range \+2). |
| **WPN\_SWD\_09** | **Ice-Soul Katana** | 4 | SWD | CORE\_026 (Bear Glacial) | Freeze\_Core | Freezes the blood of enemies. |
| **WPN\_SWD\_10** | **Obsidian Heavy Sword** | 3 | SWD | ORE\_011 (Obsidian) | Heat\_Res | Massive, heavy blade. Cannot be melted. |
| **WPN\_SWD\_11** | **Star-Fall Greatsword** | 4 | SWD | ORE\_008 (Star Steel) | Crushing | Weighs 1000kg. Uses weight rather than edge. |
| **WPN\_SWD\_12** | **Void Cutter** | 6 | SWD | MAT\_039 (Void Scale) | Ignore\_Armor | Teleports past armor to cut flesh directly. |
| **WPN\_SWD\_13** | **White Tiger Slaughter Sword** | 10 | SWD | GOD\_005 (Killing Law) | Insta\_Kill | Radiates killing intent. Stops weak hearts. |
| **WPN\_SWD\_14** | **Dragon-Tooth Saber** | 7 | SWD | MAT\_044 (Dragon Bone) | Dragon\_Fear | Suppresses all Beast-type enemies. |
| **WPN\_SWD\_15** | **Phoenix Feather Jian** | 8 | SWD | MAT\_040 (Sun Petal) | Rebirth\_Flame | Heals wielder when dealing damage. |

---

## **Part 2: Spears (The King of Weapons)**

*Range and piercing power. Favored by generals.*

| ID | Name | Tier | Type | Recipe / Core Material | Stats / Effect Tag | Description |
| :---- | :---- | :---- | :---- | :---- | :---- | :---- |
| **WPN\_SPR\_01** | **Boar-Tusk Spear** | 1 | SPR | MAT\_003 (Boar Skin/Tusk) | Durability | Crude but sturdy bone-tipped spear. |
| **WPN\_SPR\_02** | **Scorpion Stinger Lance** | 2 | SPR | MAT\_011 (Stinger) | Poison\_Inj | Injects neurotoxin on deep hits. |
| **WPN\_SPR\_03** | **Eagle Beak Pike** | 2 | SPR | MAT\_012 (Crimson Beak) | Armor\_Pen | Designed to punch through Spirit Shields. |
| **WPN\_SPR\_04** | **Lightning Rod** | 3 | SPR | MAT\_035 (Thunder Horn) | Chain\_Light | Acts as a lightning rod for Thunder Arts. |
| **WPN\_SPR\_05** | **Blood-Drinker Spear** | 4 | SPR | MAT\_024 (Leech Sac) | Life\_Steal | The shaft pulses. Drains enemy Qi. |
| **WPN\_SPR\_06** | **Golden Lion Spear** | 3 | SPR | CORE\_007 (Lion Core) | Sonic\_Boom | Thrusts create sonic booms that stun. |
| **WPN\_SPR\_07** | **Termite Acid Spear** | 3 | SPR | MAT\_041 (Termite Acid) | Corrosion | Melts enemy weapons on parry. |
| **WPN\_SPR\_08** | **Cryo-Wyrm Lance** | 7 | SPR | MAT\_047 (Ice Crystal) | Glacial\_Wall | Spinning the spear creates an ice wall. |
| **WPN\_SPR\_09** | **Magma Spine** | 4 | SPR | CORE\_019 (Magma Core) | Splash\_Dmg | Tip drips molten lava constantly. |
| **WPN\_SWD\_10** | **Wind-Chaser Pike** | 2 | SPR | MAT\_002 (Agility Tendon) | Atk\_Speed | Extremely light. Allows rapid thrusting. |
| **WPN\_SPR\_11** | **Ivory God-Spear** | 7 | SPR | SB\_07 (Mammoth Drop) | Unstoppable | Cannot be blocked, only dodged. |
| **WPN\_SPR\_12** | **Sun-God Lance** | 10 | SPR | GOD\_007 (Sunflame) | Solar\_Flare | Emits light that blinds everyone but the user. |
| **WPN\_SPR\_13** | **Silver Ant Pike** | 3 | SPR | MAT\_031 (Mandible) | Refining | Can be used to stir alchemy cauldrons. |
| **WPN\_SPR\_14** | **Bamboo Spirit Spear** | 2 | SPR | CORE\_006 (Wood Core) | Regen | Repairs itself if broken. |
| **WPN\_SPR\_15** | **Nether-Bone Lance** | 7 | SPR | SB\_14 (Bone Dragon) | Necro\_Raise | Enemies killed by this rise as skeletons. |

---

## **Part 3: Glaives (Heavy Cutting Weapons)**

*Sweeping attacks and crowd control. High strength requirement.*

| ID | Name | Tier | Type | Recipe / Core Material | Stats / Effect Tag | Description |
| :---- | :---- | :---- | :---- | :---- | :---- | :---- |
| **WPN\_GLV\_01** | **Iron-Back Glaive** | 1 | GLV | CORE\_001 (Wolf Core) | Cleave | Basic polearm. Good for sweeping mobs. |
| **WPN\_GLV\_02** | **Shark-Fin Guandao** | 3 | GLV | MAT\_023 (Shark Fin) | Hydro\_Cut | Swings leave a trail of razor water. |
| **WPN\_GLV\_03** | **Obsidian Cleaver** | 4 | GLV | MAT\_033 (Obsidian Plate) | Shatter | Heavy volcanic glass. High crit, low durability. |
| **WPN\_GLV\_04** | **Green Dragon Crescent** | 5 | GLV | MAT\_044 (Dragon Bone) | Spirit\_Wound | Damages the soul as well as the body. |
| **WPN\_GLV\_05** | **Venom-Crescent Glaive** | 7 | GLV | SB\_13 (Cobra Drop) | Zone\_Poison | Spinning creates a cloud of toxic gas. |
| **WPN\_GLV\_06** | **Thunder-Roc Scythe** | 8 | GLV | MAT\_046 (Roc Feather) | Teleport\_Hit | User blinks to the target on impact. |
| **WPN\_GLV\_07** | **Sand-Worm Slicer** | 2 | GLV | MAT\_037 (Worm Seg) | Burrow\_Strike | Can strike enemies underground. |
| **WPN\_GLV\_08** | **Vulture-Wing Glaive** | 3 | GLV | MAT\_032 (Feather) | Disease | Wounds from this weapon rot instantly. |
| **WPN\_GLV\_09** | **Crystal-Light Glaive** | 4 | GLV | MAT\_017 (Crab Chitin) | Reflect | Can bat away ranged spell attacks. |
| **WPN\_GLV\_10** | **Chaos-Eater Scythe** | 10 | GLV | GOD\_001 (Chaos Heart) | Delete\_Tile | A swing can erase a map tile from existence. |
| **WPN\_GLV\_11** | **Blood-Ape Moon Blade** | 7 | GLV | MAT\_048 (Ape Blood) | Rage\_Build | Dmg increases the lower your HP gets. |
| **WPN\_GLV\_12** | **Ogre-Bone Heavy Glaive** | 4 | GLV | MAT\_043 (Ogre Bone) | Knockback | Sends human-sized enemies flying. |
| **WPN\_GLV\_13** | **Silk-Cutter** | 3 | GLV | MAT\_042 (Cocoon) | Anti\_Air | Designed to cut flying beasts out of the sky. |
| **WPN\_GLV\_14** | **Siren-Song Glaive** | 4 | GLV | MAT\_021 (Siren Pearl) | Confusion | The blade hums. Enemies attack their allies. |
| **WPN\_GLV\_15** | **Netherfire Warblade** | 7 | GLV | MAT\_045 (Nether Scale) | Soul\_Burn | Burns the enemy's EXP/Cultivation base. |

---

## **Part 4: Halberds (Hybrid Polearms)**

*Versatile weapons combining axe, hook, and spear.*

| ID | Name | Tier | Type | Recipe / Core Material | Stats / Effect Tag | Description |
| :---- | :---- | :---- | :---- | :---- | :---- | :---- |
| **WPN\_HLB\_01** | **Crab-Claw Halberd** | 2 | HLB | MAT\_017 (Crab Chitin) | Grapple | The hook can disarm opponents. |
| **WPN\_HLB\_02** | **Bear-Paw Poleaxe** | 3 | HLB | MAT\_001 \+ SD\_10 | Maul | Crush damage \+ Bleed damage. |
| **WPN\_HLB\_03** | **Turtle-Breaker** | 4 | HLB | MAT\_025 (Turtle Shell) | Shield\_Break | Designed to shatter defensive formations. |
| **WPN\_HLB\_04** | **Demon-Face Halberd** | 6 | HLB | HRB\_030 (Demon Root) | Fear\_Aura | Low-tier beasts flee on sight. |
| **WPN\_HLB\_05** | **Thunder-Crash Halberd** | 4 | HLB | CORE\_023 (Goat Core) | AoE\_Shock | Slams ground to create electric shockwave. |
| **WPN\_HLB\_06** | **Crocodile-Tooth Halberd** | 7 | HLB | SB\_19 (Croc Drop) | Magma\_Trap | Hook drags enemies into lava pools. |
| **WPN\_HLB\_07** | **Gargoyle Stone-Axe** | 3 | HLB | MAT\_028 (Gargoyle) | Petrify | Chance to turn enemy limbs to stone. |
| **WPN\_HLB\_08** | **Void-Anchor Halberd** | 6 | HLB | MAT\_039 (Void Scale) | Root | Pins enemies to a specific coordinate in space. |
| **WPN\_HLB\_09** | **Time-River Halberd** | 10 | HLB | GOD\_009 (Time Scale) | Slow\_Time | Enemy action speed reduced by 50%. |
| **WPN\_HLB\_10** | **Fox-Fire Ji** | 7 | HLB | MAT\_050 (Fox Pearl) | Illusion\_Hit | Attacks from false directions. Hard to block. |
| **WPN\_HLB\_11** | **Toad-Tongue Halberd** | 3 | HLB | CORE\_014 (Toad Core) | Reach+ | The shaft can extend to strike distant foes. |
| **WPN\_HLB\_12** | **Mithril Crescent** | 4 | HLB | ORE\_009 (Mithril) | Spell\_Channel | Can cast spells through the weapon tip. |
| **WPN\_HLB\_13** | **Blood-Ginseng Axe** | 2 | HLB | HRB\_002 (Ginseng) | Self\_Heal | Wooden shaft grows roots into enemy flesh. |
| **WPN\_HLB\_14** | **Abyssal Anchor** | 5 | HLB | CORE\_022 (Obsidian) | Gravity\_Up | Increases gravity on the target hit. |
| **WPN\_HLB\_15** | **World-Shell Breaker** | 10 | HLB | GOD\_003 (Shell Frag) | Indestructible | Can block attacks from other God Weapons. |

