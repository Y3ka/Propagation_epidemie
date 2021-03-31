/**
 * @file population.c
 *
 * @brief Manipulateur de population.
 *
 * Fonctionnalités :
 * - Créer
 * - Cloner
 * - Afficher
 * - Faire le patient zero
 * - Obtenir la taille de la population
 *
 * @author Marc NGUYEN
 * @author Thomas LARDY
 * @date 26 Mar 2019
 */

#include <stdio.h>
#include <stdlib.h>

#include "population.h"

/**
 * @brief Créer une population dans une grille cote*cote.
 *
 * Usage:
 * ```
 * struct Population *population = creerPopulation(3);
 * ```
 *
 * @param cote Longueur de la
 * @return struct Personne**
 */
struct Population *creerPopulation(const unsigned long cote) {
  struct Population *population =
      (struct Population *)malloc(sizeof(struct Personne));
  struct Personne ***grille_de_personnes =
      (struct Personne ***)malloc(sizeof(struct Personne **) * cote);
  // Malloc x et y, la grille de Personne
  for (unsigned long i = 0; i < cote; i++) {
    grille_de_personnes[i] =
        (struct Personne **)malloc(sizeof(struct Personne *) * cote);
    for (unsigned long j = 0; j < cote; j++) {
      grille_de_personnes[i][j] =
          (struct Personne *)malloc(sizeof(struct Personne) * cote);

      grille_de_personnes[i][j]->state = SAIN;
      grille_de_personnes[i][j]->duree_quarantaine = 0;
    }
  }
  population->grille_de_personnes = grille_de_personnes;
  population->cote = cote;
  return population;
}

/**
 * @brief Clone une grille d'une population.
 *
 * Usage:
 * ```
 * struct Population *population = creerPopulation(3);
 * struct Personne *** grille_de_personnes = clonerGrillePopulation(population);
 * ```
 *
 * @param population_source Population source.
 * @return struct Population* Grille de population destination.
 */
struct Personne ***clonerGrillePopulation(
    struct Population *population_source) {
  struct Personne ***grille_de_personnes = (struct Personne ***)malloc(
      sizeof(struct Personne **) * population_source->cote);
  // Malloc x et y, la grille de Personne
  for (unsigned long i = 0; i < population_source->cote; i++) {
    grille_de_personnes[i] = (struct Personne **)malloc(
        sizeof(struct Personne *) * population_source->cote);
    for (unsigned long j = 0; j < population_source->cote; j++) {
      grille_de_personnes[i][j] = (struct Personne *)malloc(
          sizeof(struct Personne) * population_source->cote);

      // Remplissage
      grille_de_personnes[i][j]->state =
          population_source->grille_de_personnes[i][j]->state;
      grille_de_personnes[i][j]->duree_quarantaine =
          population_source->grille_de_personnes[i][j]->duree_quarantaine;
      grille_de_personnes[i][j]->duree_incube =
          population_source->grille_de_personnes[i][j]->duree_incube;
    }
  }
  return grille_de_personnes;
}

/**
 * @brief Affiche la grille de population.
 * 
 * - IMMUNISE = '*'. Une étoile, parce qu'il est immortel.
 * - VACCINE = '$'. Un dollar, parce qu'il a payé un vaccin
 * - SAIN = '.'. Un ., un homme en parfaite santé
 * - INCUBE = 'u'. Un o pas formé
 * - MALADE = 'o'. Malade, une bulle qui va pas tarder à éclater
 * - MORT = ' '. Vide
 *
 * Usage:
 * ```
 * struct Population *population = creerPopulation(3);
 * afficherGrillePopulation(population);
 * ```
 *
 * @param population Population à afficher à l'instant t.
 */
void afficherGrillePopulation(struct Population *population) {
  for (unsigned long i = 0; i < population->cote; i++) printf("-");
  printf("\n");
  for (unsigned long i = 0; i < population->cote; i++) {
    for (unsigned long j = 0; j < population->cote; j++) {
      switch (population->grille_de_personnes[i][j]->state) {
        case IMMUNISE:
          printf(population->grille_de_personnes[i][j]->duree_quarantaine
                     ? "\e[32m*\e[0m"
                     : "\e[33m*\e[0m");
          break;

        case MALADE:
          printf(population->grille_de_personnes[i][j]->duree_quarantaine
                     ? "\e[32mo\e[0m"
                     : "\e[31mo\e[0m");
          break;

        case SAIN:
          printf(population->grille_de_personnes[i][j]->duree_quarantaine
                     ? "\e[32m.\e[0m"
                     : ".");
          break;

        case MORT:
          printf(" ");
          break;

        case INCUBE:
          printf(population->grille_de_personnes[i][j]->duree_quarantaine
                     ? "\e[32mu\e[0m"
                     : "\e[35mu\e[0m");
          break;

        case VACCINE:
          printf(population->grille_de_personnes[i][j]->duree_quarantaine
                     ? "\e[32m$\e[0m"
                     : "\e[33m$\e[0m");
          break;
      }
      if (population->grille_de_personnes[i][j]->duree_quarantaine)
        printf("\e[0m");
    }
    printf("\n");
  }
  for (unsigned long i = 0; i < population->cote; i++) printf("-");
  printf("\n");
  printf("\n");
}

/**
 * @brief Créer le patient zero, début de l'infection.
 *
 * Usage:
 * ```
 * struct Population* population = creerPopulation(7);
 * patient_zero(population, 0, 6);
 * ```
 *
 * @param population Grille au temps 0.
 * @param x Position horizontal.
 * @param y Position vertical.
 */
void patient_zero(struct Population *population, unsigned long x,
                  unsigned long y) {
  population->grille_de_personnes[x][y]->state = MALADE;
}

/**
 * @brief Récupérer la taille totale de la Population de la Population
 *
 * @param population Structure Population.
 * @return unsigned long Taille totale de la population.
 */
unsigned long getTaillePopulation(struct Population *population) {
  return population->cote * population->cote;
}
