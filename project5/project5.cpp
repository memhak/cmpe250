/*
Student Name: Mehmet Hakan Kurtoglu
Project Number: 5
Operating System: Xubuntu
Compile Status: Compiled
Program Status: Works, I guess
Notes: Anything you want to say about your code that will be helpful in the grading process.

*/

#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <queue>
#include <set>

using namespace std;

// struct to sort vertices in ascending order with respect to estimated distances
struct LessThanByEstDist{
    bool operator()(const pair<int,int>& lhs, const pair<int,int>& rhs) const
    {
        return lhs.second > rhs.second;
    }
};

// this struct represents the whole graph
struct Graph{
    int *heuristics;
    int size;
    // edges are stored in an adjacency matrix
    int **adjacencyMatrix;
    // stores whether the vertex is visited
    bool *visited;
    Graph(int size_){
        size=size_;
        heuristics=new int[size];
        adjacencyMatrix=new int*[size];
        visited=new bool[size];
        for (int i = 0; i < size; ++i) {
            visited[i]=false;
            adjacencyMatrix[i]=new int[size];
        }
    }
    // adds a new edge
    void addEdge(int i,int j,int dist){
        adjacencyMatrix[i][j]=dist;
        adjacencyMatrix[j][i]=dist;
    }
};

int main (int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Run the code with the following command: ./project1 [input_file] [output_file]" << endl;
        return 1;
    }

    cout << "input file: " << argv[1] << endl;
    cout << "output file: " << argv[2] << endl;

    ifstream infile(argv[1]);

    // constructs graph
    int size,road;
    int h;
    infile>>size;
    Graph graph(size);
    infile>>road;
    for(int i=0;i<size;i++){
        infile>>h;
        graph.heuristics[i]=h;
    }
    int j,k,dist;
    for(int i=0;i<road;i++){
        infile>>j;infile>>k;infile>>dist;
        graph.addEdge(j,k,dist);
    }
    int source,target;
    infile>>source;infile>>target;

    // this stores vertices, sorted with respect to estimated distance of the path
    // that goes through the vertex
    priority_queue<pair<int,int>,vector<pair<int,int>>,LessThanByEstDist> pq;
    pq.push(make_pair(source,graph.heuristics[source]));

    // real distances to root
    int realDist[size];
    // heuristics
    int estDist[size];
    // if the vertex is in the priority queue
    bool notInQueue[size];
    for (int l = 0; l < size; ++l) {
        notInQueue[l]=true;
        estDist[l]=INFINITY;
        realDist[l]=INFINITY;
    }
    // initially distance to source is zero
    realDist[source]=0;
    // source is in the queue
    notInQueue[source]=false;
    // estimated distance of the source is initially pure heuristic
    estDist[source]=graph.heuristics[source];
    pair<int,int> curr;
    int temp;
    while(pq.size()>0){
        curr=pq.top();
        pq.pop();
        // if current vertex is the target, then we've found the shortest path
        if(curr.first==target){break;}
        graph.visited[curr.first]=true;
        for(int i=0;i<size;i++){
            if(graph.adjacencyMatrix[curr.first][i]!=0){
                temp=realDist[curr.first]+graph.adjacencyMatrix[curr.first][i];
                // if vertex is visited, it doesn't get processed
                if(!graph.visited[i]){
                    // if newly calculated distance is smaller than before
                    // found distance
                    if(notInQueue[i]){
                        if(temp<=realDist[i]){
                            realDist[i]=temp;
                            estDist[i]=realDist[i]+graph.heuristics[i];
                            pq.push(make_pair(i,estDist[i]));
                            notInQueue[i]=false;
                        }
                    }
                }
                // if a better path is found
                else if(!notInQueue[i]){
                    if(temp<=realDist[i]){
                        realDist[i]=temp;
                        estDist[i]=realDist[i]+graph.heuristics[i];
                        pq.push(make_pair(i,estDist[i]));
                    }
                }
            }
        }
    }
    ofstream myfile;
    myfile.open(argv[2]);
    myfile<<realDist[target]<<endl;
    myfile.close();
    return 0;

}