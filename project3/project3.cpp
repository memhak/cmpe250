/*
Student Name: Mehmet Hakan Kurtoglu
Project Number: 3
Operating System: Xubuntu
Compile Status: Compiled
Program Status: Working
Notes: Anything you want to say about your code that will be helpful in the grading process.

*/

#include <iostream>
#include <algorithm>
#include <fstream>
#include <list>
#include <queue>
#include <iomanip>
#include <stack>

using namespace std;

// struct representing processes
struct Process{
    // time needed to process this
    double time;
};
// graph struct to represent the system, vertices and edges correspond
// to tasks and dependencies, respectively.
struct Graph{

    // number of vertices
    int v;
    // pointer to process array
    Process *p;
    // pointer to adjacency lists array
    list<int> *adj;
    // this contains vertices with no indegrees
    vector<int> nullDegrees;
    // constructor
    Graph(int _v){
        v = _v;
        adj = new list<int>[_v];
        p = new Process[_v];
    }
    // function to add new edge
    void insertEdge(int u, int v){
        adj[u].push_back(v);
    }
    // function for computing indegrees
    void getInDegrees();
    // returns minimum time to finish all tasks
    double minTime();
    // helper function to above, finds longest path
    double longestPath(double *distance, int i);

};

// traverses the graph, stores vertices with no indegrees
void Graph::getInDegrees() {
    //this vector stores indegrees of vertices, all initialized as 0's
    //and then corrected.
    vector<int> inDegree(v,0);
    for(int j=0;j<v;j++){
        list<int>::iterator i;
        for(i=adj[j].begin();i!=adj[j].end();i++){
            inDegree[*i]++;
        }
    }
    // vertices with no indegrees stored in nullDegrees
    for(int i=0;i<v;i++){
        if(inDegree[i]==0)
            nullDegrees.push_back(i);
    }
}
// returns minimum time to finish all tasks, which corresponds to longest path
// in the graph
double Graph::minTime() {
    // vertices with no indegrees are computed, since they're roots, we only
    // need to start the paths from such vertices
    getInDegrees();
    // all distances are initialized as negative infinity,
    // distance of a vertex corresponds to distance with respect to root, summed
    // up as process times of each vertex
    double distance[v];
    std::fill_n(distance,v,-INFINITY);
    vector<int>::iterator it = nullDegrees.begin();
    // each vertex is visited
    for(;it<nullDegrees.end();it++){
        if(distance[*it]==-INFINITY){
            longestPath(distance,*it);
        }
    }
    return *std::max_element(distance,distance+v);
}
// returns longest path
double Graph::longestPath(double *distance, int i) {
    double max=0;
    // adjacent vertices are visited recursively
    list<int>::iterator it=adj[i].begin();
    for(;it!=adj[i].end();it++){
        if(distance[*it]==-INFINITY){
            longestPath(distance,*it);
        }
        max=std::max(max,distance[*it]);
    }
    distance[i]=max+p[i].time;
    return distance[i];
}


int main (int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Run the code with the following command: ./project1 [input_file] [output_file]" << endl;
        return 1;
    }

    cout << "input file: " << argv[1] << endl;
    cout << "output file: " << argv[2] << endl;

    ifstream infile(argv[1]);

    // reading from the file
    int v,e;
    infile>>v;
    infile>>e;
    Graph g(v);
    for(int i=0;i<v;i++)
        infile>>g.p[i].time;
    // vertices second being dependent on the first one
    int a,b;
    for(int i=0;i<e;i++){
        infile>>a;
        infile>>b;
        g.insertEdge(a,b);
    }

    ofstream myfile;
    myfile.open(argv[2]);
    double min = g.minTime();
    // if minTime is -Inf, it means that the graph is cyclic and not a DAG
    if(min==-INFINITY)
        min=-1;
    myfile<<fixed<<setprecision(6)<<min<<endl;
    myfile.close();
}