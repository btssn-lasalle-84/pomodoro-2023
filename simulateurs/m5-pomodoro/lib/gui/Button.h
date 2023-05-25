#pragma once

#include "Widget.h"

namespace Codingfield {
  namespace UI {
    enum IdButton
    {
      NoButton,
      ButtonA,
      ButtonB,
      ButtonC
    };
    class Button : public Widget {
    public:
      Button(Widget* parent);
      Button(Widget* parent, Point position, Size size, const GFXfont* font=&FreeSansBold9pt7b);
      virtual Color GetBackgroundColor() override;
      void SetBackgroundColor(Color c);
      void SetTextColor(Color c);
      void SetSelectedColor(Color c);
      void SetBorderColor(Color c);
      virtual void SetSelected(bool s) override;
      void SetText(const std::string& t);
      void SetTitle(const std::string& t);
      void SetFont(const GFXfont* f);
      void SetRounded(bool r, int rad=8);
      void SetBorder(bool b, int t=1);
      void SetThickness(int t);
      void Draw() override;
      void Run() override;
      virtual void EnableControls() override;
      virtual void DisableControls() override;
      virtual void OnButtonAPressed() override;
      virtual void OnButtonBPressed() override;
      virtual void OnButtonCPressed() override;
      virtual void OnButtonAReleased() override;
      virtual void OnButtonBReleased() override;
      virtual void OnButtonCReleased() override;
      void SetPressedCallback(IdButton button, std::function<bool (Codingfield::UI::Button* widget, IdButton button, bool pressed)> callback);
      void SetReleasedCallback(IdButton button, std::function<bool (Codingfield::UI::Button* widget, IdButton button, bool pressed)> callback);
    protected:
      Widget* parent;
      Color backgroundColor = BLACK;
      bool backgroundColorUpdated = true;
      Color textColor = BLACK;
      Color selectedColor = RED;
      Color borderColor = BLACK;
      std::string text;
      std::string oldText;
      std::string title;
      std::string oldTitle;
      bool wasSelected = false;
      const GFXfont* font;
      const GFXfont* oldFont;
      bool rounded = true;
      bool withBorder = false;
      int radius = 8;
      int thickness = 1;
      bool controlsEnabled = true;
      bool cleared = true;
      IdButton button;
      std::function<bool (Codingfield::UI::Button* widget, IdButton button, bool pressed)> pressedCallback = nullptr;
      std::function<bool (Codingfield::UI::Button* widget, IdButton button, bool pressed)> releasedCallback = nullptr;     
    };
  }
}
