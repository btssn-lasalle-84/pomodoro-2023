#pragma once

#include "Widget.h"

namespace Codingfield {
  namespace UI {
    class Bar : public Widget {
    public:
      Bar() : Widget(nullptr, Point(), Size()) {}
      Bar(Widget* parent, Point position, int32_t height) : Widget(parent, position, Size(parent->GetSize().width, height)) {}
      virtual Color GetBackgroundColor() override;
      void SetBackgroundColor(Color c);
      void Draw() override;
      void Run() override;
    private:
      Color color = WHITE;
    };
  }
}
