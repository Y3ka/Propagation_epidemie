/**
 * @file population_test.c
 * @author Marc NGUYEN
 * @author Thomas LARDY
 * @brief Lancer des tests unitaires sur les fonctions de manipulation de
 * Population.
 * @date 27 Mar 2019
 *
 */

#include <assert.h>
#include <stdio.h>

#include "../src/population.h"

/**
 * @brief Main pour tourner les tests unitaires.
 *
 * @return int Exit 0.
 */
int main(void) {
  printf("population_test:\n");
  printf("  creerPopulation:\n");
  struct Population *population = creerPopulation(7);
  assert(population->cote == 7);
  printf("    Population cote: \x1B[32mOK\x1B[0m\n");

  for (unsigned long i = 0; i < population->cote; i++) {
    for (unsigned long j = 0; j < population->cote; j++) {
      assert(population->grille_de_personnes[i][j]->state == SAIN);
    }
  }
  printf("    Population grille_de_personnes: \x1B[32mOK\x1B[0m\n");

  printf("  clonerGrillePopulation:\n");
  struct Personne ***grille_clone = clonerGrillePopulation(population);
  assert(grille_clone != population->grille_de_personnes);
  printf(
      "    Malloc grille_de_personnes Clone != Malloc Population "
      "Source->grille_de_personnes: \x1B[32mOK\x1B[0m\n");
  for (unsigned long i = 0; i < population->cote; i++) {
    for (unsigned long j = 0; j < population->cote; j++) {
      assert(grille_clone[i][j] != population->grille_de_personnes[i][j]);
    }
  }
  printf(
      "    Malloc grille_de_personnes[i][j] Clone != Malloc Population "
      "Source->grille_de_personnes[i][j]: \x1B[32mOK\x1B[0m\n");
  for (unsigned long i = 0; i < population->cote; i++) {
    for (unsigned long j = 0; j < population->cote; j++) {
      assert(grille_clone[i][j]->state ==
             population->grille_de_personnes[i][j]->state);
    }
  }
  printf(
      "    State Personne Clone == State Personne Source: \x1B[32mOK\x1B[0m\n");
  printf("  afficherGrillePopulation:\n");
  afficherGrillePopulation(population);
  printf("    Affichage OK: \x1B[32mOK\x1B[0m\n");

  printf("  patient_zero:\n");
  patient_zero(population, 1, 1);
  assert(population->grille_de_personnes[1][1]->state == MALADE);
  printf("    State OK: \x1B[32mOK\x1B[0m\n");

  printf("  getTaillePopulation:\n");
  assert(population->cote * population->cote ==
         getTaillePopulation(population));
  printf("    Taille OK: \x1B[32mOK\x1B[0m\n");

  return 0;
}
