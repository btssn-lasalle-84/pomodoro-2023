/**
 * @file pomodoro.cpp
 *
 * @brief Définition de la classe Pomodoro
 * @author Pierre HIRSCH
 * @version 0.1
 */

#include "pomodoro.h"
#include "basededonnees.h"
#include "ui_pomodoro.h"
#include <QTimer>

/**
 * @brief Constructeur de la classe Pomodoro
 *
 * @fn Pomodoro::Pomodoro
 * @param parent nullptr pour définir la fenêtre principale de l'application
 */
Pomodoro::Pomodoro(QWidget* parent) :
    QWidget(parent), ui(new Ui::GUIPomodoro), bddPomodoro(nullptr), tempsRestant(0),
    compteRebours(nullptr), pomodoro(false), pause(false)
{
    qDebug() << Q_FUNC_INFO;

    ouvrirBaseDeDonnees();
    initialiserGui();
    connecterSignauxSlots();
}

/**
 * @brief Destructeur de la classe Pomodoro
 *
 * @fn Pomodoro::~Pomodoro
 * @details Libère les ressources de l'application
 */
Pomodoro::~Pomodoro()
{
    delete ui;
    BaseDeDonnees::detruireInstance();
    qDebug() << Q_FUNC_INFO;
}

/**
 * @fn Pomodoro::afficherEcran(Pomodoro::Ecran ecran)
 * @brief Selectionne et affiche l'écran
 */
void Pomodoro::afficherEcran(Pomodoro::Ecran ecran)
{
    ui->ecrans->setCurrentIndex(ecran);
}

/**
 * @fn Pomodoro::afficherEcranAccueil()
 * @brief Affiche l'écran d'accueil
 */
void Pomodoro::afficherEcranAccueil()
{
    ui->labelTitre->setText("Pomodoro");
    afficherEcran(Pomodoro::Ecran::Accueil);
}

/**
 * @fn Pomodoro::afficherEcranTache()
 * @brief Affiche l'écran d'une tâche
 */
void Pomodoro::afficherEcranTache()
{
    ui->labelTitre->setText("Édition d'une tâche");
    afficherEcran(Pomodoro::Ecran::Tache);
}

void Pomodoro::afficherEcranCycle()
{
    ui->labelTitre->setText("Édition d'un cycle");
    afficherEcran(Pomodoro::Ecran::Cycle);
}

void Pomodoro::afficherEcranSession()
{
    ui->labelTitre->setText("Édition d'une session");
    afficherEcran((Pomodoro::Ecran::Session));
}

void Pomodoro::afficherTexteTache()
{
    QString texte = ui->editionNomTache->text();
    ui->labelTache->setText(texte);
    afficherEcranAccueil();
}

void Pomodoro::initialiserGui()
{
    ui->setupUi(this);
#ifdef RASPBERRY_PI
    showFullScreen();
#else
    setFixedSize(qApp->desktop()->availableGeometry(this).width(),
                 qApp->desktop()->availableGeometry(this).height());
    // showMaximized();
#endif
    afficherEcranAccueil();
}

/**
 * @fn Pomodoro::ouvrirBaseDeDonnees()
 * @brief Instancie l'objet et l'accès à base de données
 */
void Pomodoro::ouvrirBaseDeDonnees()
{
    bddPomodoro = BaseDeDonnees::getInstance();
    bddPomodoro->ouvrir();
    qDebug() << Q_FUNC_INFO << bddPomodoro->estOuverte();
}

/**
 * @fn Pomodoro::connecterSignauxSlots()
 * @brief Connecte les signaux aux slots pour l'application
 */
void Pomodoro::connecterSignauxSlots()
{
    connect(ui->boutonGestionPomodoro, SIGNAL(clicked(bool)), this, SLOT(demarrerPomodoro()));
    connect(ui->boutonEditionTache, SIGNAL(clicked(bool)), this, SLOT(afficherEcranTache()));
    // Pour les tests
    connect(ui->boutonValidationEditionTache,
            SIGNAL(clicked(bool)),
            this,
            SLOT(afficherEcranAccueil()));

    connect(ui->boutonValidationEditionTache,
            &QPushButton::clicked,
            this,
            &Pomodoro::afficherTexteTache);

    connect(ui->boutonAnnulationEditionTache,
            SIGNAL(clicked(bool)),
            this,
            SLOT(afficherEcranAccueil()));

    connect(ui->boutonEditionCycle, SIGNAL(clicked(bool)), this, SLOT(afficherEcranCycle()));
    connect(ui->boutonEcranTache, SIGNAL(clicked(bool)), this, SLOT(afficherEcranTache()));
    connect(ui->boutonEcranSession, SIGNAL(clicked(bool)), this, SLOT(afficherEcranSession()));
    connect(ui->boutonEcranAccueil, SIGNAL(clicked(bool)), this, SLOT(afficherEcranAccueil()));

    connect(ui->boutonEditionSession, SIGNAL(clicked(bool)), this, SLOT(afficherEcranSession()));
    connect(ui->boutonEcranTache_2, SIGNAL(clicked(bool)), this, SLOT(afficherEcranTache()));
    connect(ui->boutonEcranCycle, SIGNAL(clicked(bool)), this, SLOT(afficherEcranCycle()));
    connect(ui->boutonEcranAccueil_2, SIGNAL(clicked(bool)), this, SLOT(afficherEcranAccueil()));
}

void Pomodoro::demarrerPomodoro()
{
    gererBoutonPomodoro();

    // déjà démarré ?
    if(pomodoro)
        return;

    if(ui->boutonModeCompteARebours->isChecked())
    {
#ifdef DEMO
        tempsRestant = DUREE_POMODORO;
#else
        tempsRestant = DUREE_POMODORO * 60;
#endif
        qDebug() << Q_FUNC_INFO << "CompteARebours"
                 << "tempsRestant" << tempsRestant;
    }
    else if(ui->boutonModeChronometre->isChecked())
    {
        tempsRestant = 0;
        qDebug() << Q_FUNC_INFO << "Chronometre"
                 << "tempsRestant" << tempsRestant;
    }

    if(compteRebours == nullptr)
    {
        compteRebours = new QTimer(this);
        compteRebours->setInterval(TOUTES_LES_SECONDES);

        // Connecter le signal timeout du QTimer au slot
        connect(compteRebours, &QTimer::timeout, this, &Pomodoro::mettreAJourAffichageDuree);
    }

    mettreAJourAffichageDuree();
    compteRebours->start();
    pomodoro = true;
    ui->boutonModeCompteARebours->setEnabled(false);
    ui->boutonModeChronometre->setEnabled(false);
    qDebug() << Q_FUNC_INFO << "pomodoro" << pomodoro;
}

void Pomodoro::mettreAJourAffichageDuree()
{
    if(pause)
        return;

    QString temps;
    if(ui->boutonModeCompteARebours->isChecked())
    {
        // Vérifier si le temps restant est écoulé
        if(tempsRestant <= 0)
        {
            arreterPomodoro();
        }

        // Calculer les minutes et les secondes
        int minutes  = tempsRestant / 60;
        int secondes = tempsRestant % 60;

        // Convertir les minutes et les secondes en chaînes de caractères avec le format "mm:ss"
        temps = QString("%1:%2").arg(QString::number(minutes).rightJustified(2, '0'),
                                     QString::number(secondes).rightJustified(2, '0'));

        ui->affichageDuree->display(temps);
        qDebug() << Q_FUNC_INFO << "CompteARebours" << temps;

        // Réduire le temps restant d'une seconde
        tempsRestant -= 1;
    }
    else if(ui->boutonModeChronometre->isChecked())
    {
        // Vérifier si le temps a atteint la durée
#ifdef DEMO
        if(tempsRestant >= DUREE_POMODORO)
#else
        if(tempsRestant >= DUREE_POMODORO * 60)
#endif
        {
            arreterPomodoro();
        }

        // Mettre à jour l'affichage du chronomètre
        int minutes  = tempsRestant / 60;
        int secondes = tempsRestant % 60;
        temps        = QString("%1:%2").arg(QString::number(minutes).rightJustified(2, '0'),
                                     QString::number(secondes).rightJustified(2, '0'));

        ui->affichageDuree->display(temps);
        qDebug() << Q_FUNC_INFO << "Chronometre" << temps;

        // Augmenter le temps restant de 1 seconde
        tempsRestant += 1;
    }
}

void Pomodoro::arreterPomodoro()
{
    if(compteRebours)
        compteRebours->stop();
    tempsRestant = 0;
    pomodoro     = false;
    pause        = false;
    ui->boutonGestionPomodoro->setText("Démarrer");
    ui->boutonModeCompteARebours->setEnabled(true);
    ui->boutonModeChronometre->setEnabled(true);
    qDebug() << Q_FUNC_INFO << "pomodoro" << pomodoro;
}

void Pomodoro::gererBoutonPomodoro()
{
    if(ui->boutonGestionPomodoro->text() == "Démarrer")
    {
        pause = false;
        ui->boutonGestionPomodoro->setText("Mettre en pause");
        ui->affichageDuree->setStyleSheet(
          "background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);border: 2px solid rgb(113, "
          "113, 113);border-width: 2px;border-radius: 10px;");
    }
    else if(ui->boutonGestionPomodoro->text() == "Mettre en pause")
    {
        pause = true;
        ui->boutonGestionPomodoro->setText("Démarrer");
        ui->affichageDuree->setStyleSheet(
          "background-color: rgb(0, 255, 0);color: rgb(255, 255, 255);border: 2px solid rgb(113, "
          "113, 113);border-width: 2px;border-radius: 10px;");
    }
    qDebug() << Q_FUNC_INFO << "pause" << pause;
}
