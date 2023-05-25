#pragma once

#include "Widget.h"

namespace Codingfield {
  namespace UI {      
    class Label : public Widget {
    public:
      Label(Widget* parent);
      Label(Widget* parent, Point position, Size size, const GFXfont* font=&FreeSansBold9pt7b);
      virtual Color GetBackgroundColor() override;
      void SetBackgroundColor(Color c);
      void SetTextColor(Color c);
      void SetSelectedColor(Color c);
      void SetBorderColor(Color c);
      virtual void SetSelected(bool s) override;
      void SetText(const std::string& t);
      void SetFont(const GFXfont* f);
      void SetRounded(bool r, int rad=8);
      void SetBorder(bool b, int t=1);
      void SetThickness(int t);
      void Draw() override;
      void Run() override;      
      
    protected:
      Widget* parent;
      Color backgroundColor = BLACK;
      bool backgroundColorUpdated = true;
      Color textColor = BLACK;
      Color selectedColor = RED;
      Color borderColor = BLACK;
      std::string text;
      std::string oldText;      
      bool wasSelected = false;
      const GFXfont* font;
      const GFXfont* oldFont;
      bool rounded = false;
      bool withBorder = false;
      int radius = 8;
      int thickness = 1;
      bool cleared = true;
    };
  }
}
