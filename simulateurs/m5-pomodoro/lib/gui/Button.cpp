#include "Button.h"
#include "Free_Fonts.h"

using namespace Codingfield::UI;

/* Please, please, M5STack, use namespaces around your classes! */

Codingfield::UI::Button::Button(Widget* parent) : Widget(parent), parent(parent), font(&FreeSansBold9pt7b), oldFont(font), button(NoButton) {

}

Codingfield::UI::Button::Button(Widget* parent, Point position, Size size, const GFXfont* font) : Widget(parent, position, size), parent(parent), font(font), oldFont(font), button(NoButton) {

}

Color Codingfield::UI::Button::GetBackgroundColor() {
    return this->backgroundColor;
}

void Codingfield::UI::Button::SetBackgroundColor(Color c) {
  backgroundColorUpdated = true;
  this->backgroundColor = c;
}

void Codingfield::UI::Button::SetTextColor(Color c) {
  this->textColor = c;
}

void Codingfield::UI::Button::SetSelectedColor(Color c) {
    this->selectedColor = c;
}

void Codingfield::UI::Button::SetBorderColor(Color c) {
  this->borderColor = c;
}

void Codingfield::UI::Button::SetSelected(bool s) {
  if(isSelected != s) {
    wasSelected = isSelected;
    isSelected = s;
  }
}

void Codingfield::UI::Button::SetText(const std::string& t) {
  if(text != t) {
    if(text.empty())
      oldText = t;
    else
      this->oldText = this->text;
    this->text = t;
  }
}

void Codingfield::UI::Button::SetTitle(const std::string& t) {
  if(title != t) {
    if(title.empty())
      oldTitle = t;
    else
      oldTitle = title;
    title = t;
  }
}

void Codingfield::UI::Button::SetFont(const GFXfont* f) {
  if(f != nullptr) {
      this->oldFont = this->font;
      this->font = f;
  }
}

void Codingfield::UI::Button::SetRounded(bool r, int rad) {
    if(r) {
        radius = rad;
    }
    rounded = r;        
}

void Codingfield::UI::Button::SetBorder(bool b, int t) {
    withBorder = b;
    SetThickness(t);
}

void Codingfield::UI::Button::SetThickness(int t) {
    if(t > 0 && t <= 3)
        thickness = t;
}

void Codingfield::UI::Button::Draw() {
  if(IsHidden()) {
    if(!cleared) {
        if(rounded) {
              M5.Lcd.fillRoundRect(position.x, position.y, size.width, size.height, radius, parent->GetBackgroundColor());
        }
        else {
              M5.Lcd.fillRect(position.x, position.y, size.width, size.height, parent->GetBackgroundColor());
        }
        if(withBorder) {
            if(rounded) {
                M5.Lcd.drawRoundRect(position.x, position.y, size.width, size.height, radius, parent->GetBackgroundColor());
                if(thickness > 1)
                    M5.Lcd.drawRoundRect(position.x+1, position.y+1, size.width-2, size.height-2, radius, parent->GetBackgroundColor());
                if(thickness == 3)    
                    M5.Lcd.drawRoundRect(position.x+2, position.y+2, size.width-4, size.height-4, radius, parent->GetBackgroundColor());
            }
            else {                
                M5.Lcd.drawRect(position.x, position.y, size.width, size.height, parent->GetBackgroundColor());
                if(thickness > 1)
                    M5.Lcd.drawRect(position.x+1, position.y+1, size.width-2, size.height-2, parent->GetBackgroundColor());
                if(thickness == 3)
                    M5.Lcd.drawRect(position.x+2, position.y+2, size.width-4, size.height-4, parent->GetBackgroundColor());
            }
        }
        cleared = true;
    }
    return;
  }
  if(true) {
    bool forceUpdate = isInvalidated;
    if(forceUpdate) {
      //Serial.println("FORCEUPDATE");
    }
    int x = position.x + (size.width/2);
    int yText;
    int yTitle = 0;
    if(title.empty()) {
      yText = position.y + (size.height/2);
    }
    else {
      yText = position.y + (size.height/3);
      yTitle = position.y + ((size.height/3)*2);
    }

    if(backgroundColorUpdated || forceUpdate) {
      if(rounded) {
          M5.Lcd.fillRoundRect(position.x, position.y, size.width, size.height, radius, backgroundColor);
      }
      else {
          M5.Lcd.fillRect(position.x, position.y, size.width, size.height, backgroundColor);
      }
      backgroundColorUpdated = false;
      forceUpdate = true;
      cleared = false;
    }

    if(forceUpdate || (oldText != text)) {
      M5.Lcd.setTextDatum(MC_DATUM);
      M5.Lcd.setTextColor(backgroundColor);

      M5.Lcd.setFreeFont(oldFont);
      M5.Lcd.drawString(oldText.c_str(), x, yText);

      M5.Lcd.setFreeFont(font);    
      M5.Lcd.setTextColor(textColor);
      M5.Lcd.drawString(text.c_str(), x, yText);
      oldText = text;
      oldFont = font;
    }

    if(forceUpdate || (oldTitle != title)) {
      M5.Lcd.setTextDatum(MC_DATUM);
      M5.Lcd.setTextColor(backgroundColor);

      M5.Lcd.setFreeFont(oldFont);
      M5.Lcd.drawString(oldTitle.c_str(), x, yTitle);

      M5.Lcd.setFreeFont(font);    
      M5.Lcd.setTextColor(textColor);
      M5.Lcd.drawString(title.c_str(), x, yTitle);
      oldTitle = title;
      oldFont = font;
    }

    if(forceUpdate && withBorder) {
        if(rounded) {
            M5.Lcd.drawRoundRect(position.x, position.y, size.width, size.height, radius, borderColor);
            if(thickness > 1)
                M5.Lcd.drawRoundRect(position.x+1, position.y+1, size.width-2, size.height-2, radius, borderColor);
            if(thickness == 3)
                M5.Lcd.drawRoundRect(position.x+2, position.y+2, size.width-4, size.height-4, radius, borderColor);
        }
        else {
            M5.Lcd.drawRect(position.x, position.y, size.width, size.height, borderColor);
            if(thickness > 1)
                M5.Lcd.drawRect(position.x+1, position.y+1, size.width-2, size.height-2, borderColor);
            if(thickness == 3)
                M5.Lcd.drawRect(position.x+2, position.y+2, size.width-4, size.height-4, borderColor);
        }
    }

    if(forceUpdate || (wasSelected != isSelected)) {
      if(isSelected) {
        M5.Lcd.drawRect(position.x, position.y, size.width, size.height, selectedColor);
        M5.Lcd.drawRect(position.x+1, position.y+1, size.width-2, size.height-2, selectedColor);
        M5.Lcd.drawRect(position.x+2, position.y+2, size.width-4, size.height-4, selectedColor);
      }
    }
  }
  isInvalidated = false;
}

void Codingfield::UI::Button::EnableControls()  {
  controlsEnabled = true;
}

void Codingfield::UI::Button::DisableControls() {
  controlsEnabled = false;
}

void Codingfield::UI::Button::OnButtonAPressed() {
    if(button == ButtonA) {
        if(pressedCallback != nullptr) {
            pressedCallback(this, ButtonA, true);
        }
    }
}

void Codingfield::UI::Button::OnButtonAReleased() {
    if(button == ButtonA) {
        if(releasedCallback != nullptr) {
            releasedCallback(this, ButtonA, false);
        }
    }
}

void Codingfield::UI::Button::OnButtonBPressed() {
    if(button == ButtonB) {
        if(pressedCallback != nullptr) {
            pressedCallback(this, ButtonB, true);
        }
    }
}

void Codingfield::UI::Button::OnButtonBReleased() {
    if(button == ButtonB) {
        if(releasedCallback != nullptr) {
            releasedCallback(this, ButtonB, false);
        }
    }
}

void Codingfield::UI::Button::OnButtonCPressed() {
    if(button == ButtonC) {
        if(pressedCallback != nullptr) {
            pressedCallback(this, ButtonC, true);
        }
    }
}

void Codingfield::UI::Button::OnButtonCReleased() {
    if(button == ButtonC) {
        if(releasedCallback != nullptr) {
            releasedCallback(this, ButtonC, false);
        }
    }
}

void Codingfield::UI::Button::SetPressedCallback(IdButton button, std::function<bool (Codingfield::UI::Button* widget, IdButton button, bool pressed)> callback)
{
    this->button = button;
    pressedCallback = callback;
}

void Codingfield::UI::Button::SetReleasedCallback(IdButton button, std::function<bool (Codingfield::UI::Button* widget, IdButton button, bool pressed)> callback)
{
    this->button = button;
    releasedCallback = callback;
}

void Codingfield::UI::Button::Run() {
    Draw();
    if(!controlsEnabled)
        return;
    if(M5.BtnA.wasPressed()) 
    {
        OnButtonAPressed();
    }
    else if(M5.BtnA.wasReleased()) 
    {
        OnButtonAReleased();    
    }
    if(M5.BtnB.wasPressed()) 
    {
        OnButtonBPressed();
    }
    else if(M5.BtnB.wasReleased()) 
    {
        OnButtonBReleased();    
    }
    if(M5.BtnC.wasPressed()) 
    {
        OnButtonCPressed();
    }
    else if(M5.BtnC.wasReleased()) 
    {
        OnButtonCReleased();
    }
}
