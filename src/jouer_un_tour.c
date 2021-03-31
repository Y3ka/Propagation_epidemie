/**
 * @file jouer_un_tour.c
 *
 * @brief Lancer un tour.
 *
 * Fonctionnalités :
 * - Détecteur de zombie.
 * - jouerTour lancer un tour.
 * - Mettre en quarantaine
 *
 * Usage pour jouer un jeu complet:
 * ```
 * unsigned duree_incube = 4;
 * double beta = 0.5;                        // MALADE -> MORT
 * double gamma = 0.1;                       // MALADE -> IMMUNISE
 * double lambda = 1.0;                      // SAIN -> MALADE
 * double chance_quarantaine = 0.1;          // duree_quarantaine
 * double chance_decouverte_vaccin = 0.001;  // MALADE -> VACINE par découverte
 * unsigned long tour_max = 100;
 * unsigned duree_quarantaine = 20;  // Généralement très grand au temps d'incub
 * int cordon_sanitaire = 10;  // Généralement égal au temps d'incubation
 *
 * struct Population* population = creerPopulation(7);
 *
 * patient_zero(population, 0, 0);
 * for(int i = 0; i < tour_max && zombiePresent(population); i++) {
 *   jouerTour(population, beta, gamma, lambda, chance_quarantaine,
 *             chance_decouverte_vaccin, duree_incube, cordon_sanitaire,
 *             duree_quarantaine);
 * }
 * ```
 *
 * @author Marc NGUYEN
 * @author Thomas LARDY
 * @date 28 Mar 2019
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "jouer_un_tour.h"

/**
 * @brief Renvoie true s'il y a encore au moins un zombie dans la population,
 * false dans le cas contraire.
 *
 * Usage:
 * ```
 * struct Population* population = creerPopulation(7);
 * char boolean = zombiePresent(population);  // -> false;
 * ```
 *
 * @param population Grille représentant la population.
 * @return char Booléen, si zombies toujours présents.
 */
char zombiePresent(struct Population *population) {
  unsigned long cote = population->cote;
  for (unsigned long i = 0; i < cote; i++) {
    for (unsigned long j = 0; j < cote; j++) {
      if (population->grille_de_personnes[i][j]->state == MALADE ||
          population->grille_de_personnes[i][j]->state == INCUBE)
        return 1;
    }
  }
  return 0;
}

/**
 * @brief Faire passer la population->grille_de_personnes du temps t au temps
 * t+1.
 *
 * Usage pour jouer un jeu complet:
 * ```
 * unsigned long cote = 10;
 * unsigned long hauteur = 20;
 * unsigned long limite = 80;
 * unsigned duree_incube = 4;
 * double beta = 0.5;                        // MALADE -> MORT
 * double gamma = 0.1;                       // MALADE -> IMMUNISE
 * double lambda = 1.0;                      // SAIN -> MALADE
 * double chance_quarantaine = 0.1;          // duree_quarantaine
 * double chance_decouverte_vaccin = 0.001;  // MALADE -> VACINE par découverte
 * char* file_graph = "graphique.txt";
 * char* file_data = "data.txt";
 * char* file_tableau = "tableau de bord.txt";
 * unsigned long tour_max = 100;
 * unsigned duree_quarantaine = 20;  // Généralement très grand au temps d'incub
 * int cordon_sanitaire = 10;  // Généralement égal au temps d'incubation
 * struct Population* population = creerPopulation(7);
 * patient_zero(population, 0, 0);
 * for(int i = 0; i < tour_max && zombiePresent(population); i++) {
 *   jouerTour(population, beta, gamma, lambda, chance_quarantaine,
 *             chance_decouverte_vaccin, duree_incube, cordon_sanitaire,
 *             duree_quarantaine);
 * }
 * ```
 *
 * @param population Grille au temps t.
 * @param beta Probabilité d'un individu infecté de mourir.
 * @param gamma Probabilité d'un individu infecté de guérir.
 * @param lambda Probabilité d'un individu sain d'être contaminé par un voisin.
 * @param chance_quarantaine Probabilité de déclancher une quarantaine.
 * @param chance_decouverte_vaccin Probabilité de découverte d'un vaccin.
 * @param duree_incube Nombre de tours durant lesquels la personne est incubée.
 * @param cordon_sanitaire Personnes incubé(?) inclus dans la quarantaine.
 * @param duree_quarantaine Durée de la quarantaine.
 */
void jouerTour(struct Population *population, double beta, double gamma,
               double lambda, double chance_quarantaine,
               double chance_decouverte_vaccin, unsigned int duree_incube,
               int cordon_sanitaire, unsigned int duree_quarantaine) {
  const long unsigned cote = population->cote;
  // On va modifier une population->grille_de_personnes tampon pour ne pas
  // modifier la population->grille_de_personnes originale en cours de
  // traitement.
  struct Personne ***grille_tampon = clonerGrillePopulation(population);
  double nb_aleatoire;
  int voisin = 0;  // compteur de voisins.

  // Alias
  struct Personne ***grille = population->grille_de_personnes;

  // Pour chaque case
  for (long unsigned i = 0; i < cote; i++) {
    for (long unsigned j = 0; j < cote; j++) {
      // Réduire la durée de quarantaine
      if (grille[i][j]->duree_quarantaine)
        grille_tampon[i][j]->duree_quarantaine--;

      switch (grille[i][j]->state) {
        case SAIN:
          // Vaccination par découverte du vaccin
          nb_aleatoire = (double)rand() / RAND_MAX;
          if (nb_aleatoire < chance_decouverte_vaccin) {
            grille_tampon[i][j]->state = VACCINE;
            break;
          }

          // Vaccination par propagation
          if ((j > 0 && grille[i][j - 1]->state == VACCINE) ||
              (j < cote - 1 && grille[i][j + 1]->state == VACCINE) ||
              (i > 0 && grille[i - 1][j]->state == VACCINE) ||
              (i < cote - 1 && grille[i + 1][j]->state == VACCINE)) {
            grille_tampon[i][j]->state = VACCINE;
            break;
          }

          // Infection
          // La personne est MALADE s'il remplis :
          // 1. Ils sont ensemble (en quarantaine ou libre)
          // 2. Le/les voisins est INCUBE ou MALADE
          // 3. Il n'a pas de chance.
          voisin = 0;
          if (j > 0)
            voisin += (!grille[i][j - 1]->duree_quarantaine ==
                       !grille[i][j]->duree_quarantaine) &&  // not XOR
                      (grille[i][j - 1]->state == MALADE ||
                       grille[i][j - 1]->state == INCUBE);
          if (j < cote - 1)
            voisin += (!grille[i][j + 1]->duree_quarantaine ==
                       !grille[i][j]->duree_quarantaine) &&  // not XOR
                      (grille[i][j + 1]->state == MALADE ||
                       grille[i][j + 1]->state == INCUBE);
          if (i > 0)
            voisin += (!grille[i - 1][j]->duree_quarantaine ==
                       !grille[i][j]->duree_quarantaine) &&  // not XOR
                      (grille[i - 1][j]->state == MALADE ||
                       grille[i - 1][j]->state == INCUBE);
          if (i < cote - 1)
            voisin += (!grille[i + 1][j]->duree_quarantaine ==
                       !grille[i][j]->duree_quarantaine) &&  // not XOR
                      (grille[i + 1][j]->state == MALADE ||
                       grille[i + 1][j]->state == INCUBE);
          // On applique la probabilité que l'individu tombe malade pour chaque
          // voisin.
          while (voisin > 0 && grille_tampon[i][j]->state != INCUBE) {
            nb_aleatoire = (double)rand() / RAND_MAX;
            if (nb_aleatoire < lambda) {
              grille_tampon[i][j]->state = INCUBE;
              grille_tampon[i][j]->duree_incube = duree_incube;
              break;  // On sort de la boucle pour optimiser
            }
            voisin--;
          }
          break;

        case MALADE:
          // Mettre en QUARANTAINE ainsi que ses voisins
          nb_aleatoire = (double)rand() / RAND_MAX;
          if (nb_aleatoire < chance_quarantaine &&
              !grille_tampon[i][j]->duree_quarantaine) {
            mettreEnQuarantaine(grille_tampon, i, j, cote, cordon_sanitaire,
                                duree_quarantaine);
          }

          // S'il est MALADE, soit IMMUNISE, soit MORT, soit rien
          nb_aleatoire = (double)rand() / RAND_MAX;
          if (nb_aleatoire < beta) {
            grille_tampon[i][j]->state = MORT;
            break;
          }

          nb_aleatoire = (double)rand() / RAND_MAX;
          if (nb_aleatoire < gamma) {
            grille_tampon[i][j]->state = IMMUNISE;
            break;
          }
          break;

        case INCUBE:
          // S'il est incubé, on diminue le temps incubé de 1 et il devient
          // malade si t = 0
          if (grille[i][j]->duree_incube == 0)
            grille_tampon[i][j]->state = MALADE;
          else
            grille_tampon[i][j]->duree_incube--;
          break;

        case IMMUNISE:
          // Vaccination par découverte du vaccin
          nb_aleatoire = (double)rand() / RAND_MAX;
          if (nb_aleatoire < chance_decouverte_vaccin) {
            grille_tampon[i][j]->state = VACCINE;
            break;
          }

          // Vaccination par propagation
          if ((j > 0 && grille[i][j - 1]->state == VACCINE) ||
              (j < cote - 1 && grille[i][j + 1]->state == VACCINE) ||
              (i > 0 && grille[i - 1][j]->state == VACCINE) ||
              (i < cote - 1 && grille[i + 1][j]->state == VACCINE))
            grille_tampon[i][j]->state = VACCINE;
          break;

        default:
          break;
      }
    }
  }

  population->grille_de_personnes = grille_tampon;  // Appliquer les changements
}

/**
 * @brief Partie récusive de mettreEnQuarantaine.
 *
 * @param grille_de_personnes Grille de personnes au temps t.
 * @param x
 * @param y
 * @param cote Longueur d'un côté de la grille de personnes
 * @param cordon_sanitaire Distance entre un malade et la personne.
 * @param duree_quarantaine Durée de la quarantaine.
 */
void mettreEnQuarantaineRecurse(struct Personne ***grille_de_personnes,
                                unsigned long x, unsigned long y,
                                unsigned long cote, int cordon_sanitaire,
                                int duree_quarantaine) {
  grille_de_personnes[x][y]->duree_quarantaine = duree_quarantaine;
  grille_de_personnes[x][y]->cordon_sanitaire = cordon_sanitaire;

  // Mettre en quarantaine les personnes adjacentes
  // NORD
  if (x > 0 &&
      grille_de_personnes[x - 1][y]->cordon_sanitaire < cordon_sanitaire) {
    // Si la personne est MALADE ou MORT, on le met en quarantaine
    if (grille_de_personnes[x - 1][y]->state == MALADE ||
        grille_de_personnes[x - 1][y]->state == MORT)
      mettreEnQuarantaineRecurse(grille_de_personnes, x - 1, y, cote,
                                 cordon_sanitaire, duree_quarantaine);
    // La personne est peut-être incubé, on le met dans le cordon_sanitaire
    else if (cordon_sanitaire > 0)
      mettreEnQuarantaineRecurse(grille_de_personnes, x - 1, y, cote,
                                 cordon_sanitaire - 1, duree_quarantaine);
  }
  // SUD
  if (x < cote - 1 &&
      grille_de_personnes[x + 1][y]->cordon_sanitaire < cordon_sanitaire) {
    if (grille_de_personnes[x + 1][y]->state == MALADE ||
        grille_de_personnes[x + 1][y]->state == MORT)
      mettreEnQuarantaineRecurse(grille_de_personnes, x + 1, y, cote,
                                 cordon_sanitaire, duree_quarantaine);
    else if (cordon_sanitaire > 0)
      mettreEnQuarantaineRecurse(grille_de_personnes, x + 1, y, cote,
                                 cordon_sanitaire - 1, duree_quarantaine);
  }
  // OUEST
  if (y > 0 &&
      grille_de_personnes[x][y - 1]->cordon_sanitaire < cordon_sanitaire) {
    if (grille_de_personnes[x][y - 1]->state == MALADE ||
        grille_de_personnes[x][y - 1]->state == MORT)
      mettreEnQuarantaineRecurse(grille_de_personnes, x, y - 1, cote,
                                 cordon_sanitaire, duree_quarantaine);
    else if (cordon_sanitaire > 0)
      mettreEnQuarantaineRecurse(grille_de_personnes, x, y - 1, cote,
                                 cordon_sanitaire - 1, duree_quarantaine);
  }
  // EST
  if (y < cote - 1 &&
      grille_de_personnes[x][y + 1]->cordon_sanitaire < cordon_sanitaire) {
    if (grille_de_personnes[x][y + 1]->state == MALADE ||
        grille_de_personnes[x][y + 1]->state == MORT)
      mettreEnQuarantaineRecurse(grille_de_personnes, x, y + 1, cote,
                                 cordon_sanitaire, duree_quarantaine);
    else if (cordon_sanitaire > 0)
      mettreEnQuarantaineRecurse(grille_de_personnes, x, y + 1, cote,
                                 cordon_sanitaire - 1, duree_quarantaine);
  }
}

/**
 * @brief Appliquer la mise en quarantaine sur un secteur.
 *
 * NOTE: Si il s'agissait du DFS, cordon_sanitaire représente le marquage.
 *
 * La quarantaine suit ces règles :
 * - Toute personne MALADE entrant dans le cordon passe en quarantaine
 * - Toute personne non MALADE (MORT inclus) entre dans le cordon, mais réduit
 son influence
 *
 * Usage:
 * ```
 * creerPopulation(7);
 * mettreEnQuarantaine(struct Personne ***grille_de_personnes,
                         unsigned long x, unsigned long y, unsigned long cote,
                         int cordon_sanitaire, unsigned duree_quarantaine)
 * ```
 *
 * @param grille_de_personnes Grille de personnes au temps t.
 * @param x
 * @param y
 * @param cote Longueur d'un côté de la grille de personnes
 * @param cordon_sanitaire Distance entre un malade et la personne.
 * @param duree_quarantaine Durée de la quarantaine.
 */
void mettreEnQuarantaine(struct Personne ***grille_de_personnes,
                         unsigned long x, unsigned long y, unsigned long cote,
                         int cordon_sanitaire, int duree_quarantaine) {
  for (unsigned long i = 0; i < cote; i++)
    for (unsigned long j = 0; j < cote; j++)
      grille_de_personnes[i][j]->cordon_sanitaire = 0;  // Marquage mise à zero

  mettreEnQuarantaineRecurse(grille_de_personnes, x, y, cote, cordon_sanitaire,
                             duree_quarantaine);
}