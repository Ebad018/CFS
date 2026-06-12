# Standard Operating Procedures (SOP) - Cultivation Fortress

## 1. Zero-Step Verification
**CRITICAL**: Before executing ANY task, you must:
1.  **Read the Master Design Document**: [Master Design Document CF.md](file:///d:/Programming%20Projects/CFS/Master%20Design%20Document%20CF.md).
    *   *philosophy*: "Numbers are Graphics".
2.  **Read the Relevant Sub-Document**:
    *   World Gen: [Technical Design Document_ World Generation & Environment (v2.md](file:///d:/Programming%20Projects/CFS/Technical%20Design%20Document_%20World%20Generation%20&%20Environment%20(v2.md)
    *   Sects: [Game Design Document_ Sect Mechanics & Hierarchy.md](file:///d:/Programming%20Projects/CFS/Game%20Design%20Document_%20Sect%20Mechanics%20&%20Hierarchy.md)
    *   Resources: [Design Document_ Master Resource & Asset Database.md](file:///d:/Programming%20Projects/CFS/Design%20Document_%20Master%20Resource%20&%20Asset%20Database.md)
3.  **Update Artifacts**:
    *   Check `task.md` for the current objective.
    *   Update `implementation_plan.md` before writing code.

## 2. Technical Standards
*   **Language**: **C++20**.
*   **Memory Management**: **Manual**. Avoid Garbage Collection patterns. Use RAII.
*   **Architecture**:
    *   **SimEngine**: Decoupled from rendering.
    *   **ECS**: Component-based entities defined by data.
    *   **Data**: External Markdown/CSV/SQLite loaded at runtime.
*   **Dependencies**: SFML (Window/Input), ImGui (UI), SQLite (Persistence).

## 3. Workflow Protocol
1.  **Acknowledge Task**: Read the documents.
2.  **Plan**: Write the Implementation Plan. Prompt the user for approval if unsure.
3.  **Execute**: Write the code.
4.  **Verify**: Build and Run.

## 4. Key Philosophies
*   **"Numbers are Graphics"**: Don't waste time on sprite rendering if the simulation depth isn't there.
*   **Roguelike Time**: Nothing moves until the player moves.
