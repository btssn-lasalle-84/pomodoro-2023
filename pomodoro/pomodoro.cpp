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
Pomodoro::Pomodoro(QWidget* parent) : QWidget(parent), ui(new Ui::GUIPomodoro)
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
                 w² qApp->desktop()->availableGeometry(this).height());
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

void Pomodoro::on_boutonGestionPomodoro_clicked()
{
    QPushButton* boutonPause = new QPushButton("Pause", this);
    QPushButton* boutonArret = new QPushButton("Arrêt", this);

    boutonPause->setStyleSheet("background-color:red; color:white;");
    boutonArret->setStyleSheet("background-color:red; color:white;");

    ui->verticalLayoutPomodoro->addWidget(boutonPause);
    ui->verticalLayoutPomodoro->addWidget(boutonArret);

    ui->verticalLayoutPomodoro->removeWidget(ui->boutonGestionPomodoro);
    ui->boutonGestionPomodoro->deleteLater();

    if (ui->boutonModeCompteARebours->isChecked())
    {
        // Définir la durée initiale à 20 minutes (1200 secondes)
    int initialTime = 20 * 60;
    tempsRestant = initialTime;

        // Mettre à jour l'affichage initial du compte à rebours
    compteReboursAJour();

        // Créer le QTimer s'il n'existe pas déjà
    if (!compteRebours)
    {
        compteRebours = new QTimer(this);
        compteRebours->setInterval(1000); // Mettre à jour le compte à rebours toutes les secondes

            // Connecter le signal timeout du QTimer au slot updateCountdown
        connect(compteRebours, &QTimer::timeout, this, &Pomodoro::compteReboursAJour);
    }

        // Démarrer le QTimer
    compteRebours->start();
    }

    else if (ui->boutonModeChronometre->isChecked())
    {
        // Définir la durée initiale à 0 secondes
        int initialTime = 0;
        tempsRestant = initialTime;

        // Mettre à jour l'affichage initial du chronomètre
        int minutes = tempsRestant / 60000;
        int seconds = (tempsRestant / 1000) % 60;
        QString timeString = QString("%1:%2").arg(QString::number(minutes).rightJustified(2, '0'), QString::number(seconds).rightJustified(2, '0'));
        ui->Timer->display(timeString);

        // Créer le QTimer s'il n'existe pas déjà
        if (!compteRebours)
        {
            compteRebours = new QTimer(this);
            compteRebours->setInterval(1000); // Mettre à jour le chronomètre toutes les 1000 millisecondes (1 seconde)

            // Connecter le signal timeout du QTimer au slot updateCountdown
            connect(compteRebours, &QTimer::timeout, this, &Pomodoro::compteReboursAJour);
        }

        // Démarrer le QTimer
        compteRebours->start();
    }
}
    // Dans la méthode updateCountdown()
void Pomodoro::compteReboursAJour()
    {
        // Vérifier si le temps restant est écoulé
        if (tempsRestant <= 0)
        {
            // Arrêter le QTimer
            compteRebours->stop();

            // Définir la durée à 5 minutes (300 secondes)
            int newTime = 5 * 60;
            tempsRestant = newTime;

            // Mettre à jour l'affichage du compte à rebours
            compteReboursAJour();
            return;
        }

        // Calculer les minutes et les secondes
        int minutes = tempsRestant / 60;
        int seconds = tempsRestant % 60;

        // Convertir les minutes et les secondes en chaînes de caractères avec le format "mm:ss"
        QString timeString = QString("%1:%2").arg(QString::number(minutes).rightJustified(2, '0'), QString::number(seconds).rightJustified(2, '0'));

        // Afficher le compte à rebours dans le QLCDNumber
        ui->Timer->display(timeString);

        // Réduire le temps restant d'une seconde
        tempsRestant--;


        // Vérifier si le QRadioButton "boutonModeChronometre" est toujours coché
        if (ui->boutonModeChronometre->isChecked())
        {
            // Augmenter le temps restant de 1 seconde
            tempsRestant += 1000;

            // Mettre à jour l'affichage du chronomètre
            int minutes = tempsRestant / 60000;
            int seconds = (tempsRestant / 1000) % 60;
            QString timeString = QString("%1:%2").arg(QString::number(minutes).rightJustified(2, '0'), QString::number(seconds).rightJustified(2, '0'));
            ui->Timer->display(timeString);
        }
    }

void Pomodoro::on_boutonArret_clicked()
{
    // Arrêter le compte à rebours en stoppant le QTimer
    compteRebours->stop();

    // Réinitialiser le compte à rebours à 0
    ui->Timer->display("00:00");

    // Réafficher le bouton "boutonGestionPomodoro"
    ui->boutonGestionPomodoro->show();;
}

void Pomodoro::on_boutonPause_clicked()
{

}
