#ifndef TEMPS_H
#define TEMPS_H

/**
*
* @file lib/temps/temps.h
* @brief Déclaration de la classe Temps
* @author Thierry Vaira
* @version 1.2
*/

#include <Arduino.h>

class Temps
{
    private:
        uint64_t valeur;

    public:
        Temps();
        Temps(uint64_t secondes);
        Temps(int heure, int minute, int seconde=0);
        Temps(const Temps &t);
        Temps& operator =(const Temps &t);
        uint64_t getValeur() const;
        Temps& operator+=(const uint64_t &s);
        Temps& operator+=(const Temps &t);
        Temps& operator-=(const uint64_t &s);
        Temps& operator-=(const Temps &t);
        bool operator==(const Temps &t) const;
        bool operator!=(const Temps &t) const;
        Temps& operator++(); // prefixe
        Temps operator++(int n); // postfixe
        Temps& operator--();// prefixe
        Temps operator--(int n); // postfixe
        friend Temps operator +(const Temps &t, const uint64_t &s);
        friend Temps operator +(const uint64_t &s, const Temps &t);
        friend Temps operator +(const Temps &l, const Temps &r);
        friend Temps operator -(const Temps &t, const uint64_t &s);
        friend Temps operator -(const uint64_t &s, const Temps &t);
        friend Temps operator -(const Temps &l, const Temps &r);
        int getSeconde() const;
        int getMinute() const;
        int getHeure() const;
        int getJour() const;
        String getHHMMSS() const;
        String getMMSS() const;
        void reset();
        void setTemps(uint64_t secondes);
        void setTemps(int heure, int minute, int seconde=0);
        static int getSeconde(uint64_t timestamp);
        static int getMinute(uint64_t timestamp);
        static int getHeure(uint64_t timestamp);
        static int getJour(uint64_t timestamp);
        static String getHHMMSS(uint64_t timestamp);
        static String getMMSS(uint64_t timestamp);
};

#endif //TEMPS_H
