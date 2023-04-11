#include "Image.h"

using namespace Codingfield::UI;

/* Please, please, M5STack, use namespaces around your classes! */

Codingfield::UI::Image::Image(Widget* parent) : Widget(parent), parent(parent) {

}

Codingfield::UI::Image::Image(Widget* parent, Point position, Size size) : Widget(parent, position, size), parent(parent) {

}

Color Codingfield::UI::Image::GetBackgroundColor() {
    return this->backgroundColor;
}

void Codingfield::UI::Image::SetBackgroundColor(Color c) {
  backgroundColorUpdated = true;
  this->backgroundColor = c;
}

void Codingfield::UI::Image::SetBorderColor(Color c) {
  this->borderColor = c;
}

void Codingfield::UI::Image::SetBorder(bool b, int t) {
    withBorder = b;
    SetThickness(t);
}

void Codingfield::UI::Image::SetThickness(int t) {
    if(t > 0 && t <= 3)
        thickness = t;
}

void Codingfield::UI::Image::SetImage(const char *image) {
    if(image) {
        this->image = image;
        imageUpdated = true;
    }
}

void Codingfield::UI::Image::SetImage(fs::FS *fs, std::string path) {
    if(fs && !path.empty()) {
        this->fs = fs;
        this->path = path;
        imageUpdated = true;
    }
}

void Codingfield::UI::Image::SetScale(double scale) {
    if(this->scale != scale) {
        this->scale = scale;
        imageUpdated = true;
    }
}

void Codingfield::UI::Image::Draw() {    
  if(IsHidden()){
    if(!cleared) {
        M5.Lcd.fillRect(position.x, position.y, size.width, size.height, parent->GetBackgroundColor());
        if(withBorder) {            
                M5.Lcd.drawRect(position.x, position.y, size.width, size.height, parent->GetBackgroundColor());
                if(thickness > 1)
                    M5.Lcd.drawRect(position.x+1, position.y+1, size.width-2, size.height-2, parent->GetBackgroundColor());
                if(thickness == 3)
                    M5.Lcd.drawRect(position.x+2, position.y+2, size.width-4, size.height-4, parent->GetBackgroundColor());
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

    if(backgroundColorUpdated || forceUpdate) {      
      M5.Lcd.fillRect(position.x, position.y, size.width, size.height, backgroundColor);
      backgroundColorUpdated = false;
      forceUpdate = true;
      cleared = false;
    }

    if(imageUpdated || forceUpdate) {
        if (image) {
            M5.Lcd.drawJpg((uint8_t *)image, (sizeof(image) / sizeof(image[0])), position.x, position.y, size.width, size.height);
        }
        if (fs) {
            std::size_t found = path.find(".png");
            if (found != std::string::npos) {
                /*
                    void drawPngFile(fs::FS &fs, const char *path, uint16_t x = 0, uint16_t y = 0,
                                        uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                                        uint16_t offX = 0, uint16_t offY = 0,
                                        double scale = 1.0, uint8_t alphaThreshold = 127);
                */
                M5.Lcd.drawPngFile(*fs, path.c_str(), position.x, position.y, size.width, size.height, 0, 0, scale);
            }
            else {
                found = path.find(".jpg");
                if (found != std::string::npos) {
                    /*
                        void drawJpgFile(fs::FS &fs, const char *path, uint16_t x = 0, uint16_t y = 0,
                                            uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                                            uint16_t offX = 0, uint16_t offY = 0,
                                            jpeg_div_t scale = JPEG_DIV_NONE);
                    */
                    M5.Lcd.drawJpgFile(*fs, path.c_str(), position.x, position.y, size.width, size.height);
                }
                else {
                    found = path.find(".bmp");
                    if (found != std::string::npos) {
                        M5.Lcd.drawBmpFile(*fs, path.c_str(), position.x, position.y);
                    }
                }
            }
        }
        imageUpdated = false;
    }

    if(forceUpdate && withBorder) {
        M5.Lcd.drawRect(position.x, position.y, size.width, size.height, borderColor);
        if(thickness > 1)
            M5.Lcd.drawRect(position.x+1, position.y+1, size.width-2, size.height-2, borderColor);
        if(thickness == 3)
            M5.Lcd.drawRect(position.x+2, position.y+2, size.width-4, size.height-4, borderColor);
    }    
  }
  isInvalidated = false;
}

void Codingfield::UI::Image::Run() {
    Draw();    
}
