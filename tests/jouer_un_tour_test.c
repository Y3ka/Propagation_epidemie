/**
 * @file jouer_un_tour_test.c
 * @author Marc NGUYEN
 * @author Thomas LARDY
 * @brief Lancer des tests unitaires sur les fonctions principales.
 * @date 27 Mar 2019
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/jouer_un_tour.h"
#include "../src/population.h"

/**
 * @brief Main pour tourner les tests unitaires.
 *
 * @return int Exit 0.
 */
int main(void) {
  printf("jouer_un_tour_test:\n");
  struct Population* population = creerPopulation(7);
  printf("  zombiePresent:\n");
  assert(!zombiePresent(population));
  afficherGrillePopulation(population);
  printf("    False: \x1B[32mOK\x1B[0m\n");
  patient_zero(population, 0, 0);
  assert(zombiePresent(population));
  afficherGrillePopulation(population);
  printf("    True: \x1B[32mOK\x1B[0m\n");
  printf("  mettreEnQuarantaine:\n");
  mettreEnQuarantaine(population->grille_de_personnes, 0, 0, population->cote,
                      5, 5);
  afficherGrillePopulation(population);
  assert(population->grille_de_personnes[0][0]->duree_quarantaine);
  printf("    Origin 0 quarantine: \x1B[32mOK\x1B[0m\n");
  for (int i = 1; i <= 5; i++) {
    for (int j = 0; j < i; j++)
      assert(population->grille_de_personnes[i - j - 1][j]->duree_quarantaine);
    printf("    0 -> Range %i quarantine: \x1B[32mOK\x1B[0m\n", i);
  }
  for (int j = 0; j < 7; j++)
    assert(!population->grille_de_personnes[6 - j][j]->duree_quarantaine);
  printf("    0 -> Range 6 not quarantine: \x1B[32mOK\x1B[0m\n\n");
  return 0;
}
