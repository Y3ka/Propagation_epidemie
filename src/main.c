/**
 * @dir src
 * @brief Dossier code source
 */

/**
 * @file main.c
 *
 * @brief Propagation d’une épidémie dans une population
 *
 * Fichier principal.
 *
 * Fonctionnalités :
 * - Simulateur de propagation d’une épidémie dans une population
 * - Analyse des performances
 * - Extensions
 *
 * @author Marc NGUYEN
 * @author Thomas LARDY
 * @date 26 Mar 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "jouer_un_tour.h"
#include "population.h"
#include "statistique.h"

void printHelp(void);

/**
 * @brief Execute la simulation d'une propagation, et sort des statistiques.
 *
 * Executez le programme avec l'argument --help pour connaitre les arguments.
 *
 * @param argc
 * @param argv
 * @return int exit(0)
 */
int main(int argc, char const* argv[]) {
  // Defauts
  srand(time(NULL));
  unsigned long hauteur = 20;
  unsigned long limite = 80;
  unsigned duree_incube = 4;
  double beta = 0.5;    // MALADE -> MORT
  double gamma = 0.1;   // MALADE -> IMMUNISE
  double lambda = 1.0;  // SAIN -> INCUBE
  double chance_quarantaine = 0.1;
  double chance_decouverte_vaccin =
      0.001;  // SAIN/IMMUNISE -> VACINE par découverte
  char* file_graph = "graphique.txt";
  char* file_data = "data.txt";
  char* file_tableau = "tableau de bord.txt";
  unsigned long tour_max = 100;
  int duree_quarantaine = 20;  // Très grand au tps d'incub
  int cordon_sanitaire = 5;    // Généralement égal au temps d'incubation

  // Arguments positionnés
  long x, y;
  unsigned long cote;
  if (argc < 2) {
    printf("ERREUR : Pas assez d'arguments.\n\n");
    printHelp();
  }
  if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) printHelp();
  if (argc < 4) {
    printf("ERREUR : Pas assez d'arguments.\n\n");
    printHelp();
  }
  sscanf(argv[1], "%li", &x);
  sscanf(argv[2], "%li", &y);
  sscanf(argv[3], "%lu", &cote);

  // Arguments nommés (incrémenter i_défaut si arguments positionné)
  for (int i = 4; i < argc; i++) {
    if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--mort"))
      sscanf(argv[i + 1], "%lf", &beta);

    if (!strcmp(argv[i], "-g") || !strcmp(argv[i], "--immunise"))
      sscanf(argv[i + 1], "%lf", &gamma);

    if (!strcmp(argv[i], "-la") || !strcmp(argv[i], "--malade") ||
        !strcmp(argv[i], "--incube"))
      sscanf(argv[i + 1], "%lf", &lambda);

    if (!strcmp(argv[i], "-ha") || !strcmp(argv[i], "--hauteur"))
      sscanf(argv[i + 1], "%lu", &hauteur);

    if (!strcmp(argv[i], "-li") || !strcmp(argv[i], "--limite"))
      sscanf(argv[i + 1], "%lu", &limite);

    if (!strcmp(argv[i], "--graph") || !strcmp(argv[i], "-og"))
      strcpy(file_graph, argv[i + 1]);

    if (!strcmp(argv[i], "--data") || !strcmp(argv[i], "-od"))
      strcpy(file_data, argv[i + 1]);

    if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--tours"))
      sscanf(argv[i + 1], "%lu", &tour_max);

    if (!strcmp(argv[i], "--tableau") || !strcmp(argv[i], "-ot"))
      strcpy(file_tableau, argv[i + 1]);

    if (!strcmp(argv[i], "-di") || !strcmp(argv[i], "--duree-incube"))
      sscanf(argv[i + 1], "%u", &duree_incube);

    if (!strcmp(argv[i], "--quarantaine") || !strcmp(argv[i], "-q"))
      sscanf(argv[i + 1], "%lf", &chance_quarantaine);

    if (!strcmp(argv[i], "--vaccin") || !strcmp(argv[i], "-v"))
      sscanf(argv[i + 1], "%lf", &chance_decouverte_vaccin);

    if (!strcmp(argv[i], "--cordon"))
      sscanf(argv[i + 1], "%i", &cordon_sanitaire);

    if (!strcmp(argv[i], "-dq") || !strcmp(argv[i], "--duree-quarantaine"))
      sscanf(argv[i + 1], "%i", &duree_quarantaine);
  }

  // Init
  struct Statistique stats;
  struct Data* data = creerData();
  struct Population* population = creerPopulation(cote);
  data->population_totale = getTaillePopulation(population);
  stats = getStatistique(population);
  appendData(data, stats);

  // Traitement
  patient_zero(population, x, y);
  afficherGrillePopulation(population);
  for (unsigned long i = 0; i < tour_max && zombiePresent(population); i++) {
    jouerTour(population, beta, gamma, lambda, chance_quarantaine,
              chance_decouverte_vaccin, duree_incube, cordon_sanitaire,
              duree_quarantaine);
    stats = getStatistique(population);
    afficherGrillePopulation(population);
    appendData(data, stats);
  }

  // Output
  printf("Graphique (colonne = tour, charactère = %%état)\n");
  graphique(data, file_graph, hauteur, limite);
  printf("Exportation...");
  exporter(data, file_data);
  printf("OK\n");
  printf("Tableau de bord\n");
  tableau(data, file_tableau);

  return 0;
}

/**
 * @brief Affiche l'aide.
 *
 */
void printHelp(void) {
  printf(
      "Projet de semestre Mar-Apr 2019. Propagation d’une épidémie dans une population par Marc NGUYEN et Thomas LARDY\n\
\n\
Usage: ProjetS6-MarcNGUYEN-ThomasLARDY <x> <y> <cote> [options...]\n\n\
Arguments:\n\
  x, y                    coordonnées du premier infecté\n\
  cote	                  taille de la population (grille cote x cote)\n\
Population Options:\n\
  -t,  --tours            tours max de la simulation               [défaut: 100]\n\
\n\
Simulation Options Générales:\n\
  -b,  --mort             [0, 1] proba de mourir par la maladie    [défaut: 0.5]\n\
  -g,  --immunise         [0, 1] proba d'être immunise             [défaut: 0.1]\n\
  -la, --malade --incube  [0, 1] proba de contamination            [défaut: 1.0]\n\
\n\
Output Options:\n\
  -od, --data             nom de données brutes               [défaut: data.txt]\n\
  -og, --graph            nom du graphique               [défaut: graphique.txt]\n\
  -ot, --tableau          nom du tableau de bord   [défaut: tableau de bord.txt]\n\
  -li, --limite           limite de char/ligne du graphique ASCII   [défaut: 80]\n\
  -ha, --hauteur          hauteur du graphique ASCII                [défaut: 20]\n\
\n\
Extension Incubation:\n\
  -di, --duree-incube     durée d'une incubation                     [défaut: 4]\n\
\n\
Extension Vaccin et Quarantaine:\n\
  -q,  --quarantaine      [0, 1] proba d'une quarantaine           [défaut: 0.1]\n\
  -dq, --duree-quarantaine  durée d'une quarantaine                 [défaut: 20]\n\
       --cordon           taille du cordon sanitaire                 [défaut: 5]\n\
  -v,  --vaccin           [0, 1] proba de développer un vaccin   [défaut: 0.001]\n\
\n\
Autres:\n\
  -h,  --help             Affiche ce dialogue\n");
  exit(0);
}
