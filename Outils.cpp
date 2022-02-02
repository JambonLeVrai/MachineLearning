#include "Outils.h"

double Outils::aleatoire_f(double min, double max)
{
    uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

template<typename T>
T Outils::aleatoire_i(T min, T max)
{
    uniform_int_distribution<T> distribution(min, max);
    return distribution(generator);
}

template<typename T>
T Outils::aleatoire_table(map<T, unsigned int> table)
{
    int max_proba = 0;
    for (auto& it : table)
        max_proba += it.second;

    int res = Outils::aleatoire_i(0, max_proba);

    int val = 0;
    for (auto& it : tab) {
        val += it.second;
        if (res <= val)
            return it.first;
    }
    return table.begin()->first;
}

size_t Outils::aleatoire_avec_exclusion(vector<size_t> valeurs_exclues, size_t min, size_t max)
{
    if (valeurs_exclues.size() >= max - min + 1)
        throw ExceptionAleatoire();

    sort(valeurs_exclues.begin(), valeurs_exclues.end());
    uniform_int_distribution<size_t> distribution(min, max - valeurs_exclues.size());
    size_t ref = distribution(generator);
    size_t shift = 0;
    size_t result = 0;
    for (auto it : valeurs_exclues) {
        if (ref + shift >= it)
            shift++;
        else {
            result = ref + shift;
            break;
        }
    }
    return result;
}
