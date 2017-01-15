//
//  main.cpp
//  lab3_gen_alg
//
//  Created by Николай Эмрих on 14.01.17.
//  Copyright © 2017 Николай Эмрих. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <cmath>

struct Indval;
void mutate(int & fenotype_item, int negative); //Ссылка на элемент массива
void reduce(int * genotype_with_parents, size_t size_gen_par, int * genotype, size_t size_gen);
void crossing(int & left, int & right, int * genotype_with_parents, size_t from, int negative, size_t range);
int * populate_genotype(size_t size, int from, int to);
size_t * get_random_indexes(size_t n, size_t m2);
bool compare(const Indval & left, const Indval & right);
double get_y(int x);

static const int staticlog = 6;

struct Indval {
    Indval(int val, size_t index) : val_(val), index_(index) {}
    
    int val_;
    size_t index_;
};

int main() {
    
    const size_t n = 8, m = n / 2, mut = m == 1 ? 1 : m;
    const int first_elem = -10;
    const int last_elem = 53;
    const size_t range = std::abs(first_elem) + std::abs(last_elem) + 1;
    
    
    int * genotype = populate_genotype(n, first_elem, last_elem);
    std::sort(genotype, genotype + n);
    
//    std::copy(genotype, genotype + n, genotype_with_parents);
//    for (size_t i = 0; i != n; ++i) {
//        std::cout << genotype_with_parents[i] << " " << genotype[i] << std::endl;
//    }
    
    size_t counter = 0;
    while (counter != 100) {
        int * genotype_with_parents = new int[(n + m * 2)];
        std::copy(genotype, genotype + n, genotype_with_parents);
        counter++;
        // Выберем случайные пары для скрещивания
        size_t * indexes = new size_t[m * 2];
        indexes = get_random_indexes(n, m * 2);
        for (size_t i = 0; i != m * 2; i += 2) {
            //std::cout << genotype[i] << " "  << genotype[i + 1] << std::endl;
            crossing(genotype[indexes[i]], genotype[indexes[i + 1]], genotype_with_parents, n + i, -first_elem, range);
            //std::cout << genotype[i] << " "  << genotype[i + 1] << std::endl;
        }
    
        size_t * indexes_to_mutate = new size_t[mut];
        indexes_to_mutate = get_random_indexes(m, mut);
    
        for (size_t i = 0; i != mut; ++i) {
            mutate(genotype_with_parents[n + indexes_to_mutate[i]], -first_elem);
        }
        reduce(genotype_with_parents, n + m, genotype, n);
        
//        std::cout << "start" << std::endl;
//        for (size_t i = 0; i != n + m * 2; ++i) {
//            std::cout << get_y(genotype_with_parents[i]) << " " << genotype_with_parents[i] << std::endl;
//        }
//        std::cout << "mid" << std::endl;
//        for (size_t i = 0; i != n; ++i) {
//            std::cout << get_y(genotype[i]) << " " << genotype[i] << std::endl;
//        }
//        std::cout << "end" << std::endl;
        
        double avg = 0;
        for (size_t i = 0; i != n; ++i) {
            avg += get_y(genotype[i]);
//            std::cout << get_y(genotype[i]) << std::endl;
        }
        avg /= n;
        std::cout << avg << std::endl;
    }
    
//    for (size_t i = 0; i != n; ++i) {
//        std::cout << genotype[i] << std::endl;
//    }

    
    return 0;
}

double get_y(int x) {
    int a = 55, b = -2, c = -111, d = 3;
    return a + (b * x) + (c * std::pow(x, 2)) + (d * std::pow(x, 3));
}

void reduce(int * genotype_with_parents, size_t size_gen_par, int * genotype, size_t size_gen) {
    std::vector<Indval> indval_arr;
    
    for (size_t i = 0; i != size_gen_par; ++i) {
        int x = genotype_with_parents[i];
//        std::cout << get_y(x) << std::endl;
        indval_arr.push_back(Indval(get_y(x), i));
    }
    
    std::sort(indval_arr.begin(), indval_arr.end(), compare);
    
    for (size_t i = 0; i != size_gen; ++i) {
        genotype[i] = genotype_with_parents[indval_arr[i].index_];
    }
}

void mutate(int & fenotype_item, int negative) {
    const int log = 6;
    
    int val = fenotype_item + negative;
    std::bitset<log> bitval(val);
    
//    srand (time(NULL));
    size_t pos = rand() % staticlog;
    
    bitval.flip(pos);
    fenotype_item = (int)bitval.to_ulong() - negative;
    
}

void crossing(int & left, int & right, int * genotype_with_parents, size_t from, int negative, size_t range) {
    int l_val = left + negative;
    int r_val = right + negative;
    
//    int log = std::log2(range);
    // Вместо log используем статическую, потому что шаблон принимает на вход constexpr - т.е. размер долен быть известен на момент компиляции
    
    std::bitset<staticlog> l_bitset(l_val);
    std::bitset<staticlog> r_bitset(r_val);
    
//    std::cout << l_bitset << " " << r_bitset << std::endl;
    
//    srand (time(NULL));
    size_t shift = rand() % (staticlog - 1) + 1;
//    std::cout << shift << "endl" <<std::endl;
    
    std::bitset<staticlog> new_l_bitset = (l_bitset & r_bitset) << shift;
    std::bitset<staticlog> new_r_bitset = (l_bitset | r_bitset) << shift;
    
//    std::cout << new_l_bitset << " " << new_r_bitset << std::endl;
    
    genotype_with_parents[from] = (int)new_l_bitset.to_ulong() - negative;
    genotype_with_parents[from + 1] = (int)new_r_bitset.to_ulong() - negative;
}

// n - число особей всего, m2 - необходимое число особей
size_t * get_random_indexes(size_t n, size_t m2) {
    size_t * indexes = new size_t[n];
    size_t values[n];
    for (size_t i = 0; i != n; ++i) {
        values[i] = i;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(values, values + n, gen);
    
    for (size_t i = 0; i != m2; ++i) {
        indexes[i] = values[i];
    }
    
    return indexes;
}

int * populate_genotype(size_t size, int from, int to) {
    int * genotype = new int[(size)];
    
    for (size_t i = 0; i != size; ++i) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(from, to);
        genotype[i] = dis(gen);

    }
    return genotype;
}

bool compare(const Indval & left, const Indval & right) {
    return left.val_ > right.val_;
}
