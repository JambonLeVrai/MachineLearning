#include "NEAT.h"

NEAT::NEAT(size_t entrees, size_t sorties, size_t _nb_genomes, size_t nb_connexions_initiales)
{
    nb_genomes = _nb_genomes;
    // On commence par g�n�rer tous les g�nes correspondant aux sorties
    vector<GeneNoeud> genes_sortie;
    for (size_t i = 0; i < sorties; i++)
        genes_sortie.push_back(*new GeneNoeud(entrees + i)); // Les id des g�nes de noeuds de sortie doivent suivre les id des noeuds en entr�e

    // On g�n�re le g�nome "vide" initial, qui ne contient que les noeuds d'entr�e et de sortie
    Genome* g0 = new Genome(entrees, vector<GeneConnexion>(), vector<GeneNoeud>(), genes_sortie);

    // On g�n�re un certain nombre de g�nomes initiaux en faisant muter le g�nome vide
    queue<Genome*> genomes_initiaux;
    for (size_t i = 0; i < nb_genomes; i++) {
        // On d�marre forc�ment avec une connexion
        Genome* g = new Genome(g0, Mutation::Type::AjouteConnexion);
        for (size_t i = 0; i < nb_connexions_initiales; i++)
            g->mute(Mutation::Type::AjouteConnexion);
        /*// Une chance sur deux d'ajouter un noeud � la connexion
        if (Outils::aleatoire_i<size_t>(0, 1))
            g->mute(Mutation::Type::AjouteNoeud);
        // Une chance sur deux d'ajouter une seconde connexion
        if (Outils::aleatoire_i<size_t>(0, 1))
            g->mute(Mutation::Type::AjouteConnexion);*/

        genomes_initiaux.push(g);
    }

    // On trie maintenant les g�nomes initiaux en esp�ces
    while (!genomes_initiaux.empty()) {
        // On consid�re un g�nome
        Genome* g = genomes_initiaux.front();

        if (especes.empty()) {
            // Le premier g�nome cr�� une nouvelle esp�ce
            especes.push_back(*new Espece(1, GenomeMesure(g, nullopt)));
        }
        else {
            // On cherche si le g�nome consid�r� correspond � une esp�ce
            bool trouve = false;
            for (auto it : especes) {
                if (g->distance(C1, C2, C3, it.front().first) < seuil_diff) {
                    // On a trouv� une esp�ce qui correspond � ce g�nome
                    it.push_back(*new GenomeMesure(g, nullopt));
                    trouve = true;
                    break;
                }
            }
            if (!trouve) {
                // Le g�nome ne correspond � aucune esp�ce, on cr�� une nouvelle esp�ce
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
    // On commence par �valuer tous les r�seaux qui ne l'ont pas encore �t�
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
