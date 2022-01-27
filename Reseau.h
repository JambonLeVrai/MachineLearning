#pragma once
#include "Neurone.h"
#include <vector>
#include <random>
#include <future>

struct ExceptionTaille : public exception {
	const char* what() const throw () {
		return "La taille des tableaux ne correspond pas";
	}
};

class Reseau
{
public:
	Reseau(size_t e, size_t s, double (*_fonction_activation)(double) = *Activation::Sigmoid);
	void set_entrees(vector<double> e);
	vector<double> get_sorties();
	void raz();
	virtual void train(vector<double> e, vector<double> r) {}

protected:
	virtual void eval();

	vector<NeuroneEntree*> entrees;
	vector<Neurone*> neurones;
	vector<Neurone*> sorties;
};

class Perceptron : public Reseau
{
public:
	Perceptron(size_t e, size_t s, size_t h, double (*_fonction_activation)(double) = *Activation::Sigmoid);
	void train(vector<double> e, vector<double> r);
	void log();
private:
	void eval();

	vector<future<double>> neurones_async;
	vector<future<double>> sorties_async;
};