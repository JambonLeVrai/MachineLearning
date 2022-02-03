#pragma once
#include "Reseau.h"
#include "Outils.h"
#include "Genome.h"
#include <map>
#include <optional>
#include <queue>

class GenomeMesure {
public:
	GenomeMesure(pair<Genome, optional<double>> _val);
	GenomeMesure(Genome g, optional<double> m = nullopt);

	Genome* get_genome();
	optional<double> get_mesure();
	void set_mesure(double v);

	bool operator<(const GenomeMesure &g2);

private:
	pair<Genome, optional<double>> val;
};

/*class Espece {
public:
	Espece(vector<GenomeMesure> _val);

private:
	vector<GenomeMesure> val;
};*/

typedef vector<GenomeMesure> Espece; // Contient l'ensemble des génomes d'une même espèce

constexpr double DEF_C1 = 1.0;
constexpr double DEF_C2 = 1.0;
constexpr double DEF_C3 = 1.0;
constexpr double DEF_DT = 3.0;

class NEAT {
public:
	NEAT(size_t entrees, size_t sorties, size_t _nb_genomes, size_t nb_connexions_initiales);
	void set_coeffs(double _c1, double _c2, double _c3, double _seuil_diff);
	void evolue(); // Mesure l'aptitude de tous les nouveaux génomes, puis réalise un tri et créé des nouveaux génomes
	virtual	double mesure_aptitude(Genome* g); // Génère les réseaux et évalue les réseaux en fonction du problème
private:
	double C1 = DEF_C1, C2 = DEF_C2, C3 = DEF_C3, seuil_diff = DEF_DT;
	size_t nb_genomes;
	vector<Espece> especes;
};

