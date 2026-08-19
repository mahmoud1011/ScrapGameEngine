#include "ScrapTheme.h"

namespace Scrap::Editor
{
    void applyScrapTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* c = style.Colors;
        using P = Palette;

        // Geometry. Square corners throughout - the panel chrome reads as machined
        // rather than soft, which is what separates this from the default dark theme
        // and from Unity's rounded controls.
        style.WindowRounding      = 0.0f;
        style.ChildRounding       = 0.0f;
        style.FrameRounding       = 2.0f;
        style.PopupRounding       = 2.0f;
        style.ScrollbarRounding   = 0.0f;
        style.GrabRounding        = 2.0f;
        style.TabRounding         = 0.0f;

        style.WindowBorderSize    = 1.0f;
        style.ChildBorderSize     = 1.0f;
        style.PopupBorderSize     = 1.0f;
        style.FrameBorderSize     = 1.0f;
        style.TabBarBorderSize    = 2.0f;

        style.WindowPadding       = ImVec2(8.0f, 8.0f);
        style.FramePadding        = ImVec2(8.0f, 4.0f);
        style.CellPadding         = ImVec2(6.0f, 4.0f);
        style.ItemSpacing         = ImVec2(8.0f, 5.0f);
        style.ItemInnerSpacing    = ImVec2(6.0f, 4.0f);
        style.IndentSpacing       = 18.0f;
        style.ScrollbarSize       = 12.0f;
        style.GrabMinSize         = 9.0f;

        style.WindowTitleAlign    = ImVec2(0.0f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_None;   // no collapse arrow in titles
        style.SeparatorTextBorderSize  = 1.0f;
        style.SeparatorTextAlign       = ImVec2(0.0f, 0.5f);
        style.DockingSeparatorSize     = 2.0f;

        // Text
        c[ImGuiCol_Text]                  = P::Ink;
        c[ImGuiCol_TextDisabled]          = P::InkFaint;
        c[ImGuiCol_TextSelectedBg]        = P::AccentDim;

        // Surfaces
        c[ImGuiCol_WindowBg]              = P::Ground;
        c[ImGuiCol_ChildBg]               = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_PopupBg]               = P::Surface;
        c[ImGuiCol_MenuBarBg]             = P::Sunken;
        c[ImGuiCol_Border]                = P::Line;
        c[ImGuiCol_BorderShadow]          = ImVec4(0, 0, 0, 0);

        // Inputs
        c[ImGuiCol_FrameBg]               = P::Sunken;
        c[ImGuiCol_FrameBgHovered]        = P::Raised;
        c[ImGuiCol_FrameBgActive]         = P::Hover;

        // Titles
        c[ImGuiCol_TitleBg]               = P::Void;
        c[ImGuiCol_TitleBgActive]         = P::Sunken;
        c[ImGuiCol_TitleBgCollapsed]      = P::Void;

        // Scrollbars
        c[ImGuiCol_ScrollbarBg]           = P::Void;
        c[ImGuiCol_ScrollbarGrab]         = P::Raised;
        c[ImGuiCol_ScrollbarGrabHovered]  = P::Hover;
        c[ImGuiCol_ScrollbarGrabActive]   = P::Accent;

        // Controls
        c[ImGuiCol_CheckMark]             = P::Accent;
        c[ImGuiCol_SliderGrab]            = P::Accent;
        c[ImGuiCol_SliderGrabActive]      = P::AccentHover;
        c[ImGuiCol_Button]                = P::Raised;
        c[ImGuiCol_ButtonHovered]         = P::Hover;
        c[ImGuiCol_ButtonActive]          = P::AccentDim;

        // Headers - tree nodes, selectables, collapsing headers
        c[ImGuiCol_Header]                = P::AccentDim;
        c[ImGuiCol_HeaderHovered]         = P::Hover;
        c[ImGuiCol_HeaderActive]          = P::AccentDim;

        c[ImGuiCol_Separator]             = P::Line;
        c[ImGuiCol_SeparatorHovered]      = P::Accent;
        c[ImGuiCol_SeparatorActive]       = P::AccentHover;

        c[ImGuiCol_ResizeGrip]            = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_ResizeGripHovered]     = P::AccentDim;
        c[ImGuiCol_ResizeGripActive]      = P::Accent;

        // Tabs. The active tab sits at Surface with an accent underline drawn by the
        // panel code, rather than being filled with the accent - filling it competes
        // with selection highlighting inside the panel.
        c[ImGuiCol_Tab]                   = P::Void;
        c[ImGuiCol_TabHovered]            = P::Raised;
        c[ImGuiCol_TabSelected]           = P::Ground;
        c[ImGuiCol_TabSelectedOverline]   = P::Accent;
        c[ImGuiCol_TabDimmed]             = P::Void;
        c[ImGuiCol_TabDimmedSelected]     = P::Sunken;
        c[ImGuiCol_TabDimmedSelectedOverline] = P::Line;

        // Docking
        c[ImGuiCol_DockingPreview]        = P::AccentDim;
        c[ImGuiCol_DockingEmptyBg]        = P::Void;

        // Tables
        c[ImGuiCol_TableHeaderBg]         = P::Sunken;
        c[ImGuiCol_TableBorderStrong]     = P::Line;
        c[ImGuiCol_TableBorderLight]      = P::Surface;
        c[ImGuiCol_TableRowBg]            = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_TableRowBgAlt]         = ImVec4(1, 1, 1, 0.018f);

        // Plots
        c[ImGuiCol_PlotLines]             = P::Accent;
        c[ImGuiCol_PlotLinesHovered]      = P::AccentHover;
        c[ImGuiCol_PlotHistogram]         = P::Accent;
        c[ImGuiCol_PlotHistogramHovered]  = P::AccentHover;

        c[ImGuiCol_DragDropTarget]        = P::AccentHover;
        c[ImGuiCol_NavCursor]             = P::Accent;
        c[ImGuiCol_NavWindowingHighlight] = P::Accent;
        c[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.45f);
        c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);
    }
}
