#ifndef _FACTORS_FAST_ITERATOR_COMPARATOR_H
#define _FACTORS_FAST_ITERATOR_COMPARATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <algorithm>
#include <vector>
#include <sdsl/bit_vectors.hpp>

/*#include "FactorsIterator.h"*/

using namespace std;
using namespace sdsl;

class FactorsFastIteratorComparator : public std::binary_function<unsigned long long, unsigned long long, bool> {
private:
	vector<unsigned long long> *factors_starts;
	char *full_text;
	unsigned long long full_size;

public:
	FactorsFastIteratorComparator();
	
	FactorsFastIteratorComparator(vector<unsigned long long> *_factors_starts, char *_full_text, unsigned long long _full_size);
	
	inline bool operator()(const unsigned long long a, const unsigned long long b){
		unsigned long long start_a = factors_starts->at(a);
		unsigned long long start_b = factors_starts->at(b);
		unsigned long long len_a = full_size - start_a;
		unsigned long long len_b = full_size - start_b;
		for(unsigned long long i = 0; i < ((len_a < len_b)?len_a:len_b); ++i){
			if( full_text[ start_a + i ] < full_text[ start_b + i ] ){
				return true;
			}
			if( full_text[ start_a + i ] > full_text[ start_b + i ] ){
				return false;
			}
		}
		return (len_a < len_b);
	}
};

class FactorsFastIteratorReverseComparator : public std::binary_function<unsigned long long, unsigned long long, bool> {

private:
	vector<unsigned long long> *factors_starts;
	char *full_text;
	unsigned long long full_size;

public:
	FactorsFastIteratorReverseComparator();
	
	FactorsFastIteratorReverseComparator(vector<unsigned long long> *_factors_starts, char *_full_text, unsigned long long _full_size);
	
	inline bool operator()(const unsigned long long a, const unsigned long long b){
		if( a == 0 ){
			return true;
		}
		if( b == 0){
			return false;
		}
/*		cout << "Comp " << a << ", " << b << "\n";*/
		unsigned long long start_a = factors_starts->at(a) - 1;
		unsigned long long start_b = factors_starts->at(b) - 1;
		unsigned long long len_a = factors_starts->at(a) - factors_starts->at(a-1);
		unsigned long long len_b = factors_starts->at(b) - factors_starts->at(b-1);
		for(unsigned long long i = 0; i < ((len_a < len_b)?len_a:len_b); ++i){
			if( full_text[ start_a - i ] < full_text[ start_b - i ] ){
				return true;
			}
			if( full_text[ start_a - i ] > full_text[ start_b - i ] ){
				return false;
			}
		}
		return (len_a < len_b);
	}
};


class CompactedFactorsFastIteratorComparator : public std::binary_function<unsigned long long, unsigned long long, bool>
{
private:
	vector<pair<unsigned long long, unsigned long long>> factors;
	unsigned long long n_factors;
	const char *ref_text;
	// Contador para comparaciones efectivas
	long long comp;
	// Cantidad de coincidencias
	long long idem;
	// Cantidad de frases leidas
	long long f_read;

public:

	CompactedFactorsFastIteratorComparator();

	CompactedFactorsFastIteratorComparator(vector<pair<unsigned long long, unsigned long long>> &_factors, const char *_ref_text);

	inline bool operator()(const unsigned long long _a, const unsigned long long _b){
		unsigned long long a = _a;
		unsigned long long b = _b;
		bool is_last = false;
		// EXPERIMENTAL
		comp = 0;
		idem = 0;
		f_read = 0;

		// Tanto a y b son indices que reflejan el a-esimo y el b-esimo factor
		// Entonces el primer factor siembre ira antes que cualquier otro
		if( a == 0) return true;
		if( b == 0) return false;
		if( a == b) return false;
		
		// Si no es el caso, entonces debemos ocurrir en que posicion ocurre mi factor y el que viene despues de mi
		// Tanto para a como b
		// (!) Caso borde: a y b son el ultimo
			// Da igual cual sea el ultimo, cuando terminemos de procesar el mas corto, no seguiremos consultando la siguiente frase
		if (a == n_factors || b == n_factors)
		{
			is_last = true;
		}

		pair<unsigned long long, unsigned long long> f_a = factors[a];
		pair<unsigned long long, unsigned long long> f_b = factors[b];
		unsigned long long i_trigger;

		bool isNextA = false;
		if (f_a.second < f_b.second)
		{
			i_trigger = f_a.second;
			isNextA = true;
		}
		else
		{
			i_trigger = f_b.second;
			isNextA = false;
		}
		// Para detectar cuando observamos la misma posicion dentro de la referencia
		bool pointCoincidence = false;
		unsigned long long delta = 0;

		for (unsigned long long i = 0; i <= i_trigger; i++)
		{
			// Si terminamos de leer para el largo mas corto o coincidimos con algun punto en la referencia
			pointCoincidence = (f_a.first + i) == (f_b.first + i);
			if (i == i_trigger || pointCoincidence)
			{
				delta = pointCoincidence? 0:i;
				// Si ya no quedan factores por leer, se acabo
				if (is_last) continue;
				// Sino, tenemos que avanzar la lectura en base al factor mas corto
				// Si debemos avanzar en a
				if (isNextA)
				{
					// Si estamos avanzando por coincidencia de punto
					if(pointCoincidence)
					{
						// Adelantamos la lectura equivalente al factor de a
						f_b.first += f_a.second;
						// Y actualizamos el largo del factor
						f_b.second -= f_a.second;
					}
					// Avanzamos el indice y leemos el factor sgte
					a++;
					f_a = factors[a];
					// Y comparamos cual es el largo mas corto actual
					if (f_a.second < f_b.second - delta)
					{
						i_trigger = f_a.second;
						isNextA = true;
					}
					else
					{
						i_trigger = f_b.second - delta;
						isNextA = false;
					}
				}
				else
				{
					// Si estamos avanzando por coincidencia de punto
					if(pointCoincidence)
					{
						// Adelantamos la lectura equivalente al factor de b
						f_a.first += f_b.second;
						// Y actualizamos el largo del factor
						f_a.second -= f_b.second;
					}
					// Avanzamos el indice y leemos el factor sgte
					b++;
					f_b = factors[b];
					// Y comparamos cual es el largo mas corto actual
					if (f_a.second - delta < f_b.second)
					{
						i_trigger = f_a.second - delta;
						isNextA = true;
					}
					else
					{
						i_trigger = f_b.second;
						isNextA = false;
					}
				}
				f_read++;
				// Reinicio i
				i = 0;
				// Si alguno de los factores es terminal, notificamos el estado de final
				if (a == n_factors || b == n_factors) is_last = true;
				// Si resulta que estamos leyendo desde el mismo punto en LZ(S|R), son necesariamente iguales a partir de ahora
				// Asi que nos quedamos con el resultado de (_a < _b)
				if (a == b) break;
			}
			// Si queda por leer
			if( ref_text[ f_a.first + i ] < ref_text[ f_b.first + i ] ){
				comp++;
				cout << "[CD] " << _a << " " << _b << " " << comp << " " << idem << " " << f_read << endl;
				return true;
			}
			if( ref_text[ f_a.first + i ] > ref_text[ f_b.first + i ] ){
				comp++;
				cout << "[CD] " << _a << " " << _b << " " << comp << " " << idem << " " << f_read << endl;
				return false;
			}
			comp++;
			idem++;
		}
		// Si fueron iguales, nos quedamos con el mas corto
		// (!) Duda: Aqui salia una comparacion de largos, pero si a y b son indices de factores, necesariamente lo de abajo es equivalente
		cout << "[CD] " << _a << " " << _b << " " << comp << " " << idem << " " << f_read << endl;
		return (_a < _b);
	}
};

class CompactedFactorsFastIteratorReverseComparator : public std::binary_function<unsigned long long, unsigned long long, bool>
{
private:
	vector<pair<unsigned long long, unsigned long long>> factors;
	const char *ref_text;
	// Contador para comparaciones efectivas
	long long comp;
	// Cantidad de coincidencias
	long long idem;
	// Cantidad de frases leidas
	long long f_read;

public:
	CompactedFactorsFastIteratorReverseComparator();

	CompactedFactorsFastIteratorReverseComparator(vector<pair<unsigned long long, unsigned long long>> &_factors, const char *_ref_text);

	inline bool operator()(const unsigned long long a, const unsigned long long b){
		if( a == 0) return true;
		if( b == 0) return false;
		if( a == b) return false;

		// Recuperamos el prefijo que parte en a-1 y termina en a, analogo b
		pair<unsigned long long, unsigned long long> f_a_minus = factors[a - 1];
		pair<unsigned long long, unsigned long long> f_b_minus = factors[b - 1];
		unsigned long long start_a = f_a_minus.first + f_a_minus.second - 1;
		unsigned long long start_b = f_b_minus.first + f_b_minus.second - 1;
		unsigned long long len_a = f_a_minus.second;
		unsigned long long len_b = f_b_minus.second;

		for(unsigned long long i = 0; i < ((len_a < len_b)?len_a:len_b); ++i){
			if( ref_text[ start_a - i ] < ref_text[ start_b - i ] ){
				return true;
			}
			if( ref_text[ start_a - i ] > ref_text[ start_b - i ] ){
				return false;
			}
		}
		return (len_a < len_b);
	}	
};

#endif //_FACTORS_ITERATOR_COMPARATOR_H
