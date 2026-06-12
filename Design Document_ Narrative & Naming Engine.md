# **Design Document: Narrative & Naming Engine**

Project: Cultivation Fortress

Scope: Procedural Text Generation (Names, Titles, Descriptions).

Core Logic: Semantic Assembly (combining "Meaning Tokens").

Total Token Count: \~300 Unique Tokens

---

## **1\. The Semantic Token Database (JSON Structure)**

The engine constructs names by combining \[Surname\] \+ \[Given\] or \[Prefix\] \+ \[Noun\] \+ \[Suffix\].

### **1.1 Surnames (Clan Names)**

*Determines lineage and starting traits for NPCs.*

| Token | Pinyin/Sound | Meaning | Flavor Tag | Rarity |
| :---- | :---- | :---- | :---- | :---- |
| **Ye** | Ye | Leaf | Nature, humble beginnings. | Common |
| **Li** | Li | Plum | Ancient, vast numbers. | Common |
| **Wang** | Wang | King | Arrogant, ruling class. | Common |
| **Zhang** | Zhang | Archer | Martial, disciplined. | Common |
| **Chen** | Chen | Dawn/Old | Traditional, steady. | Common |
| **Liu** | Liu | Willow | Flexible, adaptable. | Common |
| **Yang** | Yang | Poplar/Sun | Bright, Fire-affinity. | Common |
| **Huang** | Huang | Yellow | Earth-affinity, wealth. | Common |
| **Zhao** | Zhao | Shine | Imperial, ambitious. | Common |
| **Wu** | Wu | Martial | Battle-focused families. | Common |
| **Long** | Long | Dragon | **Royal**, powerful bloodline. | Rare |
| **Feng** | Feng | Phoenix | **Royal**, fire/rebirth affinity. | Rare |
| **Bai** | Bai | White | Pure, Ice/Light affinity. | Uncommon |
| **Hei** | Hei | Black | Secretive, Assassin families. | Uncommon |
| **Mo** | Mo | Demon/Ink | **Demonic**, forbidden arts. | Rare |
| **Gu** | Gu | Ancient | Keepers of lost history. | Rare |
| **Tie** | Tie | Iron | Blacksmith/Body Cultivators. | Uncommon |
| **Shi** | Shi | Stone | Stoic, defensive clans. | Uncommon |
| **Murong** | Murong | (Compound) | Aristocratic, distinct. | Epic |
| **Nangong** | Nangong | (Compound) | Southern Palace, wealthy. | Epic |
| **Sima** | Sima | (Compound) | Strategists, leaders. | Epic |
| **Dugu** | Dugu | Solitary | Sword masters, loners. | Epic |
| **Xue** | Xue | Snow | Northern, Ice-affinity. | Uncommon |
| **Lei** | Lei | Thunder | Explosive temper/power. | Uncommon |
| **Yun** | Yun | Cloud | Drifters, Wind-affinity. | Uncommon |

### **1.2 Given Names (Personal Identity)**

*Combined with Surnames. Hints at the NPC's destiny or parents' wishes.*

| Token | Meaning | Gender/Vibe | Affinity |
| :---- | :---- | :---- | :---- |
| **Chen** | Morning/Star | Male/Hopeful | Cosmic |
| **Feng** | Wind/Peak | Male/Sharp | Wind |
| **Wei** | Great/Power | Male/Strong | Body |
| **Jian** | Sword/Health | Male/Martial | Weapon |
| **Lei** | Thunder | Male/Aggressive | Thunder |
| **Huo** | Fire | Male/Hot | Fire |
| **Gang** | Steel | Male/Hard | Metal |
| **Ming** | Bright | Male/Smart | Light |
| **Tao** | Wave/Dao | Male/Deep | Water |
| **Jun** | Handsome/Ruler | Male/Charisma | Neutral |
| **Tian** | Sky/Heaven | Male/Arrogant | Air |
| **Shan** | Mountain | Male/Steady | Earth |
| **Hai** | Ocean | Male/Vast | Water |
| **Yi** | Righteous | Male/Moral | Light |
| **Bo** | Wave/Elder | Male/Wise | Water |
| **Yan** | Flame/Beauty | Female/Charisma | Fire |
| **Ling** | Spirit/Bell | Female/Mystic | Soul |
| **Xue** | Snow | Female/Cold | Ice |
| **Yue** | Moon | Female/Soft | Yin |
| **Hua** | Flower | Female/Beauty | Wood |
| **Lan** | Orchid | Female/Grace | Wood |
| **Qing** | Azure/Clear | Female/Pure | Water |
| **Mei** | Plum/Beautiful | Female/Classic | Wood |
| **Xin** | Heart/Mind | Female/Soul | Soul |
| **Yu** | Jade/Rain | Female/Precious | Water |
| **Lian** | Lotus | Female/Pure | Wood |
| **Shuang** | Frost | Female/Cold | Ice |
| **Ying** | Shadow/Cherry | Female/Elusive | Dark |
| **Zhu** | Bamboo/Pearl | Female/Resilient | Wood |
| **Rou** | Soft/Gentle | Female/Healing | Water |

---

## **2\. Sect & Organization Naming**

*Structure: \[Prefix\] \+ \[Core Noun\] \+ \[Suffix\]*

### **2.1 Prefixes (Adjectives)**

| Token | Meaning | Context Trigger |
| :---- | :---- | :---- |
| **Xuan** | Mystic | Spirit/Soul cultivation. |
| **Tian** | Heavenly | High tier, Sky biome. |
| **Xue** | Blood | Demonic, War, Killing. |
| **Zi** | Violet | Lightning/Fire, Royalty. |
| **Qing** | Azure | Wind/Wood, Righteous. |
| **Jin** | Golden | Metal, Wealth, Defense. |
| **Han** | Cold/Frigid | Ice biome, North. |
| **Lie** | Raging/Fierce | Fire/Berserker focus. |
| **You** | Nether/Dark | Ghost/Necromancy focus. |
| **Ling** | Spirit | Beast Taming/Herb focus. |
| **Ku** | Withered | Poison/Death focus. |
| **Tai** | Grand/Supreme | High tier, Arrogant. |
| **Chi** | Scarlet | Fire/Blood focus. |
| **Bai** | White | Bone/Ice/Cloud focus. |
| **Yin** | Hidden/Shadow | Assassin/Spy focus. |

### **2.2 Core Nouns (The Focus)**

| Token | Meaning | Context Trigger |
| :---- | :---- | :---- |
| **Jian** | Sword | Sword Arts (WPN\_SWD). |
| **Dao** | Saber/Blade | Glaive/Saber Arts (WPN\_GLV). |
| **Dan** | Pill | Alchemy focus (CSM). |
| **Qi** | Vessel/Tool | Refining focus (MAT). |
| **Shou** | Beast | Beast Taming (SD\_xx). |
| **Long** | Dragon | High tier, Reptile focus. |
| **Feng** | Phoenix | Fire/Rebirth focus. |
| **Hu** | Tiger | Combat/Aggression focus. |
| **Gui** | Ghost | Soul/Necro focus. |
| **Xing** | Star | Cosmic/Array focus. |
| **Yue** | Moon | Yin/Female focus. |
| **Ri** | Sun | Yang/Male focus. |
| **Yun** | Cloud | Movement/Wind focus. |
| **Lei** | Thunder | Lightning focus. |
| **Hai** | Ocean | Water focus. |
| **Shan** | Mountain | Earth/Body focus. |
| **Gu** | Bone | Necro/Body focus. |
| **Hun** | Soul | Mental attack focus. |

### **2.3 Suffixes (Organization Type)**

| Token | Meaning | Context Trigger |
| :---- | :---- | :---- |
| **Zong** | Sect | Standard. |
| **Men** | Gate | Smaller/Specialized group. |
| **Gu** | Valley | Secluded, Herb/Alchemy. |
| **Ge** | Pavilion | Trade/Info/Scripture. |
| **Dian** | Palace | High tier, Ruling class. |
| **Tang** | Hall | Combat/Mission focus. |
| **Dong** | Grotto | Demonic/Hidden/Cave. |
| **Shan** | Mountain | Defensive/Earth focus. |
| **Yuan** | Courtyard | Academy/Training focus. |
| **Jiao** | Cult/Religion | Religious/Fanatic. |
| **Bang** | Gang | Loose/Bandit group. |
| **Lou** | Tower | Surveillance/Archer. |

---

## **3\. Dynamic Title Generation (The Reputation System)**

Titles are generated when an Entity completes a feat.

Structure: \[Adjective\] \+ \[Noun\]

### **3.1 Condition-Based Adjectives**

| Token | Trigger Condition |
| :---- | :---- |
| **Blood-Soaked** | Kills \> 50 (General). |
| **Merciless** | Executed \> 10 Surrendered Enemies. |
| **Iron** | Body Defense \> 500\. |
| **Swift** | Agility \> 100 or SKL\_SPD user. |
| **Divine** | Reputation \> 1000 (Righteous). |
| **Vile** | Reputation \< \-1000 (Demonic). |
| **One-Armed** | Suffered "Severed Arm" injury. |
| **Undying** | Survived with \< 5% HP 3 times. |
| **Wealthy** | Spirit Stones \> 50,000. |
| **Crazed** | Mental State \< 10 (Heart Demon). |
| **Jade** | Charisma \> 90\. |
| **Ghost** | Stealth/Assassin kills \> 20\. |
| **Thunder** | Mastered MTH\_LGT\_01. |
| **Frost** | Mastered MTH\_ICE\_01. |
| **Blazing** | Mastered MTH\_FIRE\_01. |
| **Void** | Entered the Void Layer. |

### **3.2 Achievement-Based Nouns**

| Token | Trigger Condition |
| :---- | :---- |
| **Butcher** | Killed \> 100 Humans. |
| **Slayer** | Killed \> 100 Beasts. |
| **Hunter** | Killed a Saint Beast (SB\_xx). |
| **Emperor** | Controls \> 3 Cities. |
| **Scholar** | Learned \> 20 Manuals. |
| **Sage** | Dao Comprehension \> 100\. |
| **Demon** | Demonic Cultivation (MTH\_BLD) User. |
| **Fairy** | Female \+ High Charisma. |
| **Goddess** | Female \+ God Tier Realm. |
| **Ancestor** | Age \> 500 \+ Nascent Soul. |
| **Venerable** | Age \> 1000 \+ God Tier. |
| **Prodigy** | Reached Golden Core \< 30 Years old. |
| **Trash** | Stuck at Qi Condensation \> 60 Years. |
| **Thief** | Successfully stole \> 10 times. |
| **Traitor** | Expelled from Sect. |
| **Warlord** | Won \> 5 Sect Wars. |

---

## **4\. Specific Entity Title Logic (Examples)**

The engine parses the Bestiary and Weapon files to generate highly specific titles.

### **4.1 Beast Slayer Titles**

*Logic: \[Beast Name Element\] \+ \[Slayer/Bane\]*

* **Wolf-Bane:** Killed 50x SD\_01 (Iron-Back Wolf).  
* **Dragon-Slayer:** Killed SB\_14 (Bone Dragon) or GB\_01 (Chaos Dragon).  
* **Pest Control:** Killed 500x SD\_36 (Silver Ant) or SD\_48 (Termite).  
* **Ghost-Eater:** Killed SB\_08 (Netherfire Shark) or GB\_08 (Ghost Butterfly).

### **4.2 Weapon Mastery Titles**

*Logic: \[Weapon Type\] \+ \[Mastery Level\]*

* **Sword Saint:** Sword Intent \> 100\.  
* **Spear God:** Spear Intent \> 100\.  
* **Saber Demon:** Glaive Intent \> 100 \+ Demonic Alignment.  
* **Fist King:** Unarmed Intent \> 100\.

### **4.3 Event Titles**

* **Survivor of the Tide:** Survived a Beast Tide event.  
* **Chosen of Heaven:** Rolled "Heavenly Spirit Root" at spawn.  
* **Defier of Fate:** Successfully broke through with \< 10% Success Chance.  
* **Tribulation Breaker:** Survived a Heavenly Tribulation without taking damage.

---

## **5\. Procedural Description Templates**

The NarrativeEngine fills these templates with generated names.

* **NPC Intro:** "*\[Name\], the \[Title\], steps forward. They emanate a \[Adjective\] aura characteristic of the \[Sect Name\].*"  
  * *Example:* "Ye Chen, the Sword Saint, steps forward. They emanate a Sharp aura characteristic of the Mystic Sword Sect."  
* **Attack Log:** "*\[Name\] unleashes \[Skill Name\]\! The power of \[Element\] surrounds their \[Weapon\].*"  
  * *Example:* "Li Feng unleashes Vacuum Blade\! The power of Wind surrounds their Iron Wolf Sword."  
* **Sect Rumor:** "*Have you heard? The \[Sect Name\] has declared war on the \[Sect Name\] over a \[Resource Name\] mine\!*"  
  * *Example:* "Have you heard? The Blood Demon Grotto has declared war on the Azure Cloud Pavilion over a Star Steel mine\!"

