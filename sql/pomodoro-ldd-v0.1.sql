-- LDD (langage de définition de données)

-- Supprime les tables

DROP TABLE IF EXISTS Preferences;
DROP TABLE IF EXISTS Pomodoro;
DROP TABLE IF EXISTS Etat;
DROP TABLE IF EXISTS Modele;
DROP TABLE IF EXISTS Tache;
DROP TABLE IF EXISTS Mode;

--- Création des tables

-- Structure de la table Mode

CREATE TABLE IF NOT EXISTS Mode(idMode INTEGER PRIMARY KEY, mode VARCHAR);

-- Deux modes de base

INSERT INTO Mode(idMode,mode) VALUES
(1,'Minuteur'),
(2,'Chronomètre');

-- ---------------------------------------------------------------------

-- Structure de la table Modele

CREATE TABLE IF NOT EXISTS Modele (
  idModele INTEGER PRIMARY KEY AUTOINCREMENT,
  nom VARCHAR NOT NULL,
  idMode INTEGER DEFAULT '1',
  dureePomodoro INTEGER DEFAULT '1500',
  pauseCourte INTEGER DEFAULT '300',
  pauseLongue INTEGER DEFAULT '1200',
  nbPomodori INTEGER DEFAULT '4',
  autoPomodoro INTEGER DEFAULT '0',
  autoPause INTEGER DEFAULT '1',
  UNIQUE(nom),
  CONSTRAINT Modele_fk_1 FOREIGN KEY (idMode) REFERENCES Mode(idMode) ON DELETE CASCADE
);

INSERT INTO Modele (nom,dureePomodoro,pauseCourte,pauseLongue,nbPomodori,autoPomodoro,autoPause) VALUES ('Classique','1500','300','1200','4','0','1');
INSERT INTO Modele (nom,dureePomodoro,pauseCourte,pauseLongue,nbPomodori,autoPomodoro,autoPause) VALUES ('Personnel','1800','60','1500','4','0','1');
INSERT INTO Modele (nom,dureePomodoro,pauseCourte,pauseLongue,nbPomodori,autoPomodoro,autoPause) VALUES ('Travail','3000','600','1200','2','0','1');

-- ---------------------------------------------------------------------

-- Structure de la table Etat

CREATE TABLE IF NOT EXISTS Etat(idEtat INTEGER PRIMARY KEY, etat VARCHAR);

INSERT INTO Etat(idEtat,etat) VALUES
(0,'prêt'),
(1,'démarré'),
(2,'pause'),
(3,'terminé');

-- ---------------------------------------------------------------------

-- Structure de la table Tache

CREATE TABLE IF NOT EXISTS Tache (
  idTache INTEGER PRIMARY KEY AUTOINCREMENT,
  titre VARCHAR NOT NULL,
  description VARCHAR,
  dateCreation DATETIME NOT NULL,
  dateDebut DATETIME,
  dateFin DATETIME,
  couleur VARCHAR,
  position INTEGER DEFAULT '0',
  active INTEGER DEFAULT '0',
  tempsDepense INTEGER DEFAULT '0',
  tempsEstime INTEGER DEFAULT '0',
  priorite INTEGER DEFAULT '0',
  terminee INTEGER DEFAULT '0'
);

-- ---------------------------------------------------------------------

-- Structure de la table Pomodoro

CREATE TABLE IF NOT EXISTS Pomodoro (
  idPomodoro INTEGER PRIMARY KEY AUTOINCREMENT,
  idTache INTEGER NOT NULL,
  idModele INTEGER NOT NULL,
  idEtat INTEGER DEFAULT '0',
  position INTEGER DEFAULT '0',
  debut DATETIME,
  fin DATETIME,
  duree INTEGER DEFAULT '0',
  CONSTRAINT Pomodoro_fk_1 FOREIGN KEY (idTache) REFERENCES Tache(idTache) ON DELETE CASCADE,
  CONSTRAINT Pomodoro_fk_2 FOREIGN KEY (idModele) REFERENCES Modele(idModele) ON DELETE CASCADE,
  CONSTRAINT Pomodoro_fk_3 FOREIGN KEY (idEtat) REFERENCES Etat(idEtat) ON DELETE CASCADE
);

-- ---------------------------------------------------------------------

-- Structure de la table Preferences

CREATE TABLE IF NOT EXISTS Preferences (
  idPreferences INTEGER PRIMARY KEY AUTOINCREMENT,
  nom VARCHAR NOT NULL,
  prenom VARCHAR,
  idPomodoro INTEGER DEFAULT '0',
  idModele INTEGER NOT NULL,
  UNIQUE(nom,prenom),
  -- CONSTRAINT Preferences_fk_2 FOREIGN KEY (idPomodoro) REFERENCES Pomodoro(idPomodoro) ON DELETE CASCADE,
  CONSTRAINT Preferences_fk_3 FOREIGN KEY (idModele) REFERENCES Modele(idModele) ON DELETE CASCADE
);

-- ---------------------------------------------------------------------

-- Pour les tests

INSERT INTO Tache (titre,description,dateCreation,dateDebut,dateFin,position, terminee) VALUES ('Planifier les tâches','Identifier et prioriser les tâches',DATETIME('now'),'2023-03-30 08:15:00','2023-03-30 08:40:00',1,1);
INSERT INTO Tache (titre,description,dateCreation,position) VALUES ('Maquette IHM','Définir une interface Homme-Machine',DATETIME('now'),1);
INSERT INTO Tache (titre,description,dateCreation,position) VALUES ('Classes du domaine','Réaliser le diagramme de classes du domaine',DATETIME('now'),2);
INSERT INTO Tache (titre,description,dateCreation,position) VALUES ('Implémenter squelettes','Coder les squelettes des classes',DATETIME('now'),3);

INSERT INTO Preferences (nom,prenom,idPomodoro,idModele) VALUES ('HIRSCH','Pierre',0,1);
