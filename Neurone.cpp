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

Neurone::Neurone(double (*_fonction_activation)(double), vector<Connexion> _entrees, double _biais)
{
	entrees = _entrees;
	biais = _biais;
	calc = false;
	fonction_activation = _fonction_activation;
	val = 0;
	deltaW.resize(entrees.size());
}

double Neurone::eval()
{
	if (!calc) {
		double sum = biais;
		for (auto& it : entrees) {
			sum += it.first->eval() * it.second;
		}

		val = fonction_activation(sum);
		calc = true;
	}
	return val;
}

void Neurone::raz() {
	calc = false;
}

void Neurone::aj_entree(Connexion c) {
	entrees.push_back(c);
	deltaW.resize(entrees.size());
}

void Neurone::aj_entrees(vector<Connexion> c) {
	for (auto& it : c)
		entrees.push_back(it);
	deltaW.resize(entrees.size());
}

vector<Connexion> Neurone::get_entrees() {
	return entrees;
}

void Neurone::set_deltaW(size_t c, double w) {
	deltaW[c] = w;
}

void Neurone::app_deltaW() {
	for (size_t i = 0; i < entrees.size(); i++) {
		entrees[i].second += deltaW[i];
	}
}

void Neurone::log() {
	for (auto& it : entrees)
		cout << it.second << endl;
}

NeuroneEntree::NeuroneEntree() : Neurone() {
}

double NeuroneEntree::eval() {
	return val;
}

void NeuroneEntree::set_val(double v) {
	val = v;
}