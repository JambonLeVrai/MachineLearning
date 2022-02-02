#pragma once
#include <vector>
#include <random>
#include <iostream>
#include <exception>
#include <map>

using namespace std;

struct ExceptionAleatoire : public exception {
	const char* what() const throw () {
		return "Impossible de g�n�rer un nombre al�atoire";
	}
};

class Outils
{
public:
	static double aleatoire_f(double min, double max);
	static size_t aleatoire_avec_exclusion(vector<size_t> valeurs_exclues, size_t min, size_t max);

	template<typename T>
	static T aleatoire_i(T min, T max);

	template<typename T>
	static T aleatoire_table(map<T, unsigned int> table);

	static default_random_engine generator;
};

