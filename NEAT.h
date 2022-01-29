#pragma once
#include "Reseau.h"
#include <map>

class GeneConnexion {
public:
	GeneConnexion(size_t _entree, size_t _sortie, double _poids, bool _actif, int _innovation);
	size_t get_entree();
	size_t get_sortie();
	double get_poids();
	bool get_actif();
	int get_innovation();

	bool operator<(const GeneConnexion& other);

private:
	size_t entree, sortie;
	double poids;
	bool actif;
	int innovation;
};

const enum class TypeNoeud {
	ENTREE, SORTIE, CACHE
};

class GeneNoeud {
public:
	GeneNoeud(size_t _id, TypeNoeud _type);
	size_t get_id();
	TypeNoeud get_type();

private:
	size_t id;
	TypeNoeud type;
};

class Genome {
public:
	Genome();
	Genome(vector<GeneConnexion> _connexions, vector<GeneNoeud> _noeuds);
	vector<GeneConnexion> get_connexions();
	vector<GeneNoeud> get_noeuds();
	void mutation_ajoute_connexion();
	void mutation_ajoute_noeud();

private:
	vector<GeneConnexion> connexions;
	vector<GeneNoeud> noeuds;
};

class NEAT {
public:
	static Reseau* ProduitReseau(Genome g);
private:
};

