#include "Reseau.h"

Reseau::Reseau(size_t e, size_t s, double (*_fonction_activation)(double))
{
	for (size_t i = 0; i < e; i++)
		entrees.push_back(new NeuroneEntree());

	for (size_t i = 0; i < s; i++)
		sorties.push_back(new Neurone(_fonction_activation));
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

Perceptron::Perceptron(size_t e, size_t s, size_t h, double (*_fonction_activation)(double)) : Reseau(e, s) {
	default_random_engine generator;
	uniform_real_distribution<double> dist(-1.0, 1.0);
	
	// Création des neurones cachés
	for (int i = 0; i < h; i++) {
		// On génère les connexions vers tous les neurones d'entrée
		vector<Connexion> ce;
		for (auto* it : entrees)
			ce.push_back(*new Connexion(it, dist(generator)));
		// On créé le neurone caché
		Neurone* n = new Neurone(_fonction_activation, ce);
		neurones.push_back(n);
	}

	// Gestion des neurones de sortie
	for (auto& it : sorties) {
		// On génère les connexions vers tous les neurones cachés
		vector<Connexion> cs;
		for(auto &it2 : neurones)
			cs.push_back(*new Connexion(it2, dist(generator)));

		it->aj_entrees(cs);
	}

	// Créations des async
	for (auto& it : neurones)
		neurones_async.push_back(async([it]() {return it->eval(); }));

	for (auto& it : sorties)
		sorties_async.push_back(async([it]() {return it->eval(); }));
}

void Perceptron::train(vector<double> e, vector<double> r) {
	set_entrees(e);
	eval();

	if (r.size() != sorties.size())
		throw ExceptionTaille();

	for (size_t i = 0; i < r.size();i++) {
		for (size_t j = 0; j < sorties[i]->get_entrees().size(); j++) {
			sorties[i]->set_deltaW(j, 2 * (r[i] - sorties[i]->eval()) * sorties[i]->eval() * (1 - sorties[i]->eval()) * sorties[i]->get_entrees()[j].first->eval());
		}
	}

	for (size_t i = 0; i < neurones.size(); i++) {
		for(size_t j = 0; j < neurones[i]->get_entrees().size(); j++) {
			double deltaW = 0;
			// On recherche toutes les connexions avec les neurones de sortie
			for (size_t k = 0; k < sorties.size(); k++) {
				for (size_t l = 0; l < sorties[k]->get_entrees().size(); l++) {
					if (sorties[k]->get_entrees()[l].first == neurones[i]) {
						deltaW += sorties[k]->get_entrees()[l].second *
							sorties[k]->eval() * (1 - sorties[k]->eval()) *
							2 * (r[k] - sorties[k]->eval());
					}
				}
			}
			deltaW *= neurones[i]->eval() * (1 - neurones[i]->eval()) * neurones[i]->get_entrees()[j].first->eval();
			neurones[i]->set_deltaW(j, deltaW);
		}
	}

	double sum = 0;
	for (int i = 0; i < sorties.size();i++) {
		sum += pow(sorties[i]->eval() - r[i],2);
	}

	for (auto& it : sorties)
		it->app_deltaW();
	for (auto& it : neurones)
		it->app_deltaW();
}

void Perceptron::log() {
	for (auto& it : neurones) {
		it->log();
	}
	for (auto& it : sorties) {
		it->log();
	}
}

void Perceptron::eval() {
	raz();

	for (auto& it : neurones_async) {
		it.get();
	}

	for (auto& it : sorties_async)
		it.get();
}