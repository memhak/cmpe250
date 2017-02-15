/*
Student Name: Mehmet Hakan Kurtoglu
Project Number: 4
Operating System: Xubuntu
Compile Status: Compiled
Program Status: Work
Notes: Anything you want to say about your code that will be helpful in the grading process.

*/


#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <climits>

using namespace std;

// struct represents the graph
struct  Graph{
    int size;
    int *root;
    // this will contain minimum time to partition the cities
    int minTime;

    Graph(int size_){
        minTime=0;
        size=size_;
        // first roots of all cities are initialized as -1
        root = new int[size];
        for(int i=0;i<size;i++){
            root[i]=-1;
        }
    }
    // returns the root of the specified city, meaning the root of the sub-spanning
    // tree in which this city is bounded
    int findRoot(int city){
        if(root[city] < 0){
            return city;
        }else{
            return findRoot(root[city]);
        }
    }
    // connects specified cities
    void add(int city1,int city2){
        city1 = findRoot(city1);
        city2 = findRoot(city2);
        if(root[city1] < root[city2]){
            root[city2] = city1;
        }else if(root[city1] > root[city2]){
            root[city1] = city2;
        }else{
            root[city1]--;
            root[city2] = city1;
        }
    }

};

// paths are edges in the graph
struct Path{
    int city1;
    int city2;
    int weight;

    Path(int city1_,int city2_, int weight_){
        city1=city1_;
        city2=city2_;
        weight=weight_;
    }
};

// overloaded operator to sort the paths
bool operator<(const Path& p1, const Path& p2) {
    return p1.weight > p2.weight;
}

int main (int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Run the code with the following command: ./project1 [input_file] [output_file]" << endl;
        return 1;
    }

    cout << "input file: " << argv[1] << endl;
    cout << "output file: " << argv[2] << endl;

    ifstream infile(argv[1]);

    int size;
    infile>>size;
    int cutNumb;
    infile>>cutNumb;
    Graph graph(size);
    int j,k,dist;
    vector<Path> paths;
    for(int i=1;i<size;i++){
        infile>>j;
        infile>>k;
        infile>>dist;
        paths.push_back(Path(j,k,dist));
    }

    // sorts the paths in descending order, with respect to their weights
    sort(paths.begin(),paths.end());

    int t;
    for(int i=0;i<cutNumb;i++){
        infile>>t;
        // root of the cities to cut are defined as minimum integer value, to
        // distinguish them from the other cities
        graph.root[t]=INT_MIN;
    }
    int counter=0;
    auto itr=paths.begin();
    // using the kruskal's algorithm with respect to descending order of weights
    // paths are added to graph, the path that would connect the disjoint
    // sets which would seperately contain the cities that should be seperated, will
    // the path with minimum weight, which we're looking for
    while (counter < size-1) {
        int r1 = graph.findRoot(itr->city1);
        int r2 = graph.findRoot(itr->city2);
        counter++;
        if (r1 != r2 ){
            // if roots are both int_min, then graph should stay partitioned
            if(graph.root[r1]==INT_MIN && graph.root[r2]==INT_MIN){
                graph.minTime+=itr->weight;
            }else{
                // path is added to graph
                graph.add(r1,r2);
            }
        }
        itr++;
    }

    ofstream myfile;
    myfile.open(argv[2]);
    myfile<<graph.minTime<<endl;
    myfile.close();
    return 0;

}