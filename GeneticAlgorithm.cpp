#include <ctime>
#include <iostream>
#include <math.h>
#include <vector>
#include <set>

using namespace std;

// A 1 dimensional function we wish to optimize.
// f(x) = x^2 + 2*((x + 5)^4)
int easy_fitness_functon(int x) {
	int fitness = (pow(x, 2)) + (2 * (pow((x + 5), 4)));
	return fitness;
}


// Initializes the individual population randomly from the input interval.
// We choose a population of 4 integers.
vector<int> initialize(int min, int max, int size) {
	set<int> population;
    srand (time (nullptr));
	while (population.size() < size) {
		int random = (rand() % (max - min)) + min;  // number between min and max
		population.insert(random);
	}
	return std::vector<int>(population.begin(), population.end());

}


// Evaluates the fitness of individuals in the population and assigns a
// value indicating the fitness.
vector<int> evaluate(vector<int> population) {
	cout << "Evaluating fitness scores" << endl;
	vector<int> fitness_values(population.size(), 0);
	for (int i = 0; i < population.size(); ++i) {
		fitness_values.at(i) = easy_fitness_functon(population.at(i));
	}

	return fitness_values;
}


// Takes input as the current population and the fitness score for each
// individual in the population and generates the set of population which
// is fit for crossover. Discards the weakest member.
vector<int> selection(vector<int> current_population, vector<int> fitness_values) {
	vector<int> new_population;

	// Insertion sort.
	int i,j;
	for (j = 0; j < current_population.size(); j++) {
	    int iMin = j;
	    for ( i = j+1; i < current_population.size(); i++) {
	        if (fitness_values.at(i) < fitness_values.at(iMin)) {
	            iMin = i;
	        }
	    }

	    // swap
	    int temp = current_population.at(iMin);
	    current_population.at(iMin) = current_population.at(j);
	    current_population.at(j) = temp;

	    // swap
	    int temp_fitness = fitness_values.at(iMin);
	    fitness_values.at(iMin) = fitness_values.at(j);
	    fitness_values.at(j) = temp_fitness;
	}

	new_population.insert(new_population.begin(), current_population.begin(), current_population.end());
	return new_population;
}

// 0-indexed. We make sure that some crossover always happens.
int get_crossover_point() {
	// The numbers can vary from -10 to 10. So we can use a binary representation
	// which can represent 21 numbers. The number of bits needed for this
	// representation is 5. (11111 = 31)
	// Relative chance of crossover at a location is -
	//    x     x      x      x    x
	// 0.0001 0.001  0.011   0.1   1    | Weights
	//   1     10     100   1000  10000 | Multiply by 10000
	//   1     11     111   1111  11111 | Total
	int random = rand() % 11111;
	int crossover_point;

	if (random < 1) {
		return 0;
	} else if (random < 11) {
		return 1;
	} else if (random < 111) {
		return 2;
	} else if (random < 1111) {
		return 3;
	}

	return 4;  // default
}

// We know that a and b are allowed to lie between -16 and 15 at this point as
// we can store values upto 31 using 5 bits. So we first add the number to 16
// and then do our computations for making things easier. We subtract 16 in the
// end.
vector<int> mate(int parent1, int parent2) {
	int crossover_point = get_crossover_point();

	string parent1_bin =  std::bitset<5>(parent1 + 16).to_string();
	string parent2_bin =  std::bitset<5>(parent2 + 16).to_string();

	string child1_bin = parent1_bin.substr(0, crossover_point) + parent2_bin.substr(crossover_point, 5);
	string child2_bin = parent2_bin.substr(0, crossover_point) + parent1_bin.substr(crossover_point, 5);

	int child1 = std::stoull(child1_bin, 0, 2);
	int child2 = std::stoull(child2_bin, 0, 2);

	vector<int> children;
	children.push_back(child1 - 16);
	children.push_back(child2 - 16);
	return children;
}

// Takes a population and does some cross-overs to increase the number of
// individuals in the population. The strongest member in the population
// mates with the remaining members to generate 4 individuals.
// This does a single point crossover.
vector<int> crossover(vector<int> current_population) {
	// Mate 1 and 2
	vector<int> children1 = mate(current_population.at(0), current_population.at(1));
	cout << "First mating done between " << current_population.at(0) << " and " <<
			current_population.at(1) << ". Children are " <<
			children1.at(0) << " and " << children1.at(1) << endl;

	// Mate 1 and 3
	vector<int> children2 = mate(current_population.at(0), current_population.at(2));
	cout << "Second mating done between " << current_population.at(0) << " and " <<
			current_population.at(2) << ". Children are " <<
			children2.at(0) << " and " << children2.at(1) << endl;

	vector<int> new_population;
	new_population.insert(new_population.begin(), children1.begin(), children1.end());
	new_population.insert(new_population.begin(), children2.begin(), children2.end());

	return new_population;
}

// Flips a random bit in a number with equal probability.
int flip_bit(int x) {
	string x_bin =  std::bitset<5>(x).to_string();
	int bit_to_flip = rand() % 5;  // A bit in [0, 5)

	x_bin.at(bit_to_flip) =
			x_bin.at(bit_to_flip) == '0' ? '1' : '0';

	int x_mut = std::stoull(x_bin, 0, 2);

	return x_mut;
}

// Mutates a number with some probability.
int mutate(int x) {
	// Assume a 10% chance of a mutation, which can flip any bit with equal
	// Probability. Tweak this function as desired.
	int random = rand() % 100;
	if (random < 10) {  // 10% chance
		return flip_bit(x);
	}
	return x;  // no mutation
}

// Adds mutations to the current population to increase the number of individuals.
vector<int> mutations(vector<int> current_population) {
	vector<int> mutated_population;
	for (int i : current_population) {
		mutated_population.push_back(mutate(i));
	}
	return mutated_population;
}

int main() {
	srand (time (nullptr));

	// Configuration parameters for the run.
	int min_x = -10;  // min value of population
	int max_x = 10;  // max value of the population
	int pop_size = 4;  // Number of individuals in the population.
	int max_generations = 10;  // maximum iterations in the GA, for determining the stop condition.

	vector<int> current_population = initialize(min_x, max_x, pop_size);
	/////////////////////////////////////////////////////////////////////////////////////////////////
	cout << "Population Initialized :" << endl;
	for (auto i = current_population.begin(); i != current_population.end(); ++i) {
	    std::cout << *i << ' ';
	}
	cout << endl;
	/////////////////////////////////////////////////////////////////////////////////////////////////

	vector<int> fitness_values;
	int current_generation = 1;

	while(1) {
		/////////////////////////////////////////////////////////////////////////////////////////////////
		cout << "\n\nStarting generation " << current_generation << " of the genetic algorithm." << endl;
		cout << "Current Population :" << endl;
		for (auto i = current_population.begin(); i != current_population.end(); ++i) {
		    std::cout << ' ' << *i;
		}
		cout << endl;
		/////////////////////////////////////////////////////////////////////////////////////////////////

		// get fitness scores for each individual.
		fitness_values = evaluate(current_population);
		/////////////////////////////////////////////////////////////////////////////////////////////////
		cout << "Fitness values for the population Initialized :" << endl;
		for (int i = 0; i < current_population.size(); ++i) {
		    std::cout << current_population[i] << " (" << fitness_values[i] << ")("
		    		<< easy_fitness_functon(current_population[i]) << ")   ";
		}
		cout << endl;
		/////////////////////////////////////////////////////////////////////////////////////////////////

		// generate new population using fitness scores. This is less than the original population.
		vector<int> new_population = selection(current_population, fitness_values);
		cout << "Fittest individual in current population is : " << current_population.at(0) << endl;
		if (++current_generation >= max_generations) {
			current_population = new_population;
			break;
		}

 		// now apply crossovers and mutations on the new population.
		new_population = crossover(new_population);
		/////////////////////////////////////////////////////////////////////////////////////////////////
		cout << "Crossed-over population :" << endl;
		for (int i = 0; i < new_population.size(); ++i) {
		    std::cout << new_population[i] << " (" << easy_fitness_functon(current_population[i]) << ")   ";
		}
		cout << endl;
		/////////////////////////////////////////////////////////////////////////////////////////////////

		// We begin adding mutations after a few generations have evolved.
		if (current_generation > max_generations/2) {
			new_population = mutations(new_population);
			/////////////////////////////////////////////////////////////////////////////////////////////////
			cout << "Mutated population :" << endl;
			for (int i = 0; i < new_population.size(); ++i) {
			    std::cout << new_population[i] << " (" << easy_fitness_functon(current_population[i]) << ")   ";
			}
			cout << endl;
			/////////////////////////////////////////////////////////////////////////////////////////////////
		}

		current_population = new_population;
	}

	cout << "\n\nThe final fittest individual calculated is " <<
			current_population.at(0) << " and the value is " <<
			easy_fitness_functon(current_population.at(0)) << "." << endl;

	return 0;
}
