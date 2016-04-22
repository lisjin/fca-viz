//
//  set_ops.h
//  FCA_viz
//

#ifndef set_ops_h
#define set_ops_h

#include <cassert>
#include <unordered_map>
#include "concept.h"

static bitset<MAX> complement(bitset<MAX> A, int num_obs) {
    bitset<MAX> A0;
    for (int i = 0; i < num_obs; ++i) {
        if (!A[i]) {
            A0.set(i);
        }
    }
    return A0;
}

bitset<MAX> convInput(const string &line, int num_obs) {
    bitset<MAX> res;
    string idx;
    stringstream ss(line);
    while (ss >> idx) {
        res.set(stoi(idx) - 1);
    }
    return res;
}

//Assumes that extents are sorted by increasing weight
pair<int, bitset<MAX> > closure(const vector<BitObj*> &extents, bitset<MAX> A) {
    int clos = -1;
    for (int i = 0; i < extents.size(); ++i) {
        if (isSubset(extents[i]->A, A)) {
            clos = i;
            break;
        }
    }
    assert(clos > -1);
    return make_pair(clos, extents[clos]->A);
}

pair<int, bitset<MAX> > boundary(const vector<BitObj*> &extents, bitset<MAX> A,
                                  int num_obs) {
    unordered_map<bitset<MAX>, int> bits_idx;
    for (int i = 0; i < extents.size(); ++i)
        bits_idx.emplace(extents[i]->A, i);
    bitset<MAX> A0 = complement(A, num_obs),
                res = closure(extents, A).second & closure(extents, A0).second;
    assert(bits_idx.find(res) != bits_idx.end());
    return make_pair(bits_idx[res], extents[bits_idx[res]]->A);
}

bitset<MAX> interior(const vector<BitObj*> &extents, bitset<MAX> A, int num_obs) {
    return closure(extents, A).second & ~boundary(extents, A, num_obs).second;
}

bitset<MAX> derived(const vector<BitObj*> &extents, bitset<MAX> A, int num_obs) {
    bitset<MAX> A0;
    for (int i = 0; i < num_obs; ++i) {
        bitset<MAX> A1 = A;
        A1.reset(i);
        if (closure(extents, A1).second[i])
            A0.set(i);
    }
    return A0;
}

bitset<MAX> isolated(const vector<BitObj*> &extents, bitset<MAX> A, int num_obs) {
    return A & ~derived(extents, A, num_obs);
}

#endif /* set_ops_h */
