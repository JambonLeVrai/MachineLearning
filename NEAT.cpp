#include "NEAT.h"

NEAT::NEAT(size_t entrees, size_t sorties, size_t _nb_genomes, size_t nb_connexions_initiales)
{
    nb_genomes = _nb_genomes;
    // On commence par générer tous les gènes correspondant aux sorties
    vector<GeneNoeud> genes_sortie;
    for (size_t i = 0; i < sorties; i++)
        genes_sortie.push_back(*new GeneNoeud(entrees + i)); // Les id des gènes de noeuds de sortie doivent suivre les id des noeuds en entrée

    // On génère le génome "vide" initial, qui ne contient que les noeuds d'entrée et de sortie
    Genome* g0 = new Genome(entrees, vector<GeneConnexion>(), vector<GeneNoeud>(), genes_sortie);

    // On génère un certain nombre de génomes initiaux en faisant muter le génome vide
    queue<Genome*> genomes_initiaux;
    for (size_t i = 0; i < nb_genomes; i++) {
        // On démarre forcément avec une connexion
        Genome* g = new Genome(g0, Mutation::Type::AjouteConnexion);
        for (size_t i = 0; i < nb_connexions_initiales; i++)
            g->mute(Mutation::Type::AjouteConnexion);
        /*// Une chance sur deux d'ajouter un noeud à la connexion
        if (Outils::aleatoire_i<size_t>(0, 1))
            g->mute(Mutation::Type::AjouteNoeud);
        // Une chance sur deux d'ajouter une seconde connexion
        if (Outils::aleatoire_i<size_t>(0, 1))
            g->mute(Mutation::Type::AjouteConnexion);*/

        genomes_initiaux.push(g);
    }

    // On trie maintenant les génomes initiaux en espèces
    while (!genomes_initiaux.empty()) {
        // On considère un génome
        Genome* g = genomes_initiaux.front();

        if (especes.empty()) {
            // Le premier génome créé une nouvelle espèce
            especes.push_back(*new Espece(1, GenomeMesure(g, nullopt)));
        }
        else {
            // On cherche si le génome considéré correspond à une espèce
            bool trouve = false;
            for (auto it : especes) {
                if (g->distance(C1, C2, C3, it.front().first) < seuil_diff) {
                    // On a trouvé une espèce qui correspond à ce génome
                    it.push_back(*new GenomeMesure(g, nullopt));
                    trouve = true;
                    break;
                }
            }
            if (!trouve) {
                // Le génome ne correspond à aucune espèce, on créé une nouvelle espèce
                especes.push_back(*new Espece(1, GenomeMesure(g, nullopt)));
            }
        }

        // On passe au suivant
        genomes_initiaux.pop();
    }
}

void NEAT::set_coeffs(double _c1, double _c2, double _c3, double _seuil_diff)
{
    C1 = _c1;
    C2 = _c2;
    C3 = _c3;
    seuil_diff = _seuil_diff;
}

void NEAT::evolue()
{
    // On commence par évaluer tous les réseaux qui ne l'ont pas encore été
    for (auto& it : especes) {
        for (auto& it2 : it) {
            if (!it2.second) {
                it2.second = mesure_aptitude(it2.first);
            }
        }
    }

    // On trie
}

GenomeMesure::GenomeMesure(pair<Genome*, optional<double>> _val) : val(_val)
{
}

GenomeMesure::GenomeMesure(Genome* g, optional<double> m)
{
    val = *new pair<Genome*, optional<double>>(g, m);
}

Genome* GenomeMesure::get_genome()
{
    return val.first;
}

optional<double> GenomeMesure::get_mesure()
{
    return val.second;
}

void GenomeMesure::set_mesure(double v)
{
    val.second = v;
}

bool GenomeMesure::operator<(const GenomeMesure& g2)
{
    return val.second < g2.val.second;
}
