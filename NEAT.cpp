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
            especes.push_back(*new Espece(1, GenomeMesure(*g)));
        }
        else {
            // On cherche si le g�nome consid�r� correspond � une esp�ce
            bool trouve = false;
            for (auto it : especes) {
                if (g->distance(C1, C2, C3, it.front().get_genome()) < seuil_diff) {
                    // On a trouv� une esp�ce qui correspond � ce g�nome
                    it.push_back(*new GenomeMesure(*g));
                    trouve = true;
                    break;
                }
            }
            if (!trouve) {
                // Le g�nome ne correspond � aucune esp�ce, on cr�� une nouvelle esp�ce
                especes.push_back(*new Espece(1, GenomeMesure(*g)));
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
    vector<vector<double>> aptitudes_ajustees;
    for (auto& it : especes) {
        vector<double> aptitudes_ajustees_espece;
        for (auto& it2 : it)
            if (!it2.get_mesure())
                it2.set_mesure(mesure_aptitude(it2.get_genome()));
        // On trie l'esp�ce... ou pas ?
        // sort(it.begin(), it.end());

        // On mesure l'aptitude ajust�e
        for(auto& it2 : it)
            aptitudes_ajustees_espece.push_back(*it2.get_mesure() / it.size());

        aptitudes_ajustees.push_back(aptitudes_ajustees_espece);
    }

    // On calcule l'aptitude totale, et l'aptitude individuelle de chaque esp�ce
    double somme_aptitude_totale = 0.0;
    vector<double> somme_aptitude_espece;
    for (auto& it : aptitudes_ajustees) {
        double somme_aptitude_espece_actuelle = 0.0;

        for (auto& it2 : it)
            somme_aptitude_espece_actuelle += it2;

        somme_aptitude_espece.push_back(somme_aptitude_espece_actuelle);
        somme_aptitude_totale += somme_aptitude_espece_actuelle;
    }

    // On en d�duit le nombre d'enfants par esp�ce
    for (auto& it : somme_aptitude_espece)
        it *= nb_genomes / somme_aptitude_totale;

    // Il reste alors � g�n�rer les enfants de chaque esp�ce
    vector<Espece> nouvelle_generation;
    for (size_t i = 0; i < especes.size(); i++) {
        Espece e;
        size_t nb_enfants = max((int)floor(somme_aptitude_espece[i]),1);

        // Le champion de chaque esp�ce est conserv� tel quel si l'esp�ce contient 5+ r�seaux
        if (especes[i].size() >= 5) {
            e.push_back(*max_element(especes[i].begin(), especes[i].end()));
            nb_enfants--;
        }

        // On g�n�re les enfants
        if (nb_enfants > 0) {
            // On g�n�re une table contenant les paires de parents possibles
            vector<pair<size_t, size_t>> paires_parents;
            for (size_t j = 0; j < especes[i].size(); j++)
                for (size_t k = j+1; k < especes[i].size(); k++)
                    paires_parents.push_back(pair<size_t, size_t>(j, k));
            // Puis on la m�lange
            shuffle(paires_parents.begin(), paires_parents.end(), default_random_engine(chrono::system_clock::now().time_since_epoch().count()));

            for (size_t j = 0; (double)j < (double)nb_enfants/4; j++) {
                // Un premier enfant par mutation
                GenomeMesure g(*especes[i][j].get_genome());
                g.get_genome()->mute(Outils::aleatoire_table<Mutation::Type>(Mutation::probas_mutations));
                // /!\ attention, ajouter un bout de code pour v�rifier la distance au repr�sentant du g�nome (le champion de la g�n�ration pr�c�dente) et potentiellement ajouter une nouvelle esp�ce
                e.push_back(g);
                // Puis trois enfants par paire de parents
                if (3 * j + 2 < paires_parents.size()) {
                    g = *new GenomeMesure(*new Genome(especes[i][paires_parents[j * 3].first].get_genome(), especes[i][paires_parents[j * 3].second].get_genome()));
                    e.push_back(g);
                    g = *new GenomeMesure(*new Genome(especes[i][paires_parents[j * 3 + 1].first].get_genome(), especes[i][paires_parents[j * 3 + 1].second].get_genome()));
                    e.push_back(g);
                    g = *new GenomeMesure(*new Genome(especes[i][paires_parents[j * 3 + 2].first].get_genome(), especes[i][paires_parents[j * 3 + 2].second].get_genome()));
                    e.push_back(g);
                }
                else if (3 * j + 2 < paires_parents.size()) {
                    g = *new GenomeMesure(*new Genome(especes[i][paires_parents[j * 3].first].get_genome(), especes[i][paires_parents[j * 3].second].get_genome()));
                    e.push_back(g);
                    g = *new GenomeMesure(*new Genome(especes[i][paires_parents[j * 3 + 1].first].get_genome(), especes[i][paires_parents[j * 3 + 1].second].get_genome()));
                    e.push_back(g);
                    // S'il n'y a pas assez de parents, on compl�te avec des mutations al�atoires d'organismes al�atoires
                    g = *new GenomeMesure(*especes[i][Outils::aleatoire_i<size_t>(0, especes[i].size()-1)].get_genome());
                    g.get_genome()->mute(Outils::aleatoire_table<Mutation::Type>(Mutation::probas_mutations));
                    e.push_back(g);
                }
                else if (3 * j + 1 < paires_parents.size()) {
                    g = *new GenomeMesure(*new Genome(especes[i][paires_parents[j * 3].first].get_genome(), especes[i][paires_parents[j * 3].second].get_genome()));
                    e.push_back(g);
                    // S'il n'y a pas assez de parents, on compl�te avec des mutations al�atoires d'organismes al�atoires
                    g = *new GenomeMesure(*especes[i][Outils::aleatoire_i<size_t>(0, especes[i].size() - 1)].get_genome());
                    g.get_genome()->mute(Outils::aleatoire_table<Mutation::Type>(Mutation::probas_mutations));
                    e.push_back(g);
                    g = *new GenomeMesure(*especes[i][Outils::aleatoire_i<size_t>(0, especes[i].size() - 1)].get_genome());
                    g.get_genome()->mute(Outils::aleatoire_table<Mutation::Type>(Mutation::probas_mutations));
                    e.push_back(g);
                }
                else {
                    // S'il n'y a pas assez de parents, on compl�te avec des mutations al�atoires d'organismes al�atoires
                    g = *new GenomeMesure(*especes[i][Outils::aleatoire_i<size_t>(0, especes[i].size() - 1)].get_genome());
                    g.get_genome()->mute(Outils::aleatoire_table<Mutation::Type>(Mutation::probas_mutations));
                    e.push_back(g);
                    g = *new GenomeMesure(*especes[i][Outils::aleatoire_i<size_t>(0, especes[i].size() - 1)].get_genome());
                    g.get_genome()->mute(Outils::aleatoire_table<Mutation::Type>(Mutation::probas_mutations));
                    e.push_back(g);
                    g = *new GenomeMesure(*especes[i][Outils::aleatoire_i<size_t>(0, especes[i].size() - 1)].get_genome());
                    g.get_genome()->mute(Outils::aleatoire_table<Mutation::Type>(Mutation::probas_mutations));
                    e.push_back(g);
                }
            }
        }
        nouvelle_generation.push_back(e);
    }

    // �liminer les esp�ces qui sont tr�s peu performantes

    // Du cleanup � r�aliser, des tr�s grosses fuites de m�moire ici
    // Potentiellement logger les r�seaux qui sont supprim�s ?

    especes = nouvelle_generation;
}

GenomeMesure::GenomeMesure(pair<Genome, optional<double>> _val) : val(_val)
{
}

GenomeMesure::GenomeMesure(Genome g, optional<double> m)
{
    val = *new pair<Genome, optional<double>>(g, m);
}

Genome* GenomeMesure::get_genome()
{
    return &val.first;
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
