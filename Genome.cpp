#include "Genome.h"

GeneConnexion::GeneConnexion(size_t _entree, size_t _sortie, double _poids, bool _actif, size_t _innovation) :
    entree(_entree), sortie(_sortie), poids(_poids), actif(_actif), innovation(_innovation)
{
}

size_t GeneConnexion::get_entree()
{
    return entree;
}

size_t GeneConnexion::get_sortie()
{
    return sortie;
}

double GeneConnexion::get_poids()
{
    return poids;
}

bool GeneConnexion::get_actif()
{
    return actif;
}

int GeneConnexion::get_innovation()
{
    return innovation;
}

void GeneConnexion::delta_poids(double delta)
{
    poids += delta;
}

void GeneConnexion::desactive()
{
    actif = false;
}

void GeneConnexion::active()
{
    actif = true;
}

bool GeneConnexion::operator<(const GeneConnexion& other)
{
    if (sortie < other.sortie)
        return true;
    else if (entree < other.entree)
        return true;
    return false;
}

GeneNoeud::GeneNoeud(size_t _id) : id(_id)
{
}

size_t GeneNoeud::get_id()
{
    return id;
}

Genome::Genome()
{
}

Genome::Genome(size_t _entrees, vector<GeneConnexion> _connexions, vector<GeneNoeud> _noeuds, vector<GeneNoeud> _sorties) : connexions(_connexions), noeuds(_noeuds), sorties(_sorties)
{
    for (int i = 0; i < _entrees; i++)
        entrees.push_back(*new GeneNoeud(i));
}

Genome::Genome(Genome* g, Mutation::Type mutation)
{
    connexions = g->connexions;
    noeuds = g->noeuds;
    entrees = g->entrees;
    sorties = g->sorties;
    innovation = g->innovation;
    nombre_connexions_desac = g->nombre_connexions_desac;

    mute(mutation);
}

Genome::Genome(Genome* p1, Genome* p2)
{
}

void Genome::genere_reseau()
{
    map<size_t, Neurone*> neurones_id;
    vector<NeuroneEntree*> entrees;
    vector<Neurone*> neurones;
    vector<Neurone*> sorties;
    vector<Axone*> axones;

    size_t id = 0;
    for (auto it : entrees) {
        Neurone* n = new NeuroneEntree();
        entrees.push_back((NeuroneEntree*)n);
        neurones_id.insert(pair<size_t, Neurone*>(id, n));
        id++;
    }

    for (auto it : sorties) {
        Neurone* n = new Neurone();
        sorties.push_back(n);
        neurones_id.insert(pair<size_t, Neurone*>(id, n));
        id++;
    }

    for (auto& it : noeuds) {
        Neurone* n;
        neurones_id.insert(pair<size_t, Neurone*>(it.get_id(), n));
    }

    for (auto& it : connexions) {
        if (it.get_actif()) {
            Neurone* e, * s;
            e = neurones_id[it.get_entree()];
            s = neurones_id[it.get_sortie()];
            Axone* a = new Axone(e, s, it.get_poids());
            e->aj_sortie(a);
            s->aj_entree(a);
            axones.push_back(a);
        }
    }

    reseau_associe = new Reseau(entrees, sorties, neurones, axones);
}

vector<GeneConnexion> Genome::get_connexions()
{
    return connexions;
}

vector<GeneNoeud> Genome::get_noeuds()
{
    return noeuds;
}

Reseau* Genome::get_reseau()
{
    if (!reseau_associe)
        genere_reseau();
    return reseau_associe;
}

pair<size_t, size_t> get_connexion(size_t id, size_t max) {
    return *new pair<size_t, size_t>(id % max, floor(id / max));
}

size_t get_connexion(pair<size_t, size_t> paire, size_t max) {
    return paire.first + paire.second * max;
}

void Genome::mutation_ajoute_connexion()
{
    vector<size_t> connexions_interdites;
    size_t max = noeuds.size() - 1;

    // On ne permet pas de neurones connectés à eux-mêmes
    for (int i = 0; i <= max; i++)
        connexions_interdites.push_back(get_connexion(*new pair<size_t, size_t>(i, i), max));

    // On ne peut pas dupliquer une connexion, même désactivée, et la connexion ne peut pas exister dans les deux sens
    for (auto& it : connexions) {
        connexions_interdites.push_back(get_connexion(*new pair<size_t, size_t>(it.get_entree(), it.get_sortie()), max));
        connexions_interdites.push_back(get_connexion(*new pair<size_t, size_t>(it.get_sortie(), it.get_entree()), max));
    }

    // On ne permet pas de connecter des neurones d'entrée entre eux
    for (size_t i = 0; i < entrees.size(); i++) {
        for (size_t j = i + 1; j < entrees.size(); j++)
            connexions_interdites.push_back(get_connexion(*new pair<size_t, size_t>(entrees[i].get_id(), entrees[j].get_id()), max));
    }
    // On ne permet pas non plus de connecter les neurones de sortie entre eux
    for (size_t i = 0; i < entrees.size(); i++) {
        for (size_t j = i + 1; j < entrees.size(); j++)
            connexions_interdites.push_back(get_connexion(*new pair<size_t, size_t>(sorties[i].get_id(), sorties[j].get_id()), max));
    }

    if (connexions_interdites.size() == noeuds.size() * noeuds.size())
        throw ExceptionPasDeConnexionPossible();

    pair<size_t, size_t> connexion_ES = get_connexion(Outils::aleatoire_avec_exclusion(connexions_interdites, 0, max * max), max);

    GeneConnexion nouvelle_connexion(connexion_ES.first, connexion_ES.second, Outils::aleatoire_f(MIN_POIDS_ALEATOIRE, MAX_POIDS_ALEATOIRE), true, innovation);

    connexions.push_back(nouvelle_connexion);

    innovation++;
}

void Genome::mutation_ajoute_noeud()
{
    int max = connexions.size() - 1 - nombre_connexions_desac;
    if (max < 0)
        throw ExceptionAucuneConnexion();

    // On choisit une connexion sur laquelle introduire un nouveau noeud, par référence vers les connexions disponibles
    size_t ref = Outils::aleatoire_i<size_t>(0, max);
    size_t shift = 0;
    size_t result = 0;
    auto it = connexions.begin();
    while (ref > 0) {
        while (!(*it).get_actif())
            ++it;

        ref--;
    }
    // On désactive la connexion en premier lieu
    (*it).desactive();
    nombre_connexions_desac++; // On n'oublie surtout pas d'incrémenter le nombre de connexions désactivées

    // On calcule le nouvel identifiant du neurone, et on l'ajoute
    size_t nouvel_id = sorties.back().get_id();
    if (noeuds.size() > 0)
        nouvel_id = noeuds.back().get_id() + 1;
    noeuds.push_back(*new GeneNoeud(nouvel_id));
    // Il ne reste plus qu'à insérer les deux nouvelles connexions
    connexions.push_back(*new GeneConnexion((*it).get_entree(), nouvel_id, Outils::aleatoire_f(MIN_POIDS_ALEATOIRE, MAX_POIDS_ALEATOIRE), true, innovation));
    innovation++;
    connexions.push_back(*new GeneConnexion(nouvel_id, (*it).get_sortie(), Outils::aleatoire_f(MIN_POIDS_ALEATOIRE, MAX_POIDS_ALEATOIRE), true, innovation));
    innovation++;
}

void Genome::mutation_modifie_poids()
{
    size_t n = connexions.size();
    if (n == 0)
        throw ExceptionAucuneConnexion();

    size_t c = Outils::aleatoire_i<size_t>(0, n - 1);
    connexions[c].delta_poids(Outils::aleatoire_f(MIN_DELTA_POIDS, MAX_DELTA_POIDS));
}

void Genome::mutation_supprime_noeud()
{
    if (noeuds.size() == 0)
        throw ExceptionAucunNoeud();
    size_t c = Outils::aleatoire_i<size_t>(0, noeuds.size() - 1);
    size_t id = noeuds[c].get_id();
    noeuds.erase(noeuds.begin() + c);
    for (auto it = connexions.begin(); it != connexions.end(); ++it) {
        if ((*it).get_entree() == id || (*it).get_sortie() == id) {
            if (!(*it).get_actif())
                nombre_connexions_desac--;
            connexions.erase(it);
        }
    }
}

void Genome::mutation_active_connexion()
{
    vector<size_t> connexions_desactivees;
    for (size_t i = 0; i < connexions.size(); i++) {
        if (!connexions[i].get_actif())
            connexions_desactivees.push_back(i);
    }
    size_t n = connexions_desactivees.size();
    if (n == 0)
        throw ExceptionToutesConnexionsActives();
    else {
        size_t c = Outils::aleatoire_i<size_t>(0, n - 1);
        connexions[c].active();
        nombre_connexions_desac--;
    }
}

void Genome::mutation_supprime_connexion()
{
    size_t n = connexions.size();
    if (n == 0)
        throw ExceptionAucuneConnexion();
    else {
        size_t c = Outils::aleatoire_i<size_t>(0, n - 1);
        if (!connexions[c].get_actif())
            nombre_connexions_desac--;

        connexions.erase(connexions.begin() + c);
    }
}

void Genome::mute(Mutation::Type mutation)
{
    switch (mutation) {
    case Mutation::Type::ActiveConnexion:
        try {
            mutation_active_connexion();
        }
        catch (const ExceptionToutesConnexionsActives& e) {
            // Aucune connexion inactive, on ajoute une connexion à la place
            try {
                mutation_ajoute_connexion();
            }
            catch (const ExceptionPasDeNoeudDisponible& e) {
                // Impossible d'ajouter une connexion, elles existent toutes (et sont toutes actives)
                try {
                    mutation_ajoute_noeud();
                }
                catch (const ExceptionAucuneConnexion& e) {
                    // Impossible de couper une connexion en deux, ce qui ne devrait jamais arriver (puisque toutes les connexions sont disponibles)
                    cerr << "Erreur magique...?" << endl;
                }
            }
        }
        break;
    case Mutation::Type::AjouteConnexion:
        try {
            mutation_ajoute_connexion();
        }
        catch (const ExceptionPasDeNoeudDisponible& e) {
            // Impossible d'ajouter une connexion, elles existent toutes
            try {
                mutation_ajoute_noeud();
            }
            catch (const ExceptionAucuneConnexion& e) {
                // Impossible d'ajouter un noeud sur une connexion, ce qui ne devrait pas pouvoir arriver si toutes les connexions existent
                cerr << "Erreur magique...?" << endl;
            }
        }
        break;
    case Mutation::Type::AjouteNoeud:
        try {
            mutation_ajoute_noeud();
        }
        catch (const ExceptionAucuneConnexion& e) {
            // Aucune connexion n'est disponible, on en ajoute donc
            try {
                mutation_ajoute_connexion();
            }
            catch (const ExceptionPasDeNoeudDisponible& e) {
                // Ne devrait jamais arriver
                cerr << "Erreur magique...?" << endl;
            }
        }
        break;
    case Mutation::Type::ModifiePoids:
        mutation_modifie_poids();
        break;
    case Mutation::Type::SupprimeConnexion:
        try {
            mutation_supprime_connexion();
        }
        catch (const ExceptionAucuneConnexion& e) {
            // Aucune connexion ne peut être supprimée, on supprime un noeud à la place
            try {
                mutation_supprime_noeud();
            }
            catch (const ExceptionAucunNoeud& e) {
                // Aucun noeud ne peut être supprimé, il n'y a pas de connexion, la seule chose à faire reste à ajouter une connexion
                try {
                    mutation_ajoute_connexion();
                }
                catch (const ExceptionPasDeNoeudDisponible& e) {
                    // Ne devrait jamais arriver
                    cerr << "Erreur magique...?" << endl;
                }
            }
        }
        break;
    case Mutation::Type::SupprimeNoeud:
        try {
            mutation_supprime_noeud();
        }
        catch (const ExceptionAucunNoeud& e) {
            // Aucun noeud ne peut être supprimé, on supprime donc une connexion à la place
            try {
                mutation_supprime_connexion();
            }
            catch (const ExceptionAucuneConnexion& e) {
                // Il n'y a aucun noeud, et aucune connexion ; il ne reste plus qu'à créer une connexion
                try {
                    mutation_ajoute_connexion();
                }
                catch (const ExceptionPasDeNoeudDisponible& e) {
                    // Ne devrait jamais arriver
                    cerr << "Erreur magique...?" << endl;
                }
            }
        }
        break;
    }
}

double Genome::distance(int c_1, int c_2, int c_3, Genome* g)
{
    unsigned int E = 0;
    unsigned int D = 0;
    double poids_diff = 0.0;

    vector<GeneConnexion> c = g->get_connexions();
    size_t id_conn1 = 0, id_conn2 = 0;
    while (id_conn1 < connexions.size() && id_conn2 < c.size()) {
        GeneConnexion* g1 = &connexions[id_conn1];
        GeneConnexion* g2 = &c[id_conn2];
        if (g1->get_innovation() == g2->get_innovation()) {
            // Même gène, donc on compare les poids
            poids_diff += abs(g1->get_poids() - g2->get_poids());
        }
        else {
            // Disjoint / excès
            if (id_conn1 < connexions.size() && id_conn2 < c.size()) {
                // Disjoint
                D++;
                if (g1->get_innovation() < g2->get_innovation())
                    id_conn1++;
                else
                    id_conn2++;
            }
            else if (id_conn1 == connexions.size()) {
                // Excès
                E++;
                id_conn2++;
            }
            else if (id_conn2 == connexions.size()) {
                // Excès
                E++;
                id_conn1++;
            }
        }
    }
    size_t N = max({ connexions.size(), c.size() });

    return (double)c_1 * E / N + (double)c_2 * D / N + c_3 * poids_diff;
}