#if !defined(JOUER_UN_TOUR_H)
#define JOUER_UN_TOUR_H

#include "population.h"

char zombiePresent(struct Population *population);
void jouerTour(struct Population *population, double beta, double gamma,
               double lambda, double chance_quarantaine,
               double chance_decouverte_vaccin, unsigned int duree_incube,
               int cordon_sanitaire, unsigned int duree_quarantaine);
void mettreEnQuarantaine(struct Personne ***grille_de_personnes,
                         unsigned long x, unsigned long y, unsigned long cote,
                         int cordon_sanitaire, int duree_quarantaine);
void mettreEnQuarantaineRecurse(struct Personne ***grille_de_personnes,
                                unsigned long x, unsigned long y,
                                unsigned long cote, int cordon_sanitaire,
                                int duree_quarantaine);

#endif  // JOUER_UN_TOUR_H
