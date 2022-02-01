#pragma once
#include "Reseau.h"
#include "Outils.h"
#include <map>
#include <optional>

#define MIN_POIDS_ALEATOIRE -1.0
#define MAX_POIDS_ALEATOIRE 1.0
#define MIN_DELTA_POIDS -0.5
#define MAX_DELTA_POIDS 0.5

struct ExceptionPasDeNoeudDisponible : public exception {
	const char* what() const throw () {
		return "Aucune connexion disponible pour y ajouter un n�ud";
	}
};

struct ExceptionAucunNoeud : public exception {
	const char* what() const throw () {
		return "Aucun n�ud ne peut �tre supprim�";
	}
};

struct ExceptionPasDeConnexionPossible : public exception {
	const char* what() const throw () {
		return "Impossible d'ajouter une connexion au r�seau";
	}
};

struct ExceptionToutesConnexionsActives : public exception {
	const char* what() const throw () {
		return "Toutes les connexions sont d�j� actives";
	}
};

struct ExceptionAucuneConnexion : public exception {
	const char* what() const throw () {
		return "Il n'existe aucune connexion";
	}
};

class GeneConnexion {
public:
	GeneConnexion(size_t _entree, size_t _sortie, double _poids, bool _actif, size_t _innovation);
	size_t get_entree();
	size_t get_sortie();
	double get_poids();
	bool get_actif();
	int get_innovation();
	void delta_poids(double delta);
	void desactive();
	void active();

	bool operator<(const GeneConnexion& other);

private:
	size_t entree, sortie;
	double poids;
	bool actif;
	size_t innovation;
};

class GeneNoeud {
public:
	GeneNoeud(size_t _id);
	size_t get_id();

private:
	size_t id;
};

enum class Mutation {
	AjouteConnexion, AjouteNoeud, ModifiePoids, SupprimeNoeud, ActiveConnexion, SupprimeConnexion
};

const map<Mutation, double> probas_mutations = {
		{Mutation::AjouteConnexion, 1.0},
		{Mutation::AjouteNoeud, 1.0},
		{Mutation::ModifiePoids, 1.0},
		{Mutation::SupprimeNoeud, 1.0},
		{Mutation::ActiveConnexion, 1.0},
		{Mutation::SupprimeConnexion, 1.0},
};

class MutationProba {
public:
	static Mutation get_mutation_aleatoire();
	
};

class Genome {
public:
	Genome(); // Constructeur par d�faut
	Genome(size_t _entrees, vector<GeneConnexion> _connexions, vector<GeneNoeud> _noeuds, vector<GeneNoeud> _sorties); // Constructeur � partir de listes de g�nes
	Genome(Genome* g, Mutation mutation); // Constructeur par mutation d'un autre g�nome
	Genome(Genome* p1, Genome* p2); // Constructeur � partir de deux g�nomes parents
	void genere_reseau();
	vector<GeneConnexion> get_connexions();
	vector<GeneNoeud> get_noeuds();
	Reseau* get_reseau();
	void mutation_ajoute_connexion();
	void mutation_ajoute_noeud();
	void mutation_modifie_poids();
	void mutation_supprime_noeud();
	void mutation_active_connexion();
	void mutation_supprime_connexion();
	double distance(int c_1, int c_2, int c_3, Genome* g);

private:
	vector<GeneConnexion> connexions;
	vector<GeneNoeud> entrees; // Les noeuds d'entr�e ne sont pas modifiables
	vector<GeneNoeud> sorties; // Les noeuds de sortie ne peuvent pas �tre supprim�s / d�sactiv�s, mais peuvent voir leur poids / biais changer
	vector<GeneNoeud> noeuds;
	size_t innovation = 0;
	size_t nombre_connexions_desac = 0;
	Reseau* reseau_associe;
};

typedef pair<Genome, optional<double>> GenomeMesure;
typedef vector<GenomeMesure> Espece;

class NEAT {
public:
	void mesure_aptitude();
private:
	vector<Espece> especes;
};

