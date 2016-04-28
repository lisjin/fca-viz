//
//  concept.cpp
//  FCA_viz
//

#include "concept.h"
#include "set_ops.h"

//HELPER FUNCTIONS
static bitset<MAX> calcInter(const bitset<MAX> A, int i) {
    bitset<MAX> inter;
    for (int j = 0; j < i; ++j) { //initialize bits [0, i - 1]
        inter.set(j);
    }
    inter &= A;
    return inter;
}

static bool isNext(const bitset<MAX> prev, const bitset<MAX> A, int i) {
    return prev == calcInter(A, i);
}

static pair<bitset<MAX>, bitset<MAX> > nextClos(const vector<bitset<MAX> > in,
                                                BitObj &cur, int i,
                                                int num_obs) {
    bitset<MAX> inter, res, old_inter;
    res.flip();
    cur.A = calcInter(cur.A, i);
    old_inter = cur.A;
    if (!cur.A[i]) { //set union
        cur.A.set(i);
    }
    for (int j = 0; j < in.size(); ++j) { //store common attributes in res
        if (cur.A[j])
            res &= in[j];
    }
    for (int j = 0; j < in.size(); ++j) { //find extent
        if (!cur.A[j] && isSubset(in[j], res)) {
            cur.A.set(j);
        }
    }
    return make_pair(old_inter, res); //pair(extent, intent)
}

static vector<bitset<MAX> > parseBitsets(vector<string> input,
                                         int num_obs, int num_ats) {
    vector<bitset<MAX> > in(input.size());
    transform(input.begin(), input.end(), in.begin(), [](string &s) {
        reverse(s.begin(), s.end());
        bitset<MAX> b(s);
        return b;
    });
    for (auto b : in)
        printSet(b, num_ats);
    return in;
}

//PRIMARY FUNCTIONS
void findConc(vector<string> input, unordered_set<bitset<MAX> > &concepts,
              vector<BitObj*> &extents) {
    int num_obs = (int)input.size(),
        num_ats = (int)input[0].length();
    cout << "Input:\n";
    vector<bitset<MAX> > in = parseBitsets(input, num_obs, num_ats);
    
    BitObj cur;
    bitset<MAX> intent, stop;
    for (int i = 0; i < num_ats; ++i) { //fill all attributes
        intent.set(i);
    }
    for (int i = 0; i < num_obs; ++i) { //set stopping condition (all objects), find initial A
        stop.set(i);
        if (in[i] == intent) {
            cur.A.set(i);
            cur.setWeight(num_obs);
        }
    }
    cur.B = intent;
    concepts.insert(cur.A);
    BitObj *b = new BitObj(cur.A, cur.B, 0);
    extents.push_back(b);
    bitset<MAX> prev;
    pair<bitset<MAX>, bitset<MAX> > result;
    while (cur.A != stop) {
        for (int n = num_obs - 1; n >= 0; --n) {
            result = nextClos(in, cur, n, num_obs);
            prev = result.first;
            if (concepts.find(cur.A) == concepts.end() &&
                isNext(prev, cur.A, n)) {
                cur.setWeight(num_obs);
                cur.B = result.second;
                concepts.insert(cur.A);
                BitObj *b = new BitObj(cur.A, cur.B, cur.weight);
                extents.push_back(b);
                //REMOVE
                printObj(cur.A, cout, num_obs);
                //REMOVE
                break;
            }
            else
                cur.A = prev;
        }
    }
}

void findEdges(vector<BitObj*> &extents, vector<bitset<MAX> > &relation,
               ofstream &edges) {
    sort(extents.begin(), extents.end(), BComp);
    cout << "\nSorted concepts:\n";
    for (auto e : extents) {
        printObj(e->A, cout, (int)extents.size());
    }
    for (int i = 0; i < extents.size(); ++i) {
        for (int j = i + 1; j < extents.size(); ++j) {
            if (isSubset(extents[j]->A, extents[i]->A)) {
                relation[i].set(j); //i is a subset of j
            }
        }
    }
    for (int k = 0; k < relation.size(); ++k) {
        for (int i = 0; i < relation.size(); ++i) {
            for (int j = 0; j < relation.size(); ++j) {
                if (relation[i][j]) {
                    if (relation[i][k] && relation[k][j]) {
                        relation[i].set(j, false);
                    }
                }
            }
        }
    }
    cout << "\nWriting edges to edges.txt...\n";
    for (int i = 0; i < relation.size(); ++i) {
        for (int j = 0; j < relation.size(); ++j) {
            if (relation[i][j]) {
                if (extents[i]->weight < extents[j]->weight) {
                    extents[i]->parents.push_back(extents[j]);
                    extents[j]->children.push_back(extents[i]);
                }
                edges << i << ' ' << j << '\n';
            }
        }
    }
    edges.close();
}

void findCoords(vector<BitObj*> &extents, ofstream &coords) {
    int num_rows = 0;
    vector<vector<int> > weighted(extents.back()->weight + 1);
    for (int i = 0; i < extents.size(); ++i) {
        weighted[extents[i]->weight].push_back(i);
    }
    for (auto row : weighted) {
        if (row.size())
            ++num_rows;
    }
    int row_w = (int)extents.size() * SPACE;
    CoordXY xy_set(row_w, row_w / num_rows);
    int y_idx = 0;
    for (int i = 0; i < weighted.size(); ++i) {
        int num_conc = (int)weighted[i].size();
        for (int j = 0; j < num_conc; ++j) {
            extents[weighted[i][j]]->coord = xy_set(num_conc, y_idx, j);
        }
        if (num_conc)
            ++y_idx;
    }
    cout << "Writing coords to coords.txt...\n";
    coords << extents.size() << '\n';
    for (auto e : extents) {
        coords << e->coord.first << ' ' << e->coord.second << '\n';
    }
}

static void nodeJSON(vector<BitObj*> &extents, ofstream &json) {
    json << "{\n" << "  \"nodes\":[\n";
    for (int i = 0; i < extents.size(); ++i) {
        json << '\t' << "{\"name\":\"" << i << "\",\"group\":" <<
        extents[i]->weight << '}';
        if (i < extents.size() - 1)
            json << ',';
        json << '\n';
    }
    json << "  ],\n" << "  \"links\":[\n";
}

static void nodeObs(vector<BitObj*> &extents, ofstream &obs, int num_obs) {
    cout << "Writing node objects to obs.txt...\n";
    for (int i = 0; i < extents.size(); ++i) {
        printObj(extents[i]->A, obs, num_obs);
    }
}

static void linkJSON(ofstream &json) {
    ifstream edges0("edges.txt");
    string line;
    int src, tar;
    if (edges0.is_open()) {
        while (getline(edges0, line)) {
            stringstream ss(line);
            ss >> src >> tar;
            json << '\t' << "{\"source\":" << src << ",\"target\":" <<
            tar << ",\"value\":" << 1 << '}';
            if (!edges0.peek() == edges0.eof())
                json << ',';
            json << '\n';
        }
    }
    json << "  ]\n}";
}

int main(int argc, const char * argv[]) {
    ifstream fstream("input.txt");
    ofstream edges("edges.txt");
    ofstream coords("coords.txt");
    ofstream obs("obs.txt");
    ofstream json("fca.json");
    vector<string> input;
    string line;
    if (fstream.is_open()) {
        while (getline(fstream, line)) {
            input.push_back(line);
        }
        fstream.close();
        int num_obs = (int)input.size();
        unordered_set<bitset<MAX> > concepts;
        vector<BitObj*> extents;
        findConc(input, concepts, extents);
        vector<bitset<MAX> > relation(concepts.size());
        nodeJSON(extents, json);
        findEdges(extents, relation, edges);
        linkJSON(json);
        findCoords(extents, coords);
        nodeObs(extents, obs, num_obs);
        
        //Set operations
        ifstream com("com.txt");
        cout << "Computing operations...\n";
        string line;
        while (getline(com, line)) {
            cout << '\n';
            bitset<MAX> A = convInput(line.substr(2), num_obs);
            cout << "Input A: ";
            printObj(A, cout, num_obs);
            switch (line[0]) {
                case 'c':
                    cout << "Closure of A: ";
                    printObj(closure(extents, A).second, cout, num_obs);
                    break;
                case 'b':
                    cout << "Boundary of A: ";
                    printObj(boundary(extents, A, num_obs).second, cout, num_obs);
                    break;
                case 'i':
                    cout << "Interior of A: ";
                    printObj(interior(extents, A, num_obs), cout, num_obs);
                    break;
                case 'd':
                    cout << "Derived set of A: ";
                    printObj(derived(extents, A, num_obs), cout, num_obs);
                    break;
                case 's':
                    cout << "Isolated point of A: ";
                    printObj(isolated(extents, A, num_obs), cout, num_obs);
                    break;
            }
        }
        for (BitObj *b : extents)
            delete b;
    }
    else
        cerr << "Unable to open file" << endl;
    return 0;
}
