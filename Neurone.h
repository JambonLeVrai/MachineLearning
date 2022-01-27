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

class Neurone
{
public:
	Neurone(double (*_fonction_activation)(double) = *Activation::Id, vector<Connexion> _entrees = vector<Connexion>(), double _biais=0);
	virtual double eval();
	void aj_entree(Connexion c);
	void aj_entrees(vector<Connexion> c);
	void raz();
	vector<Connexion> get_entrees();
	void set_deltaW(size_t c, double w);
	void app_deltaW();
	void log();

protected:
	vector<Connexion> entrees;				// La liste des neurones en entrée, servant à évaluer le réseau
	//vector<Connexion> sorties;				// La liste des neurones en sortie (calculée à partir des entrées), servant à backpropagate
	double (*fonction_activation)(double);
	double biais;
	double val;
	bool calc;

	vector<double> deltaW;
};

class NeuroneEntree : public Neurone {
public:
	NeuroneEntree();
	double eval();
	void set_val(double v);
};