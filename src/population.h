#if !defined(POPULATION_H)
#define POPULATION_H

/**
 * @brief Etats pouvant être une Personne.
 */
enum State { SAIN, MALADE, MORT, IMMUNISE, INCUBE, VACCINE };

/**
 * @brief Personne ayant un State, une duree_incube et une duree_quarantaine.
 */
struct Personne {
  /**
   * @brief Etat de la Personne.
   */
  enum State state;
  /**
   * @brief Durée d'une incubation (aucun symptôme mais peut infecter).
   */
  int duree_incube;
  /**
   * @brief Durée d'une quarantaine (secteur bloquant l'infection vers l'ext.).
   */
  int duree_quarantaine;
  /**
   * @brief Mesure la distance entre le MALADE et la personne.
   */
  int cordon_sanitaire;
};

/**
 * @brief Structure Population.
 */
struct Population {
  /**
   * @brief Tableau cote*cote contenant une struct Personne*.
   */
  struct Personne ***grille_de_personnes;
  /**
   * @brief Longueur d'un côté de la grille.
   */
  unsigned long cote;
};

struct Population *creerPopulation(const unsigned long cote);
struct Personne ***clonerGrillePopulation(struct Population *population_source);
void afficherGrillePopulation(struct Population *population);
void patient_zero(struct Population *population, unsigned long x,
                  unsigned long y);
unsigned long getTaillePopulation(struct Population *population);

#endif  // POPULATION_H
