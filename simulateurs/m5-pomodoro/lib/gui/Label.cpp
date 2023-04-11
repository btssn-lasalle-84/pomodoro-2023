#include "Label.h"
#include "Free_Fonts.h"

using namespace Codingfield::UI;

/* Please, please, M5STack, use namespaces around your classes! */

Codingfield::UI::Label::Label(Widget* parent) : Widget(parent), parent(parent), font(&FreeSansBold9pt7b), oldFont(font) {

}

Codingfield::UI::Label::Label(Widget* parent, Point position, Size size, const GFXfont* font) : Widget(parent, position, size), parent(parent), font(font), oldFont(font) {

}

Color Codingfield::UI::Label::GetBackgroundColor() {
    return this->backgroundColor;
}

void Codingfield::UI::Label::SetBackgroundColor(Color c) {
  backgroundColorUpdated = true;
  this->backgroundColor = c;
}

void Codingfield::UI::Label::SetTextColor(Color c) {
  this->textColor = c;
}

void Codingfield::UI::Label::SetSelectedColor(Color c) {
  this->selectedColor = c;
}

void Codingfield::UI::Label::SetBorderColor(Color c) {
  this->borderColor = c;
}

void Codingfield::UI::Label::SetSelected(bool s) {
  if(isSelected != s) {
    wasSelected = isSelected;
    isSelected = s;
  }
}

void Codingfield::UI::Label::SetText(const std::string& t) {
  if(text != t) {
    if(text.empty())
      oldText = t;
    else
      this->oldText = this->text;
    this->text = t;
  }
}

void Codingfield::UI::Label::SetFont(const GFXfont* f) {
  if(f != nullptr) {
      this->oldFont = this->font;
      this->font = f;
  }
}

void Codingfield::UI::Label::SetRounded(bool r, int rad) {
    if(r) {
        radius = rad;
    }
    rounded = r;        
}

void Codingfield::UI::Label::SetBorder(bool b, int t) {
    withBorder = b;
    SetThickness(t);
}

void Codingfield::UI::Label::SetThickness(int t) {
    if(t > 0 && t <= 3)
        thickness = t;
}

void Codingfield::UI::Label::Draw() {    
  if(IsHidden()){
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
    int yText = position.y + (size.height/2);

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

    if(forceUpdate && withBorder) {
        M5.Lcd.drawRect(position.x, position.y, size.width, size.height, borderColor);
        if(thickness > 1)
            M5.Lcd.drawRect(position.x+1, position.y+1, size.width-2, size.height-2, borderColor);
        if(thickness == 3)
            M5.Lcd.drawRect(position.x+2, position.y+2, size.width-4, size.height-4, borderColor);
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

void Codingfield::UI::Label::Run() {
    Draw();    
}
