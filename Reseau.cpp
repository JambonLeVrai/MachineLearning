#include "Reseau.h"

Reseau::Reseau(size_t e, size_t s, double (*_fonction_activation)(double))
{
	for (size_t i = 0; i < e; i++)
		entrees.push_back(new NeuroneEntree());

	for (size_t i = 0; i < s; i++)
		sorties.push_back(new Neurone(_fonction_activation));
}

Reseau::Reseau(vector<NeuroneEntree*> _entrees, vector<Neurone*> _sorties, vector<Neurone*> _neurones, vector<Axone*> _axones) :
	entrees(_entrees), sorties(_sorties), neurones(_neurones), axones(_axones)
{
}

void Reseau::raz() {
	for (auto& it : neurones) {
		it->raz();
	}
	for (auto& it : sorties) {
		it->raz();
	}
}

void Reseau::eval()
{
	raz();

	for (auto& it : sorties) {
		it->eval();
	}
}

void Reseau::set_entrees(vector<double> e) {
	// On commence par vérifier si la taille de l'entrée est la bonne
	if (e.size() != entrees.size())
		throw ExceptionTaille();

	for (size_t i = 0; i < e.size(); i++)
		entrees[i]->set_val(e[i]);
}

vector<double> Reseau::get_sorties() {
	eval();

	vector<double> out;
	for (auto& it : sorties) {
		out.push_back(it->eval());
	}

	return out;
}