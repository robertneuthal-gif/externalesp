#include "draw.h"


void draw_lib::filled_rect(const ImVec2& pos, const ImVec2& size, const ImColor& color)
{
    auto draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), color);
}

void draw_lib::health_bar(const ImVec2& pos, const ImVec2& size, float health, const ImColor& start_color, const ImColor& end_color, const ImColor& outline_clr)
{
  auto draw_list = ImGui::GetBackgroundDrawList();
  const float outline_thickness = 0.9f;
  ImVec2 outline_start(pos.x - outline_thickness, pos.y - outline_thickness);
  ImVec2 outline_end(pos.x + size.x + outline_thickness, pos.y + size.y + outline_thickness);
  draw_list->AddRect(outline_start, outline_end, outline_clr, 0.0f, 0, outline_thickness);
  draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImColor(40, 40, 40, (int)(outline_clr.Value.w * 255)));

  float fill_height = size.y * health;
  ImVec2 fill_start(pos.x, pos.y + size.y - fill_height);
  ImVec2 fill_end(pos.x + size.x, pos.y + size.y);
  draw_list->AddRectFilledMultiColor(fill_start, fill_end, start_color, start_color, end_color, end_color);
}



void draw_lib::armor_bar(const ImVec2& pos, const ImVec2& size, float health, const ImColor& start_color, const ImColor& end_color, const ImColor& outline_clr)
{
    auto draw_list = ImGui::GetBackgroundDrawList();
    const float outline_thickness = 0.9f;
    ImVec2 outline_start(pos.x - outline_thickness, pos.y - outline_thickness);
    ImVec2 outline_end(pos.x + size.x + outline_thickness, pos.y + size.y + outline_thickness);
    draw_list->AddRect(outline_start, outline_end, outline_clr, 0.0f, 0, outline_thickness);
    draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImColor(40, 40, 40, (int)(outline_clr.Value.w * 255)));

  float fill_height = size.y * health;
  ImVec2 fill_start(pos.x, pos.y + size.y - fill_height);
  ImVec2 fill_end(pos.x + size.x, pos.y + size.y);
  draw_list->AddRectFilledMultiColor(fill_start, fill_end, start_color, start_color, end_color, end_color);
}




void draw_lib::outlined_rect(const ImVec2& start, const ImVec2& end, const ImColor& fill, const ImColor& outline, float thickness, ImU32 fill_col, bool filled, int fill_mode)
{
    auto draw = ImGui::GetBackgroundDrawList();

    float X = start.x;
    float Y = start.y;
    float W = end.x - start.x;
    float H = end.y - start.y;

    if (filled)
    {
      if (fill_mode == 0)
      {
        draw->AddRectFilled({ X, Y }, { X + W, Y + H }, fill_col);
      }
      else if (fill_mode == 1)
      {
        ImU32 topColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.2f));
        draw->AddRectFilledMultiColor(
          ImVec2(X, Y), ImVec2(X + W, Y + H),
          topColor, topColor, fill_col, fill_col
        );

      }
    }

    draw->AddRect(ImVec2(start.x - 1, start.y - 1), ImVec2(end.x + 1, end.y + 1), outline, 0.0f, 0, thickness + 1.2f);
    draw->AddRect(ImVec2(start.x + 1, start.y + 1), ImVec2(end.x - 1, end.y - 1), outline, 0.0f, 0, thickness + 1.2f);
    draw->AddRect(start, end, fill, 0.0f, 0, thickness);
}



void draw_lib::cornered_rect(const ImVec2& pos, const ImVec2& size, ImU32 col, ImU32 fill_col, bool filled, int fill_mode)
{
  auto draw = ImGui::GetBackgroundDrawList();

  float X = pos.x;
  float Y = pos.y;
  float W = size.x;
  float H = size.y;

  float lineW = W / 4.f;
  float lineH = H / 4.f;
  float lineT = 1.0f;

  auto outline = IM_COL32(0, 0, 0, col >> 24);

  if (filled)
  {
    if (fill_mode == 0)
    {
      draw->AddRectFilled({ X, Y }, { X + W, Y + H }, fill_col);
    }
    else if (fill_mode == 1)
    {
      ImU32 topColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.2f));
      draw->AddRectFilledMultiColor(
        ImVec2(X, Y), ImVec2(X + W, Y + H),
        topColor, topColor, fill_col, fill_col
      );

    }
  }

  float outline_thickness = lineT + 2.0f;

  draw->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), outline, outline_thickness);
  draw->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), outline, outline_thickness);

  draw->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), outline, outline_thickness);
  draw->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), outline, outline_thickness);

  draw->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), outline, outline_thickness);
  draw->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), outline, outline_thickness);

  draw->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), outline, outline_thickness);
  draw->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), outline, outline_thickness);

  draw->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), col, lineT);
  draw->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), col, lineT);

  draw->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), col, lineT);
  draw->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), col, lineT);

  draw->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), col, lineT);
  draw->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), col, lineT);

  draw->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), col, lineT);
  draw->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), col, lineT);
}




void draw_lib::text(const ImVec2& pos, const char* str, const ImColor& color, bool center, bool draw_shadow)
{
    auto draw_list = ImGui::GetBackgroundDrawList();
    ImVec2 draw_pos = pos;

    if (center)
    {
        ImVec2 text_size = ImGui::CalcTextSize(str);
        draw_pos.x -= text_size.x * 0.5f;
        draw_pos.y -= text_size.y * 0.5f;
    }

    if (draw_shadow)
    {
        ImVec2 shadow_offset = ImVec2(2, 2); 
        ImColor shadow_color = ImColor(0, 0, 0, 128); 
        ImVec2 shadow_pos = ImVec2(draw_pos.x + shadow_offset.x, draw_pos.y + shadow_offset.y);


        draw_list->AddText(shadow_pos, shadow_color, str);
    }

    draw_list->AddText(draw_pos, color, str);
}




void draw_lib::text_shadowed(const ImVec2& pos, const char* str, const ImColor& color, ImFont* font, float font_size, bool center) {
    auto draw_list = ImGui::GetBackgroundDrawList();
    ImVec2 draw_pos = pos;

    if (center) {
        ImVec2 text_size = ImGui::CalcTextSize(str, nullptr, false, font_size);
        draw_pos.x -= text_size.x * 0.5f;
        draw_pos.y -= text_size.y * 0.5f;
    }

    auto alpha = (int)(color.Value.w * 255);
    ImU32 shadow_color = IM_COL32(0, 0, 0, alpha);

    if (alpha > 0) {
        ImVec2 offsets[] = {
            ImVec2(-1.f, 1.f), ImVec2(1.f, -1.f), ImVec2(1.f, 1.f), ImVec2(-1.f, -1.f),
            ImVec2(0.f, 1.f),  ImVec2(0.f, -1.f), ImVec2(1.f, 0.f),  ImVec2(-1.f, 0.f)
        };

        for (const auto& offset : offsets) {
            ImVec2 shadow_pos = ImVec2(draw_pos.x + offset.x, draw_pos.y + offset.y);
            draw_list->AddText(font, font_size, shadow_pos, shadow_color, str);
        }
    }

    draw_list->AddText(font, font_size, draw_pos, color, str);
}



void draw_lib::outlined_text(const ImVec2& pos, const char* str, const ImColor& text_color, const ImColor& outline_col, bool center, float font_size, bool draw_shadow)
{
    auto draw_list = ImGui::GetBackgroundDrawList();

    ImFont* old_font = g_Variables.m_DrawFont;
    float old_scale = ImGui::GetFont()->Scale;

    ImGui::GetFont()->Scale = font_size / old_font->FontSize;
    ImGui::PushFont(old_font);

    ImVec2 draw_pos = pos;

    if (center)
    {
        ImVec2 text_size = old_font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, str);
        draw_pos.x -= text_size.x * 0.5f;
        draw_pos.y -= text_size.y * 0.5f;
    }

    if (draw_shadow)
    {
        ImVec2 shadow_offset = ImVec2(2, 2);
        ImColor shadow_color = ImColor(0, 0, 0, 128);

        draw_list->AddText(old_font, font_size, ImVec2(draw_pos.x + shadow_offset.x, draw_pos.y + shadow_offset.y), shadow_color, str);
    }

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            if (x != 0 || y != 0)
            {
                draw_list->AddText(old_font, font_size, ImVec2(draw_pos.x + x, draw_pos.y + y), outline_col, str);
            }
        }
    }

    draw_list->AddText(old_font, font_size, draw_pos, text_color, str);

    ImGui::PopFont();
    ImGui::GetFont()->Scale = old_scale;
}


void draw_lib::tracer(const ImVec2& start, const ImVec2& end, const ImColor& color, float thickness)
{
    auto draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddLine(start, end, color, thickness);
}


void draw_lib::line(const ImVec2& p1, const ImVec2& p2, const ImColor& color, float thickness, bool outlined, const ImColor& outline_col)
{
    auto draw_list = ImGui::GetBackgroundDrawList();
    draw_list->Flags |= ImDrawListFlags_AntiAliasedLines;

    if (outlined)
    {
        float outline_thickness = thickness + 0.5f; 
        draw_list->AddLine(p1, p2, outline_col, outline_thickness);
    }

    draw_list->AddLine(p1, p2, color, thickness);
}

void draw_lib::circle(const ImVec2& pos, float radius, const ImColor& color, float thickness, bool outlined, const ImColor& outline_col)
{
    auto draw_list = ImGui::GetBackgroundDrawList();
    draw_list->Flags |= ImDrawListFlags_AntiAliasedLines;

    if (outlined)
    {
        float outline_thickness = thickness + 0.5f;
        draw_list->AddCircle(pos, radius, outline_col, 100, outline_thickness);
    }

    draw_list->AddCircle(pos, radius, color, 100, thickness);
}




ImFont* esp_font = nullptr;
ImFont* watermark_font = nullptr;
ImFont* main_font = nullptr;