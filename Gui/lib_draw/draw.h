#pragma once

#include <Includes/Includes.hpp>

struct ImVec2;
struct ImColor;

class draw_lib
{
public:
    void filled_rect(const ImVec2& pos, const ImVec2& size, const ImColor& color);

    void outlined_rect(const ImVec2& start, const ImVec2& end, const ImColor& fill, const ImColor& outline, float thickness, ImU32 fill_col, bool filled, int fill_mode);

    void cornered_rect(const ImVec2& pos, const ImVec2& size, ImU32 col, ImU32 fill_col, bool filled, int fill_mode);

    void text(const ImVec2& pos, const char* str, const ImColor& color, bool center, bool draw_shadow);

    void text_shadowed(const ImVec2& pos, const char* str, const ImColor& color, ImFont* font, float font_size, bool center);

    void outlined_text(const ImVec2& pos, const char* str, const ImColor& text_color, const ImColor& outline_col, bool center, float font_size, bool draw_shadow);

    void health_bar(const ImVec2& pos, const ImVec2& size, float health, const ImColor& start_color, const ImColor& end_color, const ImColor& outline_clr);

    void armor_bar(const ImVec2& pos, const ImVec2& size, float health, const ImColor& start_color, const ImColor& end_color, const ImColor& outline_clr);

    void tracer(const ImVec2& start, const ImVec2& end, const ImColor& color, float thickness = 1.0f);
    void line(const ImVec2& p1, const ImVec2& p2, const ImColor& color, float thickness, bool outlined, const ImColor& outline_col);
    void circle(const ImVec2& pos, float radius, const ImColor& color, float thickness, bool outlined, const ImColor& outline_col);
};
inline draw_lib draw;




extern ImFont* esp_font;
extern ImFont* watermark_font; 
extern ImFont* main_font;



