#include "NEAT.h"

GeneConnexion::GeneConnexion(size_t _entree, size_t _sortie, double _poids, bool _actif, int _innovation) : 
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

bool GeneConnexion::operator<(const GeneConnexion& other)
{
    if (sortie < other.sortie)
        return true;
    else if (entree < other.entree)
        return true;
    return false;
}

GeneNoeud::GeneNoeud(size_t _id, TypeNoeud _type) : id(_id), type(_type)
{
}

size_t GeneNoeud::get_id()
{
    return id;
}

TypeNoeud GeneNoeud::get_type()
{
    return type;
}

Reseau* NEAT::ProduitReseau(Genome g) {
    map<size_t, Neurone*> neurones_id;
    vector<NeuroneEntree*> entrees;
    vector<Neurone*> neurones;
    vector<Neurone*> sorties;
    vector<Axone*> axones;

    for (auto it : g.get_noeuds()) {
        Neurone* n;
        switch (it.get_type()) {
        case TypeNoeud::ENTREE:
            n = new NeuroneEntree();
            entrees.push_back((NeuroneEntree*)n);
            break;
        case TypeNoeud::SORTIE:
            n = new Neurone();
            neurones.push_back(n);
            break;
        case TypeNoeud::CACHE:
            n = new Neurone();
            sorties.push_back(n);
            break;
        }
        neurones_id.insert(pair<size_t, Neurone*>(it.get_id(), n));
    }

    for (auto it : g.get_connexions()) {
        if (it.get_actif()) {
            Neurone *e, *s;
            e = neurones_id[it.get_entree()];
            s = neurones_id[it.get_sortie()];
            Axone* a = new Axone(e, s, it.get_poids());
            e->aj_sortie(a);
            s->aj_entree(a);
            axones.push_back(a);
        }
    }

    return new Reseau(entrees, sorties, neurones, axones);
}

Genome::Genome()
{
}

Genome::Genome(vector<GeneConnexion> _connexions, vector<GeneNoeud> _noeuds) : connexions(_connexions), noeuds(_noeuds)
{
}

vector<GeneConnexion> Genome::get_connexions()
{
    return connexions;
}

vector<GeneNoeud> Genome::get_noeuds()
{
    return noeuds;
}

void Genome::mutation_ajoute_connexion()
{
}

void Genome::mutation_ajoute_noeud()
{
}
