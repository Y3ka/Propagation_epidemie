/**
 * @file statistique_test.c
 * @author Marc NGUYEN
 * @author Thomas LARDY
 * @brief Lancer des tests unitaires sur les fonctions de manipulation de Data.
 * @date 27 Mar 2019
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/statistique.h"

void lire_et_comparer(struct Data *data, const char *fichier_data);

/**
 * @brief Main pour tourner les tests unitaires.
 *
 * @return int Exit 0.
 */
int main(void) {
  printf("statistique_test:\n");
  printf("  creerData:\n");
  struct Data *data = creerData();
  assert(data->tours == 0);
  printf("    Data tours = 0: \x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques != NULL);
  printf("    Data liste_statistiques defined: \x1B[32mOK\x1B[0m\n");

  printf("  appendData:\n");
  struct Population *population = creerPopulation(7);
  struct Statistique stats = getStatistique(population);
  appendData(data, stats);
  assert(data->tours == 1);
  printf("    data->tours == 1: \x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques[0] != NULL);
  printf("    Data liste_statistiques[0] defined: \x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques[0]->nb_MALADE == 0);
  printf("    Data liste_statistiques[0]->nb_MALADE == 0: \x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques[0]->nb_SAIN == 49);
  printf("    Data liste_statistiques[0]->nb_SAIN == 49: \x1B[32mOK\x1B[0m\n");

  data->population_totale = getTaillePopulation(population);

  printf("  appendData 2 (MALADE+1, SAIN-1):\n");
  stats.nb_MALADE++;
  stats.nb_SAIN--;
  appendData(data, stats);
  assert(data->tours == 2);
  printf("    data->tours == 2: \x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques[1] != NULL);
  printf("    Data liste_statistiques[1] defined: \x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques[0] != data->liste_statistiques[1]);
  printf(
      "    Malloc liste_statistiques[0] != Malloc liste_statistiques[1]: "
      "\x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques[0]->nb_MALADE == 0);
  printf("    Data liste_statistiques[0]->nb_MALADE == 0: \x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques[0]->nb_SAIN == 49);
  printf("    Data liste_statistiques[0]->nb_SAIN == 49: \x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques[1]->nb_MALADE == 1);
  printf("    Data liste_statistiques[1]->nb_MALADE == 1: \x1B[32mOK\x1B[0m\n");
  assert(data->liste_statistiques[1]->nb_SAIN == 48);
  printf("    Data liste_statistiques[1]->nb_SAIN == 48: \x1B[32mOK\x1B[0m\n");

  printf("  deriver:\n");
  struct Data *data_derivee = deriver(data);
  assert(data_derivee->population_totale == 49);
  printf("    population_totale ne change pas: \x1B[32mOK\x1B[0m\n");
  assert(data_derivee->tours == 1);
  printf("    data_derivee->tours == 1: \x1B[32mOK\x1B[0m\n");
  assert(data_derivee->liste_statistiques[0]->nb_MALADE == 1);
  printf(
      "    Data' liste_statistiques[0]->nb_MALADE == 1: \x1B[32mOK\x1B[0m\n");
  assert(data_derivee->liste_statistiques[0]->nb_SAIN == -1);
  printf("    Data' liste_statistiques[0]->nb_SAIN == -1: \x1B[32mOK\x1B[0m\n");

  printf("  exporter:\n");
  exporter(data, "data_test.txt");
  printf("    Exporter Data: \x1B[32mOK\x1B[0m\n");
  exporter(data_derivee, "data_derivee_test.txt");
  printf("    Exporter Data dérivée: \x1B[32mOK\x1B[0m\n");
  lire_et_comparer(data, "data_test.txt");
  lire_et_comparer(data_derivee, "data_derivee_test.txt");
  appendData(data, stats);
  appendData(data, stats);
  appendData(data, stats);
  appendData(data, stats);
  appendData(data, stats);
  appendData(data, stats);
  appendData(data, stats);
  appendData(data, stats);
  appendData(data, stats);
  graphique(data, "graphique.txt", data->population_totale, data->tours);
  printf("    Graphique: \x1B[33mVOIR MANUELLEMENT\x1B[0m\n");
  graphique(data, "graphique_div2.txt", data->population_totale,
            data->tours / 2);
  printf("    Graphique div2: \x1B[33mVOIR MANUELLEMENT\x1B[0m\n");
  graphique(data, "graphique_90.txt", data->population_totale, 90);
  printf("    Graphique 90: \x1B[33mVOIR MANUELLEMENT\x1B[0m\n");
  tableau(data, "tableau.txt");
  printf("    Tableau de bord: \x1B[33mVOIR MANUELLEMENT\x1B[0m\n");
  return 0;
}

/**
 * @brief Lit et valide entre la base de donnée et les données exportées.
 *
 * @param data Base de donnée
 * @param fichier_data Nom du fichier à comparer.
 */
void lire_et_comparer(struct Data *data, const char *fichier_data) {
  FILE *file = fopen(fichier_data, "r");
  if (!file) {
    printf("Erreur: Le fichier n'a pas pu être ouvert.\n");
    exit(1);
  }
  unsigned long tour;   // Tour t
  unsigned long tours;  // Nombre de tours
  unsigned long pop_tot;
  long nb_IMMUNISE;
  long nb_SAIN;
  long nb_MORT;
  long nb_MALADE;
  long nb_INCUBE;
  long nb_VACCINE;

  fscanf(file, "%lu\n", &pop_tot);
  assert(data->population_totale == pop_tot);
  fscanf(file, "%lu\n", &tours);
  assert(data->tours == tours);

  while (fscanf(file, "%lu %li %li %li %li %li %li\n", &tour, &nb_IMMUNISE,
                &nb_SAIN, &nb_MORT, &nb_MALADE, &nb_INCUBE,
                &nb_VACCINE) != EOF) {
    assert(data->liste_statistiques[tour]->nb_SAIN == nb_SAIN);
    assert(data->liste_statistiques[tour]->nb_IMMUNISE == nb_IMMUNISE);
    assert(data->liste_statistiques[tour]->nb_MORT == nb_MORT);
    assert(data->liste_statistiques[tour]->nb_MALADE == nb_MALADE);
    assert(data->liste_statistiques[tour]->nb_INCUBE == nb_INCUBE);
    assert(data->liste_statistiques[tour]->nb_VACCINE == nb_VACCINE);
  }
  printf("    Validation %s: \x1B[32mOK\x1B[0m\n", fichier_data);
}