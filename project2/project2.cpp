/*
Student Name: Mehmet Hakan Kurtoglu
Project Number: 2
Operating System: Xubuntu
Compile Status: Compiled
Program Status: Working
Notes: Anything you want to say about your code that will be helpful in the grading process.
    Fully commented discrete event simulation, for further info please
    read comments
*/

#include <iostream>
#include <algorithm>
#include <fstream>
#include <queue>
#include <iomanip>

using namespace std;

// struct for CPU's, contains necessary infos
struct CPU{
    double freq;
    int id;
    double activeTime;
    bool isAvailable;
};

// struct for Tasks
struct Task{
    double arrivalTime, exitTime;
    // time this task will sorted with respect to, this indicates event time
    // of this task
    double processTime;
    double CPUWork, outWork;
    int id;
    // indicates state of task, what kind of a process it will go through
    int type;
    // id of the cpu which processes this task
    int cpu_id;
    // time this task spends in CPU
    double cpuTime;
    double cpuArrTime;
    double cpuExTime;
    // total time this task spends in simulation
    double totalTime;
    // time this task spends waiting, in queues
    double waitTime;
    // id of the IO device this task last has been to
    int out_id;
    // time this task spends in IO devices
    double outTime;

};

// compare class for Tasks with respect to their process time
struct CompareProcessTime{
    bool operator()(Task& t, Task& a)
    {
        if(t.processTime==a.processTime){
            if(t.type==a.type){
                if(t.outWork==a.outWork)
                    return t.cpu_id>a.cpu_id;
                return t.outWork>a.outWork;
            }
            t.type<a.type;
        }
        return t.processTime>a.processTime;
    }
};

// compare class for tasks according to their CPU work
struct CompareCPUWork{
    bool operator()(Task& t, Task& a){
        return t.CPUWork>a.CPUWork;
    }
};

// struct for IO devices
struct OutPutDevice{
    int id;
    bool isAvailable;
    double timeQuant;
    // time this device spends active
    double activeTime;
};

// method to check if any availabe CPU exists
bool isCPUable(vector<CPU>& cpus){
    int size = cpus.size();
    for(int i=0;i<size;++i){
        if(cpus[i].isAvailable)
            return true;
    }
    return false;
}

// method to check if any IO device is available
bool isOutable(vector<OutPutDevice>& outs){
    int size = outs.size();
    for(int i=0;i<size;i++){
        if(outs[i].isAvailable)
            return true;
    }
    return false;
}

// method that sends specified task to available IO device and than updates
// event queue
void sendToOut(vector<OutPutDevice>& outs, Task& t, priority_queue<Task,
        vector<Task>,CompareProcessTime>& pq, double time){
    int size = outs.size();
    int j;
    for(int i=0;i<size;i++){
        if(outs[i].isAvailable){j=i;break;}
    }
    if(t.outWork>outs[j].timeQuant){
        t.outWork -= outs[j].timeQuant;
        outs[j].activeTime += outs[j].timeQuant;
        t.out_id=j;
        t.type=2;
        t.outTime +=outs[j].timeQuant;
        outs[j].isAvailable=false;
        t.processTime=time+outs[j].timeQuant;
        pq.push(t);
        return;
    }
    else{
        outs[j].isAvailable=false;
        outs[j].activeTime += t.outWork;
        t.type=3;
        t.outTime += t.outWork;
        t.processTime=time+t.outWork;
        t.outWork=0;
        t.out_id=j;
        pq.push(t);
        return;
    }
}

// method that sends specified task to available CPU and updates event queue
void sendToCPU(vector<CPU>& cpus, Task& t, priority_queue<Task,vector<Task>,
        CompareProcessTime>& pq,double time){
    double ptime;
    int j;
    int size = cpus.size();
    for(int i=0;i<size;++i){
        if(cpus[i].isAvailable){j=i;break;}
    }
    ptime=t.CPUWork/cpus[j].freq;
    t.cpuTime = ptime;
    t.cpuArrTime = time;
    t.type=1;
    t.cpu_id=cpus[j].id;
    cpus[j].isAvailable=false;
    t.processTime = time+ptime;
    t.cpuExTime = t.processTime;
    cpus[j].activeTime += ptime;
    pq.push(t);
}

int main (int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Run the code with the following command: ./project1 [input_file] [output_file]" << endl;
        return 1;
    }

    cout << "input file: " << argv[1] << endl;
    cout << "output file: " << argv[2] << endl;

    ifstream infile(argv[1]);

    // constructing cpus
    vector<CPU> cpus;
    int numb;
    infile>>numb;
    for(int i=0;i<numb;i++) {
        CPU a;
        cpus.push_back(a);
        cpus[i].id = i;
        cpus[i].activeTime=0;
        cpus[i].isAvailable=true;
        infile>>cpus[i].freq;
    }

    // constructing output devices
    infile>>numb;
    vector<OutPutDevice> outs;
    for(int i=0;i<numb;i++){
        OutPutDevice a;
        a.isAvailable = true;
        a.activeTime=0;
        outs.push_back(a);
        outs[i].id=i;
        infile>>outs[i].timeQuant;
    }

    // constructing tasks
    infile>>numb;
    // priority queue for tasks, tasks sorted according to their event times(process time)
    priority_queue<Task,vector<Task>,CompareProcessTime> pqEvent;
    for(int i=1;i<=numb;i++){
        Task a;
        a.id=i;
        a.type=0;
        infile>>a.arrivalTime;
        a.processTime = a.arrivalTime;
        a.outTime = 0;
        infile>>a.CPUWork;
        infile>>a.outWork;
        pqEvent.push(a);
    }

    // priority queue for first level processes
    priority_queue<Task,vector<Task>,CompareCPUWork> firstQueue;
    queue<Task> secondQueue;
    queue<Task> finished;

    // queues used to store lengths of previous queues after each update,
    // to find maximum lengths
    priority_queue<int> p1;
    priority_queue<int> p2;

    // time
    double time;
    // main loop
    while(!pqEvent.empty()){
        // event at top of the event queue
        Task e = pqEvent.top();
        pqEvent.pop();
        time = e.processTime;
        // for tasks of type 0, tasks that haven't been to a CPU yet
        // if an available CPU exists, task is processed otherwise goes to first
        // queue
        if(e.type==0){
            if(isCPUable(cpus)){
                sendToCPU(cpus,e,pqEvent,time);
            }else{
                firstQueue.push(e);
                p1.push(firstQueue.size());
            }
        }
        // for tasks of type 1, tasks that have been processed by a CPU,
        // if an IO device is available, tasks goes to IO device, otherwise
        // to second queue
        else if(e.type==1){
            cpus[e.cpu_id].isAvailable = true;
            if(isOutable(outs)){
                if(secondQueue.empty()){
                    sendToOut(outs,e,pqEvent,time);
                }
                else{
                    Task t = secondQueue.front();
                    secondQueue.pop();
                    sendToOut(outs,e,pqEvent,time);
                }
            }else{
                secondQueue.push(e);
                p2.push(secondQueue.size());
            }
            if(!(firstQueue.empty())){
                Task t = firstQueue.top();
                firstQueue.pop();
                sendToCPU(cpus,t,pqEvent,time);
            }
        }
        // for tasks of type 2, tasks that have been to an IO device
        // if no IO work left, tasks go to finished queue otherwise again to
        // second queue
        else if(e.type==2){
            outs[e.out_id].isAvailable=true;
            if(!isOutable(outs)){
                secondQueue.push(e);
                p2.push(secondQueue.size());
            }else{
                if(secondQueue.empty()){
                    sendToOut(outs,e,pqEvent,time);
                }else{
                    Task t=secondQueue.front();
                    secondQueue.pop();
                    sendToOut(outs,t,pqEvent,time);
                    secondQueue.push(e);
                    p2.push(secondQueue.size());
                }
            }
        }
        // tasks of type 3, go here, they go to finished queue
        else {
            outs[e.out_id].isAvailable=true;
            e.exitTime=e.processTime;
            e.totalTime = e.exitTime-e.arrivalTime;
            e.waitTime = e.totalTime-e.cpuTime;
            e.waitTime -= e.outTime;
            finished.push(e);
            if(!secondQueue.empty()){
                Task t=secondQueue.front();
                secondQueue.pop();
                sendToOut(outs,t,pqEvent,time);
            }
        }
    }

    // here CPU with most active time is found
    double cpumaxtime = 0;
    int cpumaxid=1;
    for(int i=0;i<cpus.size();i++){
        if(cpumaxtime<cpus[i].activeTime){
            cpumaxid=i+1;
            cpumaxtime=cpus[i].activeTime;
        }
    }

    // here IO device with most active time is found
    double outmaxtime = 0;
    int outmaxid=1;
    for(int i=0;i<outs.size();i++){
        if(outmaxtime<outs[i].activeTime){
            outmaxid=i+1;
            outmaxtime=outs[i].activeTime;
        }
    }

    // average time spend by a task in the simulation, average wait time and
    // longest wait time are found
    double totalTime = 0;
    double totalWaitTime = 0;
    double maxWaitTime = finished.front().waitTime;
    double size = finished.size();
    Task temp;
    for(int i=0;i<=size;i++){
        temp= finished.front();
        totalTime += temp.totalTime;
        totalWaitTime += temp.waitTime;
        if(maxWaitTime<temp.waitTime)
            maxWaitTime=temp.waitTime;
        finished.pop();
    }

    // prints the output to specifeid output file
    ofstream myfile;
    myfile.open(argv[2]);
    myfile<<fixed;
    myfile<<setprecision(6);
    myfile<<p1.top()<<endl;
    myfile<<p2.top()<<endl;
    myfile<<cpumaxid<<endl;
    myfile<<outmaxid<<endl;
    myfile<<totalWaitTime/size<<endl;
    myfile<<maxWaitTime<<endl;
    myfile<<totalTime/size<<endl;

    myfile.close();
}
