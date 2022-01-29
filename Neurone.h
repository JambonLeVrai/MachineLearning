#pragma once
#include <vector>
#include <iostream>
#include <utility>

using namespace std;

class Neurone;

using Connexion = pair<Neurone*, double>;

namespace Activation {
	double Heaviside(double x, double seuil);
	double Sigmoid(double x);
	double Tanh(double x);
	double Id(double x);
	double ReLU(double x);
}

class Axone {
public:
	Axone(Neurone* _entree, Neurone* _sortie, double _poids=1.0);
	void set_delta_poids(double d);
	void app_delta_poids();
	double get_poids();
	Neurone* get_entree();
	Neurone* get_sortie();

private:
	Neurone* entree;
	Neurone* sortie;
	double poids;
	double delta_poids;
};

class Neurone
{
public:
	Neurone(double (*_fonction_activation)(double) = *Activation::Id, vector<Axone*> _entrees = vector<Axone*>(), vector<Axone*> _sorties = vector<Axone*>(), double _biais=0);
	virtual double eval();
	void aj_entree(Axone* c);
	void aj_sortie(Axone* a);
	void aj_entrees(vector<Axone*> c);
	void raz();
	vector<Axone*> get_entrees();
	vector<Axone*> get_sorties();
	void app_deltaBiais(double b);
	void log();

protected:
	vector<Axone*> entrees;					// La liste des neurones en entrée, servant à évaluer le réseau
	vector<Axone*> sorties;				// La liste des neurones en sortie (calculée à partir des entrées), servant à backpropagate
	double (*fonction_activation)(double);
	double biais;
	double val;
	bool calc;
};

class NeuroneEntree : public Neurone {
public:
	NeuroneEntree();
	double eval();
	void set_val(double v);
};