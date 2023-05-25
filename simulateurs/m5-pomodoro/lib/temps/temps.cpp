/**
*
* @file lib/temps/temps.cpp
* @brief Définition de la classe Temps
* @author Thierry Vaira
* @version 1.2
*
* $LastChangedRevision: 71 $
* $LastChangedDate: 2021-04-27 16:10:27 +0200 (mar. 27 avril 2021) $
*/

#include "temps.h"

Temps::Temps() : valeur(0)
{
}

Temps::Temps(uint64_t secondes) : valeur(secondes)
{
}

Temps::Temps(int heure, int minute, int seconde) : valeur((heure*3600)+(minute*60)+seconde)
{
}

Temps::Temps(const Temps &t) : valeur(t.valeur)
{
}

Temps& Temps::operator=(const Temps &t)
{
    if(this != &t)
    {
        valeur = t.valeur;
    }

    return *this;
}

uint64_t Temps::getValeur() const
{
    return valeur;
}

int Temps::getSeconde() const
{
    return (valeur%60);
}

int Temps::getMinute() const
{
    return (valeur%3600)/60;
}

int Temps::getHeure() const
{
    return ((valeur  % 86400L) / 3600);
}

int Temps::getJour() const
{
    return (((valeur  / 86400L) + 4) % 7); // 0 = Dimanche
}

String Temps::getHHMMSS() const
{
  unsigned long rawTime = valeur;
  unsigned long hours = (rawTime % 86400L) / 3600;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  unsigned long minutes = (rawTime % 3600) / 60;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  unsigned long seconds = rawTime % 60;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return hoursStr + ":" + minuteStr + ":" + secondStr;
}

String Temps::getMMSS() const
{
  unsigned long rawTime = valeur;

  unsigned long minutes = (rawTime % 3600) / 60;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  unsigned long seconds = rawTime % 60;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return minuteStr + ":" + secondStr;
}

void Temps::reset()
{
    valeur = 0;
}

void Temps::setTemps(uint64_t secondes)
{
    valeur = secondes;
}

void Temps::setTemps(int heure, int minute, int seconde/*=0*/)
{
    valeur = (heure * 3600) + (minute * 60) + seconde;
}

int Temps::getSeconde(uint64_t timestamp)
{
    return (timestamp % 60);
}

int Temps::getMinute(uint64_t timestamp)
{
    return (timestamp % 3600) / 60;
}

int Temps::getHeure(uint64_t timestamp)
{
    return ((timestamp  % 86400L) / 3600);
}

int Temps::getJour(uint64_t timestamp)
{
    return (((timestamp / 86400L) + 4) % 7); // 0 = Dimanche
}

String Temps::getHHMMSS(uint64_t timestamp)
{
  unsigned long rawTime = timestamp;
  unsigned long hours = (rawTime % 86400L) / 3600;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  unsigned long minutes = (rawTime % 3600) / 60;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  unsigned long seconds = rawTime % 60;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return hoursStr + ":" + minuteStr + ":" + secondStr;
}

String Temps::getMMSS(uint64_t timestamp)
{
  unsigned long rawTime = timestamp;

  unsigned long minutes = (rawTime % 3600) / 60;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  unsigned long seconds = rawTime % 60;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return minuteStr + ":" + secondStr;
}

Temps& Temps::operator+=(const uint64_t &s)
{
    this->valeur += s;
    // ou :
    //*this = *this + s;

    return *this;
}

Temps& Temps::operator+=(const Temps &t)
{
    this->valeur += t.valeur;

    return *this;
}

Temps& Temps::operator-=(const uint64_t &s)
{
    this->valeur -= s;
    // ou :
    //*this = *this - s;

    return *this;
}

Temps& Temps::operator-=(const Temps &t)
{
    this->valeur -= t.valeur;

    return *this;
}

bool Temps::operator==(const Temps &t) const
{
    return (valeur == t.valeur);
}

bool Temps::operator!=(const Temps &t) const
{
    return !(*this == t);
}

Temps operator+(const Temps &t, const uint64_t &s)
{
    Temps temps(t.valeur + s);
    return temps;
}

Temps operator+(const uint64_t &s, const Temps &t)
{
    return (t + s);
}

Temps operator+(const Temps &l, const Temps &r)
{
    Temps temps(l.valeur + r.valeur);
    return temps;
}

Temps operator-(const Temps &t, const uint64_t &s)
{
    Temps temps(t.valeur - s);
    return temps;
}

Temps operator-(const uint64_t &s, const Temps &t)
{
    return (t - s);
}

Temps operator-(const Temps &l, const Temps &r)
{
    Temps temps(l.valeur - r.valeur);
    return temps;
}

Temps& Temps::operator++() 
{ 
    this->valeur++;
    return *this; 
} 

Temps Temps::operator++(int n) 
{ 
    Temps temps = *this; 
    ++(*this);
    return temps; 
}

Temps& Temps::operator--() 
{ 
    this->valeur--;
    return *this; 
} 

Temps Temps::operator--(int n) 
{ 
    Temps temps = *this; 
    --(*this);
    return temps; 
}
