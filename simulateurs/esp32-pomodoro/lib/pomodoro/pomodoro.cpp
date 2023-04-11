#include "pomodoro.h"
#include "temps.h"
#include <afficheur.h>
#include <Ticker.h>

// Bluetooth
static BluetoothSerial SerialBT; //!< objet pour la communication Bluetooth
static bool            connecte = false; //!< état de la connexion Bluetooth
const String           nomsTrame[TypeTrame::NB_TRAMES] = {
              "P", "U", "B", "T", "R", "S", "X", "W", "D", "H", "E", "F", "A"
}; //!< identifiant des trames dans le protocole
static String entete     = String(EN_TETE_TRAME);      // caractère séparateur
static String separateur = String(DELIMITEUR_CHAMP);   // caractère séparateur
static String delimiteurFin = String(DELIMITEURS_FIN); // fin de trame
const String  nomsTypePause[TypePause::NB_TYPES_PAUSE] = {
     "C",
     "L"
};                               //!< nom des types de pause
static int     rssi = 0;         //!< le niveau RSSI du Bluetooth
static String  adresseBluetooth; //!< l'adresse MAC Bluetooth du Pomodoro
static uint8_t adresseDistante[ESP_BD_ADDR_LEN] = {
    0, 0, 0, 0, 0, 0
}; //!< l'adresse MAC du Bluetooth connecté
static uint8_t adressePomodoro[ESP_BD_ADDR_LEN] = {
    0, 0, 0, 0, 0, 0
}; //!< l'adresse MAC Bluetooth du Pomodoro

// Pomodoro
static Pomodoro     pomodoro;
static int          nbPomodori = 0;
static EtatPomodoro etatPrecedent;
static bool         changementEtatPomodoro =
  false; //!< indique un changement d'état du Pomodoro
Temps               tempsHorloge;
static EtatPomodoro etatBouton = EnAttente;

// Preferences
static Preferences preferences;       //!< objet pour le stockage interne
static std::string nomUtilisateur;    //!< le nom de l'utilisateur
static std::string prenomUtilisateur; //!< le prenom de l'utilisateur
static std::string nomTache;          //!< le nom de la tâche

// Timers
Ticker timerPomodoro; //!< le timer pour gérer un pomodoro
Ticker timerHorloge;  //!< le timer pour gérer l'affichage du temps

// GUI
extern Afficheur afficheur;
static String    labelUtilisateur;
static String    labelEtatPomodoro;
static String    labelNomTache;
static String    labelNbPomodori;
static bool      boutonPomodoroValide = true;
static bool      antiRebond           = false; //!< anti-rebond

/**
 * @brief Lit une trame via le Bluetooth
 *
 * @fn lireTrame(String &trame)
 * @param trame la trame reçue
 * @return bool true si une trame a été lue, sinon false
 */
bool lireTrame(String& trame)
{
    if(SerialBT.available())
    {
        trame.clear();
        trame = SerialBT.readStringUntil(DELIMITEUR_FIN);
        trame.concat(DELIMITEUR_FIN); // remet le DELIMITEUR_FIN
#ifdef DEBUG_POMODORO
        Serial.print("< ");
        if(trame.indexOf('\r') != -1)
            trame.remove(trame.indexOf('\r'), 2);
        Serial.println(trame);
        trame.concat(delimiteurFin);
#endif
        return true;
    }

    return false;
}

/**
 * @brief Compte le nombre de paramètres (champ délimité) d'une trame
 *
 * @fn compterParametres(const String &trame)
 * @param trame
 * @return int le nombre de paramètres contenu dans la trame
 */
int compterParametres(const String& trame)
{
    int n = 0;
    for(int i = 0; i < trame.length(); i++)
    {
        if(trame[i] == DELIMITEUR_DATAS)
            ++n;
    }
    return n;
}

/**
 * @brief Vérifie si la trame reçue est valide et retorune le type de la trame
 *
 * @fn verifierTrame(String &trame)
 * @param trame
 * @return TypeTrame le type de la trame
 */
TypeTrame verifierTrame(String& trame)
{
    String format;

    for(int i = 0; i < TypeTrame::NB_TRAMES; i++)
    {
        // Format : ${TYPE}\r\n
        format = entete + nomsTrame[i];
#ifdef DEBUG_POMODORO_VERIFICATION
        Serial.print("Verification trame : ");
        Serial.print(format);
#endif
        if(trame.startsWith(format))
        {
            return (TypeTrame)i;
        }
        else
        {
#ifdef DEBUG_POMODORO_VERIFICATION
            Serial.println("");
#endif
        }
    }
#ifdef DEBUG_POMODORO_VERIFICATION
    Serial.println("Type de trame : inconnu");
#endif
    return Inconnu;
}

TypePause verifierTypePause(String& type)
{
    String typePause;

    for(int i = 0; i < TypePause::NB_TYPES_PAUSE; i++)
    {
        // Format : $R;{TYPE}\r\n
        typePause = nomsTypePause[i];
#ifdef DEBUG_POMODORO_VERIFICATION
        Serial.print("Verification type : ");
        Serial.print(typePause);
#endif
        if(type == typePause)
        {
            return (TypePause)i;
        }
        else
        {
#ifdef DEBUG_POMODORO_VERIFICATION
            Serial.println("");
#endif
        }
    }
#ifdef DEBUG_POMODORO_VERIFICATION
    Serial.println("Type inconnu");
#endif
    return Invalide;
}

String getNomTrame(TypeTrame typeTrame)
{
    return nomsTrame[typeTrame];
}

/**
 * @brief Envoie une trame Alive via le Bluetooth
 *
 * @fn envoyerTrameAlive()
 */
void envoyerTrameAlive()
{
    char trameEnvoi[64];

    // Trame envoyée : #H\r\n
    sprintf((char*)trameEnvoi,
            "%s%s\r\n",
            entete.c_str(),
            nomsTrame[TypeTrame::HEARTBEAT].c_str());
    SerialBT.write((uint8_t*)trameEnvoi, strlen((char*)trameEnvoi));
#ifdef DEBUG_POMODORO
    Serial.println(String(trameEnvoi));
#endif
}

/**
 * @brief Envoie une trame d'erreur via le Bluetooth
 *
 */
void envoyerTrameErreur(int code)
{
    char trameEnvoi[64];

    // Format : #F&{CODE}\r\n
    sprintf((char*)trameEnvoi,
            "%s%s%s%d\r\n",
            entete.c_str(),
            nomsTrame[TypeTrame::ERROR].c_str(),
            DELIMITEUR_CHAMP,
            code);

    SerialBT.write((uint8_t*)trameEnvoi, strlen((char*)trameEnvoi));
#ifdef DEBUG_POMODORO
    String trame = String(trameEnvoi);
    trame.remove(trame.indexOf("\r"), 2);
    Serial.print("> ");
    Serial.println(trame);
#endif
}

/**
 * @brief Envoie une trame d'acquittement via le Bluetooth
 *
 */
void envoyerTrameAcquittement()
{
    char trameEnvoi[64];

    // Format : #A\r\n
    sprintf((char*)trameEnvoi,
            "%s%s\r\n",
            entete.c_str(),
            nomsTrame[TypeTrame::ACK].c_str());

    SerialBT.write((uint8_t*)trameEnvoi, strlen((char*)trameEnvoi));
#ifdef DEBUG_POMODORO
    String trame = String(trameEnvoi);
    trame.remove(trame.indexOf("\r"), 2);
    Serial.print("> ");
    Serial.println(trame);
#endif
}

/**
 * @brief Envoie une trame d'état via le Bluetooth
 *
 */
void envoyerTrameEtat(int etat)
{
    char trameEnvoi[64];

    // Format : #E&{ETAT}\r\n
    sprintf((char*)trameEnvoi,
            "%s%s%s%d\r\n",
            entete.c_str(),
            nomsTrame[TypeTrame::STATE].c_str(),
            DELIMITEUR_CHAMP,
            etat);

    SerialBT.write((uint8_t*)trameEnvoi, strlen((char*)trameEnvoi));
#ifdef DEBUG_POMODORO
    String trame = String(trameEnvoi);
    trame.remove(trame.indexOf("\r"), 2);
    Serial.print("> ");
    Serial.println(trame);
#endif
}

/**
 * @brief Extrait un champ d'une trame Pomodoro
 *
 * @fn extraireChamp(String &trame, unsigned int numeroChamp)
 * @param trame
 * @param numeroChamp
 * @return String
 */
String extraireChamp(String& trame, unsigned int numeroChamp)
{
    String       champ;
    unsigned int compteurCaractere  = 0;
    unsigned int compteurDelimiteur = 0;
    char         fin                = '\n';

    if(delimiteurFin.length() > 0)
        fin = delimiteurFin[0];

    for(unsigned int i = 0; i < trame.length(); i++)
    {
        if(trame[i] == separateur[0] || trame[i] == fin)
        {
            compteurDelimiteur++;
        }
        else if(compteurDelimiteur == numeroChamp)
        {
            champ += trame[i];
            compteurCaractere++;
        }
    }

    return champ;
}

void setActivationPomodoro(bool etat)
{
    pomodoro.actif = etat;
    preferences.putInt("actif", (int)pomodoro.actif);
}

void enregistreEtatPomodoro(EtatPomodoro etat)
{
    etatPrecedent          = pomodoro.etat;
    pomodoro.etat          = etat;
    changementEtatPomodoro = true;
    preferences.putInt("etat", (int)pomodoro.etat);
}

void enregistreNbPomodori()
{
    preferences.putInt("nb", nbPomodori);
}

void incrementerNbPomodori()
{
    ++nbPomodori;
    enregistreNbPomodori();
    // if(nbPomodori <= pomodoro.nbPomodori)
    afficherNbPomodori();
    /*if(nbPomodori > pomodoro.nbPomodori)
    {
      nbPomodori = 0;
      enregistreNbPomodori();
      afficherNbPomodori();
    }*/
}

void reinitialiserNbPomodori()
{
    nbPomodori = 0;
    enregistreNbPomodori();
    afficherNbPomodori();
}

bool gererModeAutomatique()
{
#ifdef DEBUG_POMODORO_AUTO
    Serial.print("autoPomodoro : ");
    Serial.println(pomodoro.autoPomodoro);
    Serial.print("autoPause : ");
    Serial.println(pomodoro.autoPause);
    Serial.print("nbPomodori : ");
    Serial.println(nbPomodori);
    Serial.print("pomodoro.etat : ");
    Serial.println(pomodoro.etat);
    Serial.print("etatPrecedent : ");
    Serial.println(etatPrecedent);
#endif
    // passage automatique en pause ?
    if((etatPrecedent == EnCours && pomodoro.etat == Termine) &&
       pomodoro.autoPause && nbPomodori < pomodoro.nbPomodori)
    {
// passage automatique en pause courte
#ifdef DEBUG_POMODORO
        Serial.println("Passage automatique en pause courte");
#endif
        setEtatPomodoro(EnCourtePause);
        return true;
    }
    else if((etatPrecedent == EnCours && pomodoro.etat == Termine) &&
            pomodoro.autoPause && nbPomodori == pomodoro.nbPomodori)
    {
// passage automatique en pause longue
#ifdef DEBUG_POMODORO
        Serial.println("Passage automatique en pause longue");
#endif
        setEtatPomodoro(EnLonguePause);
        return true;
    }
    // passage automatique en pomodoro ?
    if(pomodoro.etat == FinCourtePause && pomodoro.autoPomodoro)
    {
#ifdef DEBUG_POMODORO
        Serial.println("Passage automatique en tâche");
#endif
        setEtatPomodoro(EnCours);
        return true;
    }
    // repasse en attente ?
    if(pomodoro.etat == Termine || pomodoro.etat == FinCourtePause ||
       pomodoro.etat == FinLonguePause)
    {
#ifdef DEBUG_POMODORO
        Serial.println("Passage en attente");
#endif
        setEtatPomodoro(EnAttente);
        return false;
    }

    return false;
}

void reinitialiserPomodoro()
{
    reinitialiserNbPomodori();
    setEtatPomodoro(EnAttente);
#ifdef DEBUG_POMODORO
    Serial.println("Tâche ou pause annulée");
#endif
}

/**
 * @brief Met à jour l'état du Pomodoro
 *
 * @fn setEtatPomodoro(int etat)
 * @param etat
 */
void setEtatPomodoro(int etat)
{
    if(pomodoro.etat != (EtatPomodoro)etat)
    {
        enregistreEtatPomodoro((EtatPomodoro)etat);
        envoyerTrameEtat((int)getEtatPomodoro());
        switch(pomodoro.etat)
        {
            case EnAttente:
                arreterTimerPomodoro();
                arreterTimerHorloge();
                setActivationPomodoro(false);
                tempsHorloge.reset();
                labelEtatPomodoro = String("00:00") + String(" En attente");
                // boutonPomodoro->SetText("START");
                break;
            case Termine:
                arreterTimerHorloge();
                setActivationPomodoro(false);
                tempsHorloge.reset();
                labelEtatPomodoro = "00:00" + String(" Pomodoro fini");
                // boutonPomodoro->SetText("START");
                if(pomodoro.sonnette)
                {
                    // M5.Speaker.tone(432, 500);
                }
                incrementerNbPomodori();
                break;
            case EnCours:
                if(!pomodoro.actif)
                {
                    tempsHorloge.setTemps(pomodoro.duree);
                    labelEtatPomodoro =
                      tempsHorloge.getMMSS() + String(" Pomodoro");
                    arreterTimerPomodoro();
                    demarrerTimerPomodoro(pomodoro.duree);
                    setActivationPomodoro(true);
                }
                demarrerTimerHorloge();
                // boutonPomodoro->SetText("STOP");
                break;
            case EnCourtePause:
                if(!pomodoro.actif)
                {
                    tempsHorloge.setTemps(pomodoro.pauseCourte);
                    labelEtatPomodoro =
                      tempsHorloge.getMMSS() + String(" Pause courte");
                    arreterTimerPomodoro();
                    demarrerTimerPomodoro(pomodoro.pauseCourte);
                    setActivationPomodoro(true);
                }
                demarrerTimerHorloge();
                // boutonPomodoro->SetText("STOP");
                break;
            case FinCourtePause:
                arreterTimerPomodoro();
                arreterTimerHorloge();
                setActivationPomodoro(false);
                tempsHorloge.reset();
                labelEtatPomodoro = "00:00" + String(" Pause finie");
                // boutonPomodoro->SetText("START");
                if(pomodoro.sonnette)
                {
                    // M5.Speaker.tone(1230, 500);
                }
                break;
            case EnLonguePause:
                if(!pomodoro.actif)
                {
                    tempsHorloge.setTemps(pomodoro.pauseLongue);
                    labelEtatPomodoro =
                      tempsHorloge.getMMSS() + String(" Pause longue");
                    arreterTimerPomodoro();
                    demarrerTimerPomodoro(pomodoro.pauseLongue);
                    setActivationPomodoro(true);
                }
                demarrerTimerHorloge();
                // boutonPomodoro->SetText("STOP");
                break;
            case FinLonguePause:
                arreterTimerPomodoro();
                arreterTimerHorloge();
                setActivationPomodoro(false);
                tempsHorloge.reset();
                labelEtatPomodoro = "00:00" + String(" Pause finie");
                // boutonPomodoro->SetText("START");
                if(pomodoro.sonnette)
                {
                    // M5.Speaker.tone(1230, 500);
                }
                reinitialiserNbPomodori();
                break;
            case Gele:
                arreterTimerPomodoro();
                arreterTimerHorloge();
                setActivationPomodoro(false);
                // boutonPomodoro->SetText("STOP");
                break;
            case Reprise:
                // demarrerTimerHorloge();
                //  boutonPomodoro->SetText("STOP");
                break;
            default:
                break;
        }
        controlerAffichageBouton();
    }
}

EtatPomodoro getEtatPomodoro()
{
    return pomodoro.etat;
}

EtatPomodoro getEtatPomodoroPrecedent()
{
    return etatPrecedent;
}

bool getChangementEtatPomodoro()
{
    return changementEtatPomodoro;
}

void setChangementEtatPomodoro(bool etat)
{
    changementEtatPomodoro = etat;
}

/**
 * @brief Met à jour l'état d'activation de la sonnette
 *
 * @fn setActivationSonnette(int etat)
 * @param etat (ON ou OFF)
 */
void setActivationSonnette(int etat)
{
    if(pomodoro.sonnette != (Etat)etat)
    {
        pomodoro.sonnette = (Etat)etat;
        preferences.putInt("sonnette", (int)pomodoro.sonnette);
    }
}

/**
 * @brief Retourne true si l'échéance de la période fixée a été atteinte
 *
 * @fn estEcheance(unsigned long intervalle)
 *
 * @param intervalle unsigned long l'intervalle de temps en millisecondes entre
 * deux échéances
 *
 * @return bool vrai si l'échéance de la période a été atteinte
 */
bool estEcheance(unsigned long intervalle)
{
    static unsigned long tempsPrecedent = millis();
    unsigned long        temps          = millis();
    if(temps - tempsPrecedent >= intervalle)
    {
        tempsPrecedent = temps;
        return true;
    }
    return false;
}

bool configrerTache(String& trame)
{
    // #T&[NOM]\r\n
    int nbParametres = compterParametres(trame);
#ifdef DEBUG_POMODORO
    Serial.print("nbParametres : ");
    Serial.println(nbParametres);
#endif
    if(nbParametres == NB_PARAMETRES_TACHE)
    {
        String champ = extraireChamp(trame, CHAMP_NOM_TACHE);
#ifdef DEBUG_POMODORO
        Serial.print("champ : ");
        Serial.println(champ.c_str());
#endif
        size_t retour = preferences.putString("nomTache", champ.c_str());
        if(retour == champ.length())
        {
            nomTache = champ.c_str();
#ifdef DEBUG_POMODORO
            Serial.print("nomTache : ");
            Serial.println(nomTache.c_str());
#endif
            afficherTache(true);
            return true;
        }
    }
    return false;
}

bool configurerUtilisateur(String& trame)
{
    // #U&NOM&PRENOM\r\n
    int nbParametres = compterParametres(trame);
    if(nbParametres == NB_PARAMETRES_UTILISATEUR)
    {
        String champ  = extraireChamp(trame, CHAMP_NOM_UTILISATEUR);
        size_t retour = preferences.putString("nom", champ.c_str());
        if(retour == champ.length())
            nomUtilisateur = champ.c_str();
#ifdef DEBUG_POMODORO
        Serial.print("nomUtilisateur : ");
        Serial.println(nomUtilisateur.c_str());
#endif
        champ  = extraireChamp(trame, CHAMP_PRENOM_UTILISATEUR);
        retour = preferences.putString("prenom", champ.c_str());
        if(retour == champ.length())
            prenomUtilisateur = champ.c_str();
#ifdef DEBUG_POMODORO
        Serial.print("prenomUtilisateur : ");
        Serial.println(prenomUtilisateur.c_str());
#endif
        afficherInformationsUtilisateur(true);
        return true;
    }
    return false;
}

bool configurerPomodoro(String& trame)
{
    // #P&duree&pauseCourte&pauseLongue&nbPomodori&autoPomodoro&autoPause&mode\r\n
    int nbParametres = compterParametres(trame);
    if(nbParametres == NB_PARAMETRES_POMODORO)
    {
        String champ  = extraireChamp(trame, CHAMP_DUREE);
        size_t retour = preferences.putInt("duree", champ.toInt());
        if(retour != 0)
        {
            pomodoro.duree = champ.toInt();
        }
        champ  = extraireChamp(trame, CHAMP_PAUSE_COURTE);
        retour = preferences.putInt("pauseCourte", champ.toInt());
        if(retour != 0)
        {
            pomodoro.pauseCourte = champ.toInt();
        }
        champ  = extraireChamp(trame, CHAMP_PAUSE_LONGUE);
        retour = preferences.putInt("pauseLongue", champ.toInt());
        if(retour != 0)
        {
            pomodoro.pauseLongue = champ.toInt();
        }
        champ  = extraireChamp(trame, CHAMP_NB_POMODORI);
        retour = preferences.putInt("nbPomodori", champ.toInt());
        if(retour != 0)
        {
            pomodoro.nbPomodori = champ.toInt();
        }
        champ  = extraireChamp(trame, CHAMP_AUTO_POMODORO);
        retour = preferences.putInt("autoPomodoro", champ.toInt());
        if(retour != 0)
        {
            pomodoro.autoPomodoro = champ.toInt();
        }
        champ  = extraireChamp(trame, CHAMP_AUTO_PAUSE);
        retour = preferences.putInt("autoPause", champ.toInt());
        if(retour != 0)
        {
            pomodoro.autoPause = champ.toInt();
        }
        champ  = extraireChamp(trame, CHAMP_AUTO_PAUSE);
        retour = preferences.putInt("mode", (Mode)champ.toInt());
        if(retour != 0)
        {
            pomodoro.mode = (Mode)champ.toInt();
        }
        tempsHorloge.setTemps(pomodoro.duree);
        labelEtatPomodoro = tempsHorloge.getMMSS();
        controlerAffichageBouton();
        return true;
    }
    return false;
}

bool configurerSonnette(String& trame)
{
    // #B&ACTIVATION\r\n
    int nbParametres = compterParametres(trame);
    if(nbParametres == NB_PARAMETRES_SONNETTE)
    {
        String champ  = extraireChamp(trame, CHAMP_ACTIVATION_SONNETTE);
        size_t retour = preferences.putInt("sonnette", champ.toInt());
        if(retour != 0)
        {
            pomodoro.sonnette = champ.toInt();
            setActivationSonnette(pomodoro.sonnette);
        }
#ifdef DEBUG_POMODORO
        Serial.print("sonnette : ");
        Serial.println(pomodoro.sonnette);
#endif
        return true;
    }
    return false;
}

void traiterBoutonPomodoro()
{
    switch(etatBouton)
    {
        case EnCours:
            setEtatPomodoro(etatBouton);
            break;
        case Termine:
            setEtatPomodoro(etatBouton);
            gererModeAutomatique();
            break;
        case FinCourtePause:
            setEtatPomodoro(etatBouton);
            gererModeAutomatique();
            break;
        case FinLonguePause:
            setEtatPomodoro(etatBouton);
            gererModeAutomatique();
            break;
        case Gele:
            break;
        default:
            break;
    }
    etatBouton = EnAttente;
}

/**
 * @brief Fonction de callback pour le clic sur le bouton
 *
 * @fn cliquerBoutonPomodoro()
 *
 */
void IRAM_ATTR cliquerBoutonPomodoro()
{
    if(antiRebond || !connecte)
        return;

    switch(pomodoro.etat)
    {
        case EnAttente:
            etatBouton = EnCours;
            break;
        case EnCours:
            etatBouton = Termine;
            break;
        case EnCourtePause:
            etatBouton = FinCourtePause;
            break;
        case EnLonguePause:
            etatBouton = FinLonguePause;
            break;
        case Gele:
            etatBouton = Termine;
            break;
        default:
            break;
    }
    /*
    switch(pomodoro.etat)
    {
    case EnAttente:
      setEtatPomodoro(EnCours);
      break;
    case EnCours:
      setEtatPomodoro(Termine);
      gererModeAutomatique();
      //setEtatPomodoro(EnAttente);
      break;
    case EnCourtePause:
      setEtatPomodoro(FinCourtePause);
      gererModeAutomatique();
      //setEtatPomodoro(EnAttente);
      break;
    case EnLonguePause:
      setEtatPomodoro(FinLonguePause);
      gererModeAutomatique();
      //setEtatPomodoro(EnAttente);
      break;
    case Gele:
      setEtatPomodoro(Termine);
      gererModeAutomatique();
      //setEtatPomodoro(EnAttente);
      break;
    default:
      break;
    }
    */
    antiRebond = true;
}

/**
 * @brief Fonction de callback pour le clic sur le bouton
 *
 * @fn cliquerBoutonPause()
 *
 */
void IRAM_ATTR cliquerBoutonPause()
{
    if(antiRebond || !connecte)
        return;
#ifdef DEBUG_POMODORO
    Serial.print("Bouton PAUSE -> pomodoro.etat : ");
    Serial.println(pomodoro.etat);
#endif
    /*
    switch(pomodoro.etat)
    {
    case EnCours:
      setEtatPomodoro(Gele);
      break;
    case EnCourtePause:
      setEtatPomodoro(Gele);
      break;
    case EnLonguePause:
      setEtatPomodoro(Gele);
      break;
    case Gele:
      setEtatPomodoro(etatPrecedent);
      break;
    default:
      break;
    }
    */
    antiRebond = true;
}

void controlerAffichageBouton(bool actif /*=true*/)
{
    if(actif)
    {
        boutonPomodoroValide = true;
    }
    else
    {
        boutonPomodoroValide = false;
    }
}

/**
 * @brief Initialise les ressources pour l'écran
 *
 * @fn initialiserEcran()
 */
void initialiserEcran()
{
    afficheur.initialiser();

    labelUtilisateur  = " ";
    labelEtatPomodoro = " ";
    labelNomTache     = " ";
    labelNbPomodori   = " ";
}

/**
 * @brief Lit le RSSI de la connexion sans fil
 *
 */
void getRSSI(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param)
{
    if(event == ESP_BT_GAP_READ_RSSI_DELTA_EVT)
    {
        rssi = param->read_rssi_delta.rssi_delta;
#ifdef DEBUG_POMODORO_RSSI
        if(rssi != 0)
        {
            char adresse[32];
            sprintf(adresse,
                    "%02X:%02X:%02X:%02X:%02X:%02X",
                    (uint8_t)adresseDistante[0],
                    (uint8_t)adresseDistante[1],
                    (uint8_t)adresseDistante[2],
                    (uint8_t)adresseDistante[3],
                    (uint8_t)adresseDistante[4],
                    (uint8_t)adresseDistante[5]);
            Serial.print("mac = ");
            Serial.print(adresse);
            Serial.print(" -> rssi = ");
            Serial.println(rssi);
        }
#endif
    }
}

/**
 * @brief Lit l'état de la connexion sans fil
 *
 */
void getEtatBluetooth(esp_spp_cb_event_t event, esp_spp_cb_param_t* param)
{
    // Serveur : connexion ?
    if(event == ESP_SPP_SRV_OPEN_EVT)
    {
        connecte = true;
        controlerAffichageBouton();
        memcpy(adresseDistante, param->srv_open.rem_bda, ESP_BD_ADDR_LEN);
        char adresse[32];
        sprintf(adresse,
                "%02X:%02X:%02X:%02X:%02X:%02X",
                (uint8_t)adresseDistante[0],
                (uint8_t)adresseDistante[1],
                (uint8_t)adresseDistante[2],
                (uint8_t)adresseDistante[3],
                (uint8_t)adresseDistante[4],
                (uint8_t)adresseDistante[5]);
#ifdef DEBUG_POMODORO
        Serial.print("mac = ");
        Serial.println(adresse);
#endif
    }
    else if(event == ESP_SPP_CLOSE_EVT) // déconnexion ?
    {
        connecte = false;
        memset(adresseDistante, 0, ESP_BD_ADDR_LEN);
        // topBar->SetStatusRight(HeaderBar::WirelessStatuses::NoSignal);
        // boutonPomodoro->Hide();
    }
}

/**
 * @brief Lit et affiche le RSSI de la connexion sans fil Bluetooth
 *
 */
void lireNiveauBluetooth()
{
    if(connecte)
    {
        esp_bt_gap_read_rssi_delta(adresseDistante);
        if(rssi >= ESP_BT_GAP_RSSI_HIGH_THRLD)
        {
            // topBar->SetStatusRight(HeaderBar::WirelessStatuses::FullSignal);
        }
        else if(rssi >= (ESP_BT_GAP_RSSI_HIGH_THRLD - 10))
        {
            // topBar->SetStatusRight(HeaderBar::WirelessStatuses::MediumSignal);
        }
        else if(rssi >= ESP_BT_GAP_RSSI_LOW_THRLD)
        {
            // topBar->SetStatusRight(HeaderBar::WirelessStatuses::WeakSignal);
        }
        else
        {
            // topBar->SetStatusRight(HeaderBar::WirelessStatuses::NoSignal);
        }
    }
}

bool estConnecte()
{
    return connecte;
}

/**
 * @brief Initialise le Bluetooth
 *
 */
void initialiserBluetooth()
{
    SerialBT.begin(PERIPHERIQUE_BLUETOOTH);
    esp_bt_gap_register_callback(getRSSI);
    SerialBT.register_callback(getEtatBluetooth);
    const uint8_t* adresseLocale = esp_bt_dev_get_address();
    memcpy(adressePomodoro, adresseLocale, ESP_BD_ADDR_LEN);
    char adresse[32];
    sprintf(adresse,
            "%02X:%02X:%02X:%02X:%02X:%02X",
            (uint8_t)adressePomodoro[0],
            (uint8_t)adressePomodoro[1],
            (uint8_t)adressePomodoro[2],
            (uint8_t)adressePomodoro[3],
            (uint8_t)adressePomodoro[4],
            (uint8_t)adressePomodoro[5]);
    adresseBluetooth = String(adresse);
#ifdef DEBUG_POMODORO
    Serial.print("mac bluetooth = ");
    Serial.println(adresse);
#endif
}

/**
 * @brief Initialise les préférences stockées
 *
 */
void initialiserPreferences()
{
    // initialise le stockage interne
    preferences.begin("eeprom", false); // false pour r/w

    // récupère les données stockées à partir des clés
    String valeur;
    valeur = preferences.getString("nom", String());
    if(!valeur.isEmpty())
        nomUtilisateur = valeur.c_str();
    valeur = preferences.getString("prenom", String());
    if(!valeur.isEmpty())
        prenomUtilisateur = valeur.c_str();
    valeur = preferences.getString("nomTache", String());
    if(!valeur.isEmpty())
        nomTache = valeur.c_str();

    // initialise la partie Pomodoro
    initialiserPomodoro();

    // setEtatPomodoro(pomodoro.etat);
    setEtatPomodoro(EnAttente);
    setActivationSonnette(pomodoro.sonnette);

    tempsHorloge.setTemps(pomodoro.duree);
    labelEtatPomodoro = tempsHorloge.getMMSS();
    controlerAffichageBouton();

#ifdef DEBUG_POMODORO
    Serial.println("[initialiserPreferences]");
    Serial.print("nomUtilisateur = ");
    Serial.println(nomUtilisateur.c_str());
    Serial.print("prenomUtilisateur = ");
    Serial.println(prenomUtilisateur.c_str());
    Serial.print("nomTache = ");
    Serial.println(nomTache.c_str());
    Serial.print("etatPomodoro = ");
    Serial.println((int)pomodoro.etat);
    Serial.print("activationSonnette = ");
    Serial.println((int)pomodoro.sonnette);
#endif
}

void initialiserPomodoro()
{
    int  id          = preferences.getInt("id", 0);
    bool actif       = preferences.getInt("actif", 0);
    int  sonnette    = preferences.getInt("sonnette", 1);
    int  etat        = preferences.getInt("etat", (EtatPomodoro)EnAttente);
    int  duree       = preferences.getInt("duree", DUREE_POMODORO);
    int  pauseCourte = preferences.getInt("pauseCourte", PAUSE_COURTE_POMODORO);
    int  pauseLongue = preferences.getInt("pauseLongue", PAUSE_LONGUE_POMODORO);
    int  nbPomodori  = preferences.getInt("nbPomodori", NB_POMODORI);
    int  autoPomodoro     = preferences.getInt("autoPomodoro", 0);
    int  autoPause        = preferences.getInt("autoPause", 0);
    int  mode             = preferences.getInt("mode", (Mode)Minuteur);
    int  nb               = preferences.getInt("nb", 0);
    pomodoro.id           = id;
    pomodoro.actif        = actif;
    pomodoro.sonnette     = sonnette;
    pomodoro.etat         = (EtatPomodoro)etat;
    pomodoro.duree        = duree;
    pomodoro.pauseCourte  = pauseCourte;
    pomodoro.pauseLongue  = pauseLongue;
    pomodoro.nbPomodori   = nbPomodori;
    pomodoro.autoPomodoro = autoPomodoro;
    pomodoro.autoPause    = autoPause;
    pomodoro.mode         = (Mode)mode;
    nbPomodori            = nb;
}

void initialiserTimers()
{
}

int getDuree()
{
    int duree = preferences.getInt("duree", DUREE_POMODORO);
    return duree;
}

int getDureePauseCourte()
{
    int pauseCourte = preferences.getInt("pauseCourte", PAUSE_COURTE_POMODORO);
    return pauseCourte;
}

int getDureePauseLongue()
{
    int pauseLongue = preferences.getInt("pauseLongue", PAUSE_LONGUE_POMODORO);
    return pauseLongue;
}

void setDuree(int duree)
{
    pomodoro.duree = duree;
}

void setDureePauseCourte(int duree)
{
    pomodoro.pauseCourte = duree;
}

void setDureePauseLongue(int duree)
{
    pomodoro.pauseLongue = duree;
}

void demarrerTimerPomodoro(int duree)
{
    timerPomodoro.once(duree, terminerTimerPomodoro, pomodoro.id);
}

void arreterTimerPomodoro()
{
    timerPomodoro.detach();
}

void terminerTimerPomodoro(int id)
{
    if(!pomodoro.actif)
        return;
    switch(pomodoro.etat)
    {
        case EnCours:
#ifdef DEBUG_POMODORO
            Serial.println("Fin du pomodoro");
#endif
            setEtatPomodoro(Termine);
            gererModeAutomatique();
            // setEtatPomodoro(EnAttente);
            break;
        case EnCourtePause:
#ifdef DEBUG_POMODORO
            Serial.println("Fin de la pause courte");
#endif
            setEtatPomodoro(FinCourtePause);
            gererModeAutomatique();
            // setEtatPomodoro(EnAttente);
            break;
        case EnLonguePause:
#ifdef DEBUG_POMODORO
            Serial.println("Fin de la pause longue");
#endif
            setEtatPomodoro(FinLonguePause);
            gererModeAutomatique();
            // setEtatPomodoro(EnAttente);
            break;
        default:
            break;
    }
}

void demarrerTimerHorloge()
{
    timerHorloge.attach(1, afficherHorloge); // toutes les s
}

void arreterTimerHorloge()
{
    timerHorloge.detach();
}

void afficherHorloge()
{
    if(tempsHorloge.getValeur() > 0)
        tempsHorloge -= 1;
#ifdef DEBUG_POMODORO_HORLOGE
    Serial.print(tempsHorloge.getMMSS());
    Serial.print(" -> ");
    Serial.print(tempsHorloge.getValeur());
    Serial.println(" s");
#endif
    // labelEtatPomodoro = tempsHorloge.getMMSS();
    switch(pomodoro.etat)
    {
        case EnAttente:
            break;
        case Termine:
            break;
        case EnCours:
            labelEtatPomodoro = tempsHorloge.getMMSS() + String(" Pomodoro");
            break;
        case EnCourtePause:
            labelEtatPomodoro =
              tempsHorloge.getMMSS() + String(" Pause courte");
            break;
        case FinCourtePause:
            break;
        case EnLonguePause:
            labelEtatPomodoro =
              tempsHorloge.getMMSS() + String(" Pause longue");
            break;
        case FinLonguePause:
            break;
        case Gele:
            break;
        case Reprise:
            break;
        default:
            break;
    }
}

/**
 * @brief Affiche les informations pour l'écran d'accueil
 *
 */
void afficherAccueil()
{
    String         titre        = "";
    String         stitre       = "=====================";
    String         nomBluetooth = PERIPHERIQUE_BLUETOOTH;
    const uint8_t* adresseESP32 = esp_bt_dev_get_address();
    char           str[18];
    sprintf(str,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            adresseESP32[0],
            adresseESP32[1],
            adresseESP32[2],
            adresseESP32[3],
            adresseESP32[4],
            adresseESP32[5]);
    stitre = String("=== ") + String(str) + String(" ===");
    titre  = String("Bluetooth : ") + nomBluetooth;

    afficheur.setTitre(titre);
    afficheur.setSTitre(stitre);
    rafraichirEcran();
}

/**
 * @brief Affiche les informations sur l'utilisateur
 *
 */
void afficherInformationsUtilisateur(bool redraw)
{
    nettoyerChaines();
    labelUtilisateur =
      String(prenomUtilisateur.c_str()) + " " + String(nomUtilisateur.c_str());
}

/**
 * @brief Affiche la tâche
 *
 */
void afficherTache(bool redraw)
{
    nettoyerChaines();
    if(nomTache.length() > LONGUEUR_MAX)
    {
        nomTache.resize(LONGUEUR_MAX - 3);
        nomTache.resize(LONGUEUR_MAX, '.');
    }

    labelNomTache = nomTache.c_str();
}

void afficherNbPomodori(bool redraw)
{
    String strNbPomodori = String(nbPomodori);
    labelNbPomodori      = "Pomodori : " + strNbPomodori;
}

void rafraichirEcran(uint32_t tempo)
{
    afficheur.setMessageLigne(Afficheur::Ligne1, labelUtilisateur);
    afficheur.setMessageLigne(Afficheur::Ligne2, labelNomTache);
    afficheur.setMessageLigne(Afficheur::Ligne3, labelEtatPomodoro);
    afficheur.afficher();
    if(antiRebond)
    {
        delay(tempo);
        antiRebond = false;
    }
}

// Fonctions utilitaires
int count(const String& str, const String& sub)
{
    if(sub.length() == 0)
        return 0;
    int count = 0;
    for(int offset = str.indexOf(sub); offset != -1;
        offset     = str.indexOf(sub, offset + sub.length()))
    {
        ++count;
    }
    return count;
}

void nettoyerCaractere(std::string& chaine,
                       std::string  caractere,
                       char         remplacement)
{
    std::size_t found = chaine.find(caractere);
    while(found != std::string::npos)
    {
        chaine.replace(found, 1, 1, remplacement);
        found = chaine.find(caractere);
    }
}

void nettoyerChaines()
{
    nettoyerCaractere(nomUtilisateur, "é", 'e');
    nettoyerCaractere(nomUtilisateur, "è", 'e');
    nettoyerCaractere(nomUtilisateur, "ê", 'e');
    nettoyerCaractere(prenomUtilisateur, "é", 'e');
    nettoyerCaractere(prenomUtilisateur, "è", 'e');
    nettoyerCaractere(prenomUtilisateur, "ê", 'e');
    nettoyerCaractere(nomTache, "é", 'e');
    nettoyerCaractere(nomTache, "è", 'e');
    nettoyerCaractere(nomTache, "ê", 'e');
    nettoyerCaractere(nomTache, "à", 'a');
}
