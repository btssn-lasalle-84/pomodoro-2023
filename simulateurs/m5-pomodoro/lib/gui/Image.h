#pragma once

#include "Widget.h"

namespace Codingfield {
  namespace UI {      
    class Image : public Widget {
    public:
      Image(Widget* parent);
      Image(Widget* parent, Point position, Size size);
      virtual Color GetBackgroundColor() override;      
      void SetBackgroundColor(Color c);
      void SetBorderColor(Color c);
      void SetBorder(bool b, int t=1);
      void SetThickness(int t);
      void SetImage(const char *image);
      void SetImage(fs::FS *fs, std::string path);
      void SetScale(double scale);
      void Draw() override;
      void Run() override;      
      
    protected:
      Widget* parent;
      const char *image = nullptr;
      fs::FS *fs = nullptr;
      std::string path;
      bool imageUpdated = true;
      double scale = 1.0;
      Color backgroundColor = BLACK;
      bool backgroundColorUpdated = true;
      Color borderColor = BLACK;
      bool withBorder = false;
      int thickness = 1;
      bool cleared = true;
    };
  }
}
