// starry_menu.cpp
//
// A clean, self-contained "starry night" themed ImGui menu skeleton.
// Vertical tab list (top -> bottom): Combat, Movement, Visual, Theme, Configs
//
// This file is a UI SKELETON ONLY. Each tab body is a placeholder panel —
// no gameplay-affecting logic (no aim assist, ESP, speed modification, etc.)
// is implemented. Wire your own application logic into the marked sections.
//
// Dependencies (not included, grab via your package manager / vcpkg / git submodule):
//   - Dear ImGui        https://github.com/ocornut/imgui
//   - GLFW3             https://www.glfw.org/
//   - OpenGL3 loader    (ImGui's built-in gl3w/glad backend is fine)
//
// Build (example, Linux/g++):
//   g++ starry_menu.cpp imgui/*.cpp imgui/backends/imgui_impl_glfw.cpp \
//       imgui/backends/imgui_impl_opengl3.cpp -Iimgui -Iimgui/backends \
//       -lglfw -lGL -ldl -o starry_menu
//
// Build (example, Windows/MSVC) — link glfw3.lib, opengl32.lib, add ImGui
// sources to your project the same way.

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

// ---------------------------------------------------------------------------
// Tabs, in the requested top-to-bottom order.
// ---------------------------------------------------------------------------
enum class MenuTab : int
{
    Combat = 0,
    Movement,
    Visual,
    Theme,
    Configs,
    COUNT
};

static const char* kTabNames[] = {
    "Combat",
    "Movement",
    "Visual",
    "Theme",
    "Configs"
};

// ---------------------------------------------------------------------------
// Simple starfield background, drawn behind the menu with ImDrawList.
// Purely cosmetic — twinkling dots on a dark navy gradient.
// ---------------------------------------------------------------------------
struct Star
{
    ImVec2 pos;
    float  radius;
    float  twinkleSpeed;
    float  twinklePhase;
};

class Starfield
{
public:
    void Init(int count, ImVec2 canvasSize)
    {
        stars.clear();
        stars.reserve(count);
        for (int i = 0; i < count; ++i)
        {
            Star s;
            s.pos.x        = static_cast<float>(rand() % static_cast<int>(canvasSize.x));
            s.pos.y        = static_cast<float>(rand() % static_cast<int>(canvasSize.y));
            s.radius       = 0.6f + static_cast<float>(rand() % 100) / 100.0f * 1.6f;
            s.twinkleSpeed = 0.5f + static_cast<float>(rand() % 100) / 100.0f * 1.5f;
            s.twinklePhase = static_cast<float>(rand() % 628) / 100.0f; // 0..2pi
            stars.push_back(s);
        }
    }

    void Draw(ImDrawList* drawList, ImVec2 origin, ImVec2 size, float time)
    {
        // Background gradient: deep navy -> almost-black.
        drawList->AddRectFilledMultiColor(
            origin,
            ImVec2(origin.x + size.x, origin.y + size.y),
            IM_COL32(10, 12, 30, 255),
            IM_COL32(15, 17, 40, 255),
            IM_COL32(5, 6, 18, 255),
            IM_COL32(8, 9, 24, 255));

        for (auto& s : stars)
        {
            float twinkle = 0.5f + 0.5f * sinf(time * s.twinkleSpeed + s.twinklePhase);
            int   alpha   = static_cast<int>(120 + twinkle * 135);
            ImU32 col     = IM_COL32(230, 230, 255, alpha);
            drawList->AddCircleFilled(
                ImVec2(origin.x + s.pos.x, origin.y + s.pos.y),
                s.radius,
                col);
        }
    }

private:
    std::vector<Star> stars;
};

// ---------------------------------------------------------------------------
// Theme setup — cool dark palette with a soft indigo/gold accent.
// ---------------------------------------------------------------------------
static void ApplyStarryNightTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors     = style.Colors;

    style.WindowRounding    = 10.0f;
    style.FrameRounding     = 6.0f;
    style.GrabRounding      = 6.0f;
    style.TabRounding       = 6.0f;
    style.ScrollbarRounding = 8.0f;
    style.WindowPadding     = ImVec2(14, 14);
    style.FramePadding      = ImVec2(10, 6);
    style.ItemSpacing       = ImVec2(10, 8);
    style.WindowBorderSize  = 1.0f;

    colors[ImGuiCol_WindowBg]         = ImVec4(0.04f, 0.045f, 0.09f, 0.96f);
    colors[ImGuiCol_ChildBg]          = ImVec4(0.05f, 0.06f, 0.11f, 0.55f);
    colors[ImGuiCol_Border]           = ImVec4(0.35f, 0.35f, 0.55f, 0.35f);
    colors[ImGuiCol_Text]             = ImVec4(0.92f, 0.92f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled]     = ImVec4(0.55f, 0.55f, 0.65f, 1.00f);

    colors[ImGuiCol_FrameBg]          = ImVec4(0.10f, 0.11f, 0.20f, 0.85f);
    colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.16f, 0.17f, 0.30f, 0.90f);
    colors[ImGuiCol_FrameBgActive]    = ImVec4(0.20f, 0.20f, 0.38f, 0.95f);

    colors[ImGuiCol_Header]           = ImVec4(0.20f, 0.18f, 0.40f, 0.80f);
    colors[ImGuiCol_HeaderHovered]    = ImVec4(0.28f, 0.24f, 0.55f, 0.90f);
    colors[ImGuiCol_HeaderActive]     = ImVec4(0.33f, 0.28f, 0.62f, 1.00f);

    colors[ImGuiCol_Button]           = ImVec4(0.16f, 0.15f, 0.30f, 0.85f);
    colors[ImGuiCol_ButtonHovered]    = ImVec4(0.26f, 0.22f, 0.48f, 0.95f);
    colors[ImGuiCol_ButtonActive]     = ImVec4(0.85f, 0.70f, 0.30f, 0.90f); // gold accent

    colors[ImGuiCol_CheckMark]        = ImVec4(0.85f, 0.70f, 0.30f, 1.00f);
    colors[ImGuiCol_SliderGrab]       = ImVec4(0.55f, 0.45f, 0.85f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.85f, 0.70f, 0.30f, 1.00f);

    colors[ImGuiCol_Tab]              = ImVec4(0.10f, 0.10f, 0.20f, 0.85f);
    colors[ImGuiCol_TabHovered]       = ImVec4(0.28f, 0.24f, 0.55f, 0.90f);
    colors[ImGuiCol_TabActive]        = ImVec4(0.22f, 0.19f, 0.44f, 1.00f);

    colors[ImGuiCol_TitleBg]          = ImVec4(0.04f, 0.045f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgActive]    = ImVec4(0.08f, 0.08f, 0.16f, 1.00f);
}

// ---------------------------------------------------------------------------
// Placeholder panel bodies. Replace the TODO sections with real logic/state.
// Kept deliberately inert — no external effects, just UI controls.
// ---------------------------------------------------------------------------
static void DrawCombatPanel()
{
    ImGui::TextColored(ImVec4(0.85f, 0.70f, 0.30f, 1.0f), "Combat");
    ImGui::Separator();
    ImGui::Spacing();
    // TODO: wire these controls to your own combat-related settings.
    static bool optionA = false;
    static bool optionB = false;
    static float sliderVal = 0.5f;
    ImGui::Checkbox("Option A", &optionA);
    ImGui::Checkbox("Option B", &optionB);
    ImGui::SliderFloat("Setting", &sliderVal, 0.0f, 1.0f);
}

static void DrawMovementPanel()
{
    ImGui::TextColored(ImVec4(0.85f, 0.70f, 0.30f, 1.0f), "Movement");
    ImGui::Separator();
    ImGui::Spacing();
    // TODO: wire these controls to your own movement-related settings.
    static bool optionA = false;
    static float sliderVal = 1.0f;
    ImGui::Checkbox("Option A", &optionA);
    ImGui::SliderFloat("Setting", &sliderVal, 0.0f, 2.0f);
}

static void DrawVisualPanel()
{
    ImGui::TextColored(ImVec4(0.85f, 0.70f, 0.30f, 1.0f), "Visual");
    ImGui::Separator();
    ImGui::Spacing();
    // TODO: wire these controls to your own display/visual settings.
    static bool optionA = false;
    static ImVec4 color = ImVec4(0.55f, 0.45f, 0.85f, 1.0f);
    ImGui::Checkbox("Option A", &optionA);
    ImGui::ColorEdit3("Accent Color", (float*)&color);
}

static void DrawThemePanel()
{
    ImGui::TextColored(ImVec4(0.85f, 0.70f, 0.30f, 1.0f), "Theme");
    ImGui::Separator();
    ImGui::Spacing();
    static int themeChoice = 0;
    const char* themes[] = { "Starry Night", "Midnight", "Aurora" };
    ImGui::Combo("Preset", &themeChoice, themes, IM_ARRAYSIZE(themes));
    ImGui::TextDisabled("Only 'Starry Night' is implemented in this skeleton.");
}

static void DrawConfigsPanel()
{
    ImGui::TextColored(ImVec4(0.85f, 0.70f, 0.30f, 1.0f), "Configs");
    ImGui::Separator();
    ImGui::Spacing();
    static char configName[64] = "default";
    ImGui::InputText("Config Name", configName, IM_ARRAYSIZE(configName));
    if (ImGui::Button("Save"))
    {
        // TODO: serialize your settings struct to disk.
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        // TODO: deserialize your settings struct from disk.
    }
}

// ---------------------------------------------------------------------------
// Main menu window: starfield background + left-side vertical tab list.
// ---------------------------------------------------------------------------
static void DrawStarryMenu(Starfield& starfield, float time)
{
    ImGui::SetNextWindowSize(ImVec2(640, 420), ImGuiCond_FirstUseEver);
    ImGui::Begin("Starry Night Menu", nullptr, ImGuiWindowFlags_NoCollapse);

    ImVec2 winPos  = ImGui::GetWindowPos();
    ImVec2 winSize = ImGui::GetWindowSize();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    starfield.Draw(dl, winPos, winSize, time);

    // Left column: vertical tab list, top to bottom in the requested order.
    static MenuTab activeTab = MenuTab::Combat;

    ImGui::BeginChild("TabList", ImVec2(150, 0), true);
    for (int i = 0; i < static_cast<int>(MenuTab::COUNT); ++i)
    {
        bool selected = (activeTab == static_cast<MenuTab>(i));
        if (ImGui::Selectable(kTabNames[i], selected, 0, ImVec2(0, 32)))
        {
            activeTab = static_cast<MenuTab>(i);
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right column: active panel content.
    ImGui::BeginChild("TabContent", ImVec2(0, 0), true);
    switch (activeTab)
    {
        case MenuTab::Combat:   DrawCombatPanel();   break;
        case MenuTab::Movement: DrawMovementPanel(); break;
        case MenuTab::Visual:   DrawVisualPanel();   break;
        case MenuTab::Theme:    DrawThemePanel();    break;
        case MenuTab::Configs:  DrawConfigsPanel();  break;
        default: break;
    }
    ImGui::EndChild();

    ImGui::End();
}

// ---------------------------------------------------------------------------
// Entry point: GLFW + OpenGL3 boilerplate to host the ImGui menu.
// ---------------------------------------------------------------------------
int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(900, 600, "Starry Night Menu", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ApplyStarryNightTheme();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Starfield starfield;
    starfield.Init(180, ImVec2(900, 600));

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawStarryMenu(starfield, static_cast<float>(glfwGetTime()));

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
