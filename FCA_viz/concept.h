//
//  concept.h
//  FCA_viz
//

#ifndef concept_h
#define concept_h

#include <vector>
#include <numeric>
#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <sstream>

using namespace std;
const int MAX = 50;
const int SPACE = 5;

struct BitObj {
    vector<BitObj*> parents;
    vector<BitObj*> children;
    bitset<MAX> A;
    bitset<MAX> attr;
    pair<int, int> coord;
    int weight;
    BitObj() : A(0), weight(0) {}
    BitObj(bitset<MAX> A, bitset<MAX> a, int w) : A(A), attr(a), weight(w) {}
    void setWeight(int num_obs) {
        this->weight = 0;
        for (int h = 0; h < num_obs; ++h) {
            if (this->A[h])
                ++this->weight;
        }
    }
};

struct BCompare {
    bool operator() (const BitObj *lhs, const BitObj *rhs) {
        if (lhs->weight != rhs->weight)
            return lhs->weight < rhs->weight;
        else
            return lhs->A.to_ulong() < rhs->A.to_ulong();
    }
} BComp;

struct CoordXY {
    CoordXY(int rw, int cs) : row_w(rw), col_sp(cs) {}
    pair<int, int> operator()(int num_conc, int y_idx, int x_idx) {
        int row_sp = row_w / num_conc;
        int x_coord = row_sp * x_idx + row_sp / 2;
        int y_coord = col_sp * y_idx;
        return make_pair(x_coord, y_coord);
    }
private:
    int row_w;
    int col_sp;
};

void printSet(const bitset<MAX> in, int num_obs) {
    for (int i = 0; i < num_obs; ++i) {
        if (in[i])
            cout << in[i] << ' ';
        else
            cout << ". ";
    }
    cout << '\n';
}

void printObj(const bitset<MAX> in, ostream &os, int num_obs) {
    bool empty = 1;
    for (int i = 0; i < num_obs; ++i) {
        if (in[i]) {
            os << i + 1 << ' ';
            empty = 0;
        }
    }
    if (empty)
        os << "∅";
    os << '\n';
}

void printAts(const BitObj &cur, int num_ats) {
    char alpha[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n',
        'o','p','q','r','s','t','u','v','w','x','y','z'};
    cout << '\t' << "{ ";
    for (int i = 0; i < num_ats; ++i) {
        if (cur.attr[i])
            cout << alpha[i] << ' ';
    }
    cout << '}';
}

bool isSubset(const bitset<MAX> sup, const bitset<MAX> sub) {
    return (sup & sub) == sub;
}

#endif /* concept_h */
