#include "Neurone.h"
#include <cmath>

double Activation::Heaviside(double x, double seuil)
{
	if (x >= seuil)
		return 1.0;
	else
		return 0.0;
}

double Activation::Sigmoid(double x)
{
	return 1/(1+exp(-x));
}

double Activation::Tanh(double x)
{
	return tanh(x);
}

double Activation::Id(double x)
{
	return x;
}

double Activation::ReLU(double x)
{
	return (x >= 0) ? x : 0;
}

Neurone::Neurone(double (*_fonction_activation)(double), vector<Axone*> _entrees, vector<Axone*> _sorties, double _biais)
{
	entrees = _entrees;
	biais = _biais;
	calc = false;
	fonction_activation = _fonction_activation;
	val = 0;
}

double Neurone::eval()
{
	if (!calc) {
		double sum = biais;
		for (auto& it : entrees) {
			sum += it->get_entree()->eval() * it->get_poids();
		}

		val = fonction_activation(sum);
		calc = true;
	}
	return val;
}

void Neurone::raz() {
	calc = false;
}

void Neurone::aj_entree(Axone* c) {
	entrees.push_back(c);
}

void Neurone::aj_sortie(Axone* a)
{
	sorties.push_back(a);
}

void Neurone::aj_entrees(vector<Axone*> c) {
	for (auto& it : c)
		entrees.push_back(it);	
}

vector<Axone*> Neurone::get_entrees() {
	return entrees;
}

vector<Axone*> Neurone::get_sorties()
{
	return sorties;
}


void Neurone::app_deltaBiais(double b)
{
	biais += b;
}

void Neurone::log() {
	/*for (auto& it : entrees)
		cout << it.second << endl;*/
}

NeuroneEntree::NeuroneEntree() : Neurone() {
}

double NeuroneEntree::eval() {
	return val;
}

void NeuroneEntree::set_val(double v) {
	val = v;
}

Axone::Axone(Neurone* _entree, Neurone* _sortie, double _poids) : entree(_entree), sortie(_sortie), poids(_poids), delta_poids(0.0)
{}

void Axone::set_delta_poids(double d)
{
	delta_poids = d;
}

void Axone::app_delta_poids()
{
	poids += delta_poids;
}

double Axone::get_poids()
{
	return poids;
}

Neurone* Axone::get_entree()
{
	return entree;
}

Neurone* Axone::get_sortie()
{
	return sortie;
}
