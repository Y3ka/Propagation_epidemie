#if !defined(STATISTIQUE_H)
#define STATISTIQUE_H

#include "population.h"

const char *FICHIER_DATA;

/**
 * @brief Structure contenant les chiffres intéressants.
 */
struct Statistique {
  /**
   * @brief Nombre de Personne IMMUNISE.
   */
  long nb_IMMUNISE;
  /**
   * @brief Nombre de Personne SAIN.
   */
  long nb_SAIN;
  /**
   * @brief Nombre de Personne MORT.
   */
  long nb_MORT;
  /**
   * @brief Nombre de Personne MALADE.
   */
  long nb_MALADE;
  /**
   * @brief Nombre de Personne INCUBE.
   */
  long nb_INCUBE;
  /**
   * @brief Nombre de Personne VACCINE.
   */
  long nb_VACCINE;
};

/**
 * @brief Base de données.
 */
struct Data {
  /**
   * @brief Population totale, égale à cote*cote.
   */
  unsigned long population_totale;
  /**
   * @brief Nombre de tours au total
   */
  unsigned long tours;
  /**
   * @brief Liste des statistiques
   *
   * Usage:
   * ```
   * liste_statistiques[tour]->donnée.
   * ```
   */
  struct Statistique **liste_statistiques;
};

struct Data *creerData(void);
struct Statistique getStatistique(struct Population *population);
void appendData(struct Data *data, struct Statistique stats);
void exporter(struct Data *data, const char *fichier_data);
struct Data *deriver(struct Data *data);
char **graphique(struct Data *data, const char *fichier_data,
                 unsigned long hauteur, unsigned long limite);
void tableau(struct Data *data, const char *fichier_tableau);

#endif  // STATISTIQUE_H
