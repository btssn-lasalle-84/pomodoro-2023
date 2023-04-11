#ifndef POMODORO_H
#define POMODORO_H

#include <Arduino.h>
#include "BluetoothSerial.h"
#include <Wire.h>
#include <Preferences.h>
#include <string>
#include <time.h>
#include <sys/time.h>
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include <esp_spp_api.h>

/**
 * @def DEBUG
 * @brief A définir si on désire les message de debug sur le port série
 */
#define DEBUG_POMODORO
//#define DEBUG_RSSI
//#define DEBUG_BATTERIE
//#define DEBUG_HORLOGE
//#define DEBUG_AUTO

/**
 * @enum TypeTrame
 * @brief Les differents types de trame
 */
enum TypeTrame
{
    Inconnu = -1,
    POMODORO,
    USER,
    BELL,
    TASK,
    REST,
    STOP,
    RESET,
    WAIT,
    DARN,
    HEARTBEAT,
    STATE,
    ERROR,
    ACK,
    //"P",      "U",  "B",  "T",  "R",  "S",  "X",   "W",  "D",  "H",       "E",
    //"F",   "A"
    NB_TRAMES
};

/**
 * @enum TypePause
 * @brief Les differents types de pause
 */
enum TypePause
{
    Invalide = -1,
    COURTE,
    LONGUE,
    NB_TYPES_PAUSE
};

#define CHAMP_TYPE_PAUSE 1

/**
 * @enum EtatPomodoro
 * @brief Les differents états du Pomodoro
 */
enum EtatPomodoro
{
    EnAttente = -1,
    Termine   = 0,
    EnCours,
    EnCourtePause,
    FinCourtePause,
    EnLonguePause,
    FinLonguePause,
    Gele,
    Reprise
};

/**
 * @enum Etat
 * @brief Les differents états
 */
enum Etat
{
    Aucun = -1,
    OFF   = 0,
    ON    = 1
};

/**
 * @enum CouleurPomodoro
 * @brief Les differents états
 */
enum CouleurPomodoro
{
    Noir      = 0x0000, /*   0,   0,   0 */
    Navy      = 0x000F, /*   0,   0, 128 */
    VertClair = 0x03E0, /*   0, 128,   0 */
    CyanFonce = 0x03EF, /*   0, 128, 128 */
    Marron    = 0x7800, /* 128,   0,   0 */
    Violet    = 0x780F, /* 128,   0, 128 */
    Olive     = 0x7BE0, /* 128, 128,   0 */
    GrisClair = 0xC618, /* 192, 192, 192 */
    GrisFonce = 0x7BEF, /* 128, 128, 128 */
    Bleu      = 0x001F, /*   0,   0, 255 */
    Vert      = 0x07E0, /*   0, 255,   0 */
    Cyan      = 0x07FF, /*   0, 255, 255 */
    Rouge     = 0xF800, /* 255,   0,   0 */
    Magenta   = 0xF81F, /* 255,   0, 255 */
    Jaune     = 0xFFE0, /* 255, 255,   0 */
    Blanc     = 0xFFFF, /* 255, 255, 255 */
    Orange    = 0xFD20, /* 255, 165,   0 */
    VertJaune = 0xAFE5, /* 173, 255,  47 */
    Rose      = 0xF81F,
    NbCouleurs
};

/**
 * @enum Mode
 * @brief Les differents modes
 */
enum Mode
{
    Minuteur    = 0,
    Chronometre = 1
};

#define DUREE_POMODORO        25 // 1500 // 25 minutes
#define PAUSE_COURTE_POMODORO 5  // 300  // 5 minutes
#define PAUSE_LONGUE_POMODORO 20 // 1200 // 20 minutes
#define NB_POMODORI           4

/**
 * @struct Pomodoro
 * @brief Paramètres d'un pomodoro
 */
struct Pomodoro
{
    int          id       = 0;     //!< l'identifiant
    bool         actif    = false; //!< avec sonnette ou pas
    bool         sonnette = false; //!< avec sonnette ou pas
    EtatPomodoro etat;             //!< état du pomodoro
    int          duree;            //!< durée d'un pomodoro en secondes
    int          pauseCourte;      //!< durée de la pause courte en secondes
    int          pauseLongue;      //!< durée de la pause longue en secondes
    int          nbPomodori; //!< nombre de pomodori avant une pause longue
    bool         autoPomodoro = false; //!<
    bool         autoPause    = false; //!< passe automatiquement en pause
    Mode         mode;                 //!< minuteur ou chronomètre
};

/**
 * @def PERIPHERIQUE_BLUETOOTH
 * @brief Définit le nom du préiphérique Bluetooth
 */
#define PERIPHERIQUE_BLUETOOTH "pomodoro-1"
#define EN_TETE_TRAME          "#"
#define DELIMITEUR_CHAMP       "&"
#define DELIMITEURS_FIN        "\r\n"
#define DELIMITEUR_DATAS       '&'
#define DELIMITEUR_FIN         '\n'

#define ERREUR_TRAME_INCONNUE      0
#define ERREUR_TRAME_NON_SUPPORTEE 1
#define ERREUR_COMMANDE            2
#define ERREUR_PARAMETRE           3
#define ERREUR_CONFIGURATION       4

// #U&NOM&PRENOM\r\n
#define NB_PARAMETRES_UTILISATEUR 2
#define CHAMP_NOM_UTILISATEUR     1
#define CHAMP_PRENOM_UTILISATEUR  2
// #T&[NOM]\r\n
#define NB_PARAMETRES_TACHE 1
#define CHAMP_NOM_TACHE     1
#define LONGUEUR_MAX        28
// #B&ACTIVATION\r\n
#define NB_PARAMETRES_SONNETTE    1
#define CHAMP_ACTIVATION_SONNETTE 1
// #P&duree&pauseCourte&pauseLongue&nbPomodori&autoPomodoro&autoPause&mode\r\n
#define NB_PARAMETRES_POMODORO 7
#define CHAMP_DUREE            1
#define CHAMP_PAUSE_COURTE     2
#define CHAMP_PAUSE_LONGUE     3
#define CHAMP_NB_POMODORI      4
#define CHAMP_AUTO_POMODORO    5
#define CHAMP_AUTO_PAUSE       6
#define CHAMP_MODE             7

// Fonctions de service
bool           lireTrame(String& trame);
int            compterParametres(const String& trame);
TypeTrame      verifierTrame(String& trame);
TypePause      verifierTypePause(String& type);
String         getNomTrame(TypeTrame typeTrame);
void           envoyerTrameAlive();
void           envoyerTrameErreur(int code);
void           envoyerTrameAcquittement();
void           envoyerTrameEtat(int etat);
String         extraireChamp(String& trame, unsigned int numeroChamp);
void           setActivationPomodoro(bool etat);
void           enregistreEtatPomodoro(EtatPomodoro etat);
void           enregistreNbPomodori();
void           incrementerNbPomodori();
void           reinitialiserNbPomodori();
bool           gererModeAutomatique();
void           reinitialiserPomodoro();
void           setEtatPomodoro(int etat);
EtatPomodoro   getEtatPomodoro();
EtatPomodoro   getEtatPomodoroPrecedent();
bool           getChangementEtatPomodoro();
void           setChangementEtatPomodoro(bool etat);
void           setActivationSonnette(int etat);
bool           estEcheance(unsigned long intervalle);
bool           configrerTache(String& trame);
bool           configurerUtilisateur(String& trame);
bool           configurerPomodoro(String& trame);
bool           configurerSonnette(String& trame);
void           traiterBoutonPomodoro();
void IRAM_ATTR cliquerBoutonPomodoro();
void IRAM_ATTR cliquerBoutonPause();
void           controlerAffichageBouton(bool actif = true);
void           initialiserEcran();
void getRSSI(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param);
void getEtatBluetooth(esp_spp_cb_event_t event, esp_spp_cb_param_t* param);
void lireNiveauBluetooth();
bool estConnecte();
void initialiserBluetooth();
void initialiserPreferences();
void initialiserPomodoro();
void initialiserTimers();
int  getDuree();
int  getDureePauseCourte();
int  getDureePauseLongue();
void setDuree(int duree);
void setDureePauseCourte(int duree);
void setDureePauseLongue(int duree);
void demarrerTimerPomodoro(int duree);
void arreterTimerPomodoro();
void terminerTimerPomodoro(int id);
void demarrerTimerHorloge();
void arreterTimerHorloge();
void afficherHorloge();
void afficherAccueil();
void afficherInformationsUtilisateur(bool redraw = false);
void afficherTache(bool redraw = false);
void afficherNbPomodori(bool redraw = false);
void rafraichirEcran(uint32_t tempo = 300);

// Fonctions utilitaires
int  count(const String& str, const String& sub);
void nettoyerCaractere(std::string& chaine,
                       std::string  caractere,
                       char         remplacement);
void nettoyerChaines();

#endif
