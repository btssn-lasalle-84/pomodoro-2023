#pragma once

#include "Bar.h"

namespace Codingfield {
  namespace UI {
    class ButtonInfoBar : public Bar {
    public:
      ButtonInfoBar() : Bar() {}
      ButtonInfoBar(Widget* parent, Point position, int32_t height) : Bar(parent, position, height) {}
      void Draw() override;
      void Run() override;
      void SetButtonAText(const std::string& t);
      void SetButtonBText(const std::string& t);
      void SetButtonCText(const std::string& t);
      /*virtual void EnableControls() override;
      virtual void DisableControls() override;
      virtual void OnButtonAPressed() override;
      virtual void OnButtonBPressed() override;
      virtual void OnButtonCPressed() override;
      virtual void OnButtonAReleased() override;
      virtual void OnButtonBReleased() override;
      virtual void OnButtonCReleased() override;*/
    private:
      //Widget* parent;
      Color color = WHITE;
      std::string btnAText;
      std::string oldBtnAText;
      std::string btnBText;
      std::string oldBtnBText;
      std::string btnCText;
      std::string oldBtnCText;
    };
  }
}
