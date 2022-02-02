#pragma once
#include "Reseau.h"
#include "Outils.h"
#include <map>
#include <optional>
#include <queue>

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

namespace Mutation {
	enum class Type {
		AjouteConnexion, AjouteNoeud, ModifiePoids, SupprimeNoeud, ActiveConnexion, SupprimeConnexion
	};
	
	const map<Type, int> probas_mutations = {
		{Type::AjouteConnexion, 8},
		{Type::AjouteNoeud, 4},
		{Type::ModifiePoids, 20},
		{Type::SupprimeNoeud, 1},
		{Type::ActiveConnexion, 10},
		{Type::SupprimeConnexion, 2},
	};
};

class Genome {
public:
	Genome(); // Constructeur par d�faut
	Genome(size_t _entrees, vector<GeneConnexion> _connexions, vector<GeneNoeud> _noeuds, vector<GeneNoeud> _sorties); // Constructeur � partir de listes de g�nes
	Genome(Genome* g, Mutation::Type mutation); // Constructeur par mutation d'un autre g�nome
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

	void mute(Mutation::Type mutation);

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

typedef pair<Genome*, optional<double>> GenomeMesure; // Associe un g�nome avec sa mesure de fitness
typedef vector<GenomeMesure> Espece; // Contient l'ensemble des g�nomes d'une m�me esp�ce

constexpr double DEF_C1 = 1.0;
constexpr double DEF_C2 = 1.0;
constexpr double DEF_C3 = 1.0;
constexpr double DEF_DT = 3.0;

class NEAT {
public:
	NEAT(size_t entrees, size_t sorties, size_t nb_genomes_initiaux=10);
	void set_coeffs(double _c1, double _c2, double _c3, double _seuil_diff);
	void evolue(); // Mesure l'aptitude de tous les nouveaux g�nomes, puis r�alise un tri et cr�� des nouveaux g�nomes
	virtual	double mesure_aptitude(Genome* g); // G�n�re les r�seaux et �value les r�seaux en fonction du probl�me
private:
	double C1 = DEF_C1, C2 = DEF_C2, C3 = DEF_C3, seuil_diff = DEF_DT;
	vector<Espece> especes;
};

