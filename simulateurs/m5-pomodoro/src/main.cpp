#include <Arduino.h>
#include "BluetoothSerial.h"
#include <M5Stack.h>
#include "pomodoro.h"

/**
 * @brief Initialise les ressources
 *
 * @fn setup()
 */
void setup()
{
  M5.begin(true, false, true, true);
  M5.Power.begin();

  #ifdef DEBUG
  Serial.println(F("Simulateur Bluetooth POMODORO"));
  #endif

  // initialise le support SD
  initialiserSD();

  // initialise l'écran
  initialiserEcran();

  // initialise le bluetooth
  initialiserBluetooth();

  // Initialise les préférences stockées
  initialiserPreferences();

  afficherAccueil();
}

/**
 * @brief Boucle principale
 *
 * @fn loop()
 */
void loop()
{
  String trame;
  TypeTrame typeTrame;
  String champType;
  TypePause typePause;

  if(lireTrame(trame))
  {
    typeTrame = verifierTrame(trame);
    #ifdef DEBUG
    if(typeTrame > Inconnu && typeTrame < NB_TRAMES)
      Serial.println("Trame : " + getNomTrame(typeTrame));
    #endif
    //POMODORO, USER, BELL, TASK, REST, STOP, RESET, WAIT, DARN, HEARTBEAT, STATE, ERROR, ACK,
    //"P",      "U",  "B",  "T",  "R",  "S",  "X",   "W",  "D",  "H",       "E",   "F",   "A"
    switch(typeTrame)
    {
      case TypeTrame::Inconnu:
        envoyerTrameErreur(ERREUR_TRAME_INCONNUE);
        break;
      case TypeTrame::POMODORO:
        if(getEtatPomodoro() == EnAttente)
        {
          #ifdef DEBUG
          Serial.println("Configuration POMODORO");
          #endif
          if(configurerPomodoro(trame))
          {
            envoyerTrameAcquittement();
          }
          else
          {
            envoyerTrameErreur(ERREUR_CONFIGURATION);
          }
        }
        else
          envoyerTrameErreur(ERREUR_COMMANDE);
        break;
      case TypeTrame::USER:
        if(getEtatPomodoro() == EnAttente)
        {
          #ifdef DEBUG
          Serial.println("Configuration UTILISATEUR");
          #endif
          if(configurerUtilisateur(trame))
          {
            envoyerTrameAcquittement();
          }
          else
          {
            envoyerTrameErreur(ERREUR_CONFIGURATION);
          }
        }
        else
          envoyerTrameErreur(ERREUR_COMMANDE);
        break;
      case TypeTrame::BELL:
        #ifdef DEBUG
        Serial.println("Configuration SONNERIE");
        #endif
        if(configurerSonnette(trame))
        {
          envoyerTrameAcquittement();
        }
        else
        {
          envoyerTrameErreur(ERREUR_CONFIGURATION);
        }
        break;
      case TypeTrame::TASK:
        if(getEtatPomodoro() == EnAttente)
        {
          if(configrerTache(trame))
          {
            envoyerTrameAcquittement();
            int duree = getDuree();
            setDuree(duree);
            setEtatPomodoro(EnCours);
            #ifdef DEBUG
            Serial.println("Démarrage tâche");
            #endif
          }
          else
          {
            envoyerTrameErreur(ERREUR_PARAMETRE);
          }
        }
        else
          envoyerTrameErreur(ERREUR_COMMANDE);
        break;
      case TypeTrame::REST: // Pause
        champType = extraireChamp(trame, CHAMP_TYPE_PAUSE);
        #ifdef DEBUG_VERIFICATION
        Serial.print("type = ");
        Serial.println(champType);
        #endif
        typePause = verifierTypePause(champType);
        switch(typePause)
        {
          case COURTE:
            if(getEtatPomodoro() == EnAttente)
            {
              envoyerTrameAcquittement();
              int duree = getDureePauseCourte();
              setDureePauseCourte(duree);
              setEtatPomodoro(EnCourtePause);
              #ifdef DEBUG
              Serial.println("Tâche en pause courte");
              #endif
            }
            else
              envoyerTrameErreur(ERREUR_COMMANDE);
            break;
          case LONGUE:
            if(getEtatPomodoro() == EnAttente)
            {
              envoyerTrameAcquittement();
              int duree = getDureePauseLongue();
              setDureePauseLongue(duree);
              setEtatPomodoro(EnLonguePause);
              #ifdef DEBUG
              Serial.println("Tâche en pause longue");
              #endif
            }
            else
              envoyerTrameErreur(ERREUR_COMMANDE);
            break;
            default:
              envoyerTrameErreur(ERREUR_PARAMETRE);
              break;
        }
        break;
      case TypeTrame::STOP:
        if(getEtatPomodoro() > Termine)
        {
          #ifdef DEBUG
          if(getEtatPomodoro() == EnCours)
            Serial.println("Tâche terminée");
          else if(getEtatPomodoro() == EnCourtePause)
            Serial.println("Pause courte terminée");
          else if(getEtatPomodoro() == EnLonguePause)
            Serial.println("Pause longue terminée");
          #endif
          envoyerTrameAcquittement();
          if(getEtatPomodoro() == EnCours)
            setEtatPomodoro(Termine);
          else if(getEtatPomodoro() == EnCourtePause)
            setEtatPomodoro(FinCourtePause);
          else if(getEtatPomodoro() == EnLonguePause)
            setEtatPomodoro(FinLonguePause);
          gererModeAutomatique();
        }
        else
          envoyerTrameErreur(ERREUR_COMMANDE);
        break;
      case TypeTrame::RESET:
        envoyerTrameAcquittement();
        reinitialiserPomodoro();
        break;
      case TypeTrame::WAIT:
        if(getEtatPomodoro() == EnCours || getEtatPomodoro() == EnCourtePause || getEtatPomodoro() == EnLonguePause)
        {
          envoyerTrameAcquittement();
          setEtatPomodoro(Gele);
          #ifdef DEBUG
          Serial.println("Tâche gelée");
          #endif
        }
        else
          envoyerTrameErreur(ERREUR_COMMANDE);
        //envoyerTrameErreur(ERREUR_TRAME_NON_SUPPORTEE);
        break;
      case TypeTrame::DARN: // Reprise
        if(getEtatPomodoro() == Gele)
        {
          String tempsReprise = extraireChamp(trame, 1);
          envoyerTrameAcquittement();
          switch(getEtatPomodoroPrecedent())
          {
             case EnCours:
                setDuree(tempsReprise.toInt());
                break;
            case EnCourtePause:
                setDureePauseCourte(tempsReprise.toInt());
                break;
            case EnLonguePause:
                setDureePauseLongue(tempsReprise.toInt());
                break;
          }
          setEtatPomodoro(getEtatPomodoroPrecedent());
          #ifdef DEBUG
          Serial.println("Tâche en reprise : ");
          Serial.print(tempsReprise);
          #endif
        }
        else
          envoyerTrameErreur(ERREUR_COMMANDE);
        //envoyerTrameErreur(ERREUR_TRAME_NON_SUPPORTEE);
        break;
      case TypeTrame::HEARTBEAT:
        envoyerTrameAcquittement();
        break;
      case TypeTrame::ERROR:
        envoyerTrameErreur(ERREUR_TRAME_NON_SUPPORTEE);
        break;
      case TypeTrame::ACK:
        break;
      default:
        #ifdef DEBUG
        Serial.println("Trame invalide !");
        #endif
        break;
    }
  }

  if(estEcheance(5000))
  {
    lireNiveauBluetooth();
    lireNiveauBatterie();
  }

  rafraichirEcran();
}
