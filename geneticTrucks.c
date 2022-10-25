//geneticTrucks.c
//genetic algorithm for capacitated vehicle routing problem
//made by Renan Frantz

#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <random>
#include <algorithm>
#include <experimental/random>
using namespace std;

const int numTrucks = 9;
const int numNodes = 55;
const int crossoverChance = 50;
const int mutationChance = 5;

int allNodes[numNodes][4];
double distanceBetweenNodes[numNodes][numNodes];


struct Team{
    int eachTruckSize[numTrucks];
    int sequenceNodes[numNodes][4];
};

void readFile();
double calculateDistanceNodes(int node1[4], int node2[4]);
void calculateAllDistances();
tuple<Team,Team> crossOver(Team team1, Team team2);
Team mutate(Team team);
double calculateFitness(Team team);
Team makeRandomTeam();

//debugging function
void printTeam(Team team);


int main(){
    readFile();
    calculateAllDistances();
    Team test = makeRandomTeam();
    Team test2 = makeRandomTeam();

    printTeam(test);

    cout<<"--------------------"<<endl;

    printTeam(test2);

    cout<<"===================="<<endl;

    auto children = crossOver(test,test2);
   
    Team child1 = get<0>(children);
    printTeam(child1);

    cout<<"--------------------"<<endl;

    Team child2 = get<1>(children);
    printTeam(child2);
    
    //Team childtest = mutate(test);

    //printTeam(childtest);

}

void printTeam(Team team){
    /*
    cout<<"sizes of the trucks\n";

    for(int i = 0; i<numTrucks; i++){
        cout << team.eachTruckSize[i] << "|";
    }
    cout<<"\n------------------\n";
*/
    cout<<"sequence of nodes\n";
    for(int i = 0; i<numNodes; i++){
        cout << team.sequenceNodes[i][0] << "|";
    }
}

void readFile(){

    string nodeString;
    ifstream nodesFile ("nodes_clean.txt");

    for (int i = 0; nodesFile; i++){
        getline(nodesFile, nodeString);

        stringstream ss(nodeString);
        string nodeAttr;

        for (int j = 0; ss >> nodeAttr;j++) {
            allNodes[i][j] = stoi(nodeAttr);
        }

    }

} 

double calculateDistanceNodes(int node1[4], int node2[4]){
    return sqrt(pow(node1[2] - node2[2],2) + pow(node1[3] - node2[3],2));
}

void calculateAllDistances(){
    for (int i = 0;i < numNodes; i++){
        //can optimize by making j = i+1 (will only calculate distances one time), but that makes using the array more restrictive, and computiationally it's a marginal difference with this number of nodes
       for (int j = 0;j < numNodes; j++){
            distanceBetweenNodes[i][j] = calculateDistanceNodes(allNodes[i], allNodes[j]);
       } 
    }
}

tuple<Team,Team> crossOver(Team team1, Team team2){
    Team childteam1;
    Team childteam2;
        
    int substring1[4];
    int substring2[4];

    if (experimental::randint(0,99) < crossoverChance){

        int beginIndexSubstring = experimental::randint(0,numNodes-5);
        int endIndexSubstring = beginIndexSubstring+5;

        for(int i = beginIndexSubstring; i < endIndexSubstring;i++){
            substring1[i] = team1.sequenceNodes[i][0];
            substring2[i] = team2.sequenceNodes[i][0];

            //copy randomly selected subsequence to children
            for(int j = 0; j<4; j++){
                childteam1.sequenceNodes[i][j] = team1.sequenceNodes[i][j];
                childteam2.sequenceNodes[i][j] = team2.sequenceNodes[i][j];
            }
        }

        //copy rest of the sequence from other parent
        for(int i = 0; i<numNodes; i++){

            //if node not in other parent's substring, append to child

        }


    }else{
        childteam1 = team1;
        childteam2 = team2;
    }

    return make_tuple(childteam1,childteam2);
}

Team mutate(Team team){
    Team childteam;
    for(int i = 0; i < numNodes; i++){
        for (int j = 0; j < 4; j++){
            childteam.sequenceNodes[i][j] = team.sequenceNodes[i][j];
        }
    }

    for(int i = 0; i < numNodes-4;i+=5){
        //check for mutation every 5 nodes
        if(experimental::randint(0,99) < mutationChance){
            cout<<"mutated at: "<<i<<"\n";
            cout<<"========="<<endl;
            //go through the five nodes if mutation happened and shuffle them
            for(int j = i; j<i+5; j++){

                int randnumber = experimental::randint(i,i+4);
                int aux[4];

                for (int k = 0; k<4; k++){
                    aux[k] = childteam.sequenceNodes[j][k];
                }

                for (int k = 0; k<4; k++){
                    childteam.sequenceNodes[j][k] = childteam.sequenceNodes[randnumber][k];
                }

                for (int k = 0; k<4; k++){
                    childteam.sequenceNodes[randnumber][k] = aux[k];
                }

            }
        }
    }

    return childteam;
}

double calculateFitness(Team team){
    double totalDistance = 0;

    for(int i = 0; i < numTrucks;i++){

        int totalCargo = 0;

        for(int j = 0; j < team.eachTruckSize[i]; j++){

            totalCargo += team.sequenceNodes[j][1];

            if(totalCargo > 100){

                totalDistance -= 10000.0;

            }else if(j % 2 == 0){

                totalDistance -= *distanceBetweenNodes[team.sequenceNodes[j][0]-1,team.sequenceNodes[j+1][0]-1];
            } 
        }
    }

    return totalDistance;

}

//generates random numbers for each truck without going over the total numNodes
//copies all nodes to sequenceNodes of the team and then shuffles
Team makeRandomTeam(){
    Team generatedTeam;
    int lastTruckSize = 0;
    
    //works in theory but result is completely retarted, hopefully GA will fix it
    for (int i = numTrucks-1; i>=0; i--){
        int randsize = experimental::randint(1,numNodes-2*i-lastTruckSize);
        generatedTeam.eachTruckSize[i] = randsize;
        lastTruckSize += randsize;
    }

    //copy allnodes to team
    for (int i = 0; i<numNodes;i++){
        for (int j = 0; j<4;j++){
            generatedTeam.sequenceNodes[i][j] = allNodes[i][j];
        }
    }

    //shuffle team's nodes
    for(int i = 0; i < numNodes; i++){
        int randnumber = experimental::randint(0,numNodes-1);

        int aux[4];
        for (int j = 0; j<4; j++){
            aux[j] = generatedTeam.sequenceNodes[i][j];
        }

        for (int j = 0; j<4; j++){
           generatedTeam.sequenceNodes[i][j] = generatedTeam.sequenceNodes[randnumber][j];
        }

        for (int j = 0; j<4; j++){
           generatedTeam.sequenceNodes[randnumber][j] = aux[j];
        }
    }

    return generatedTeam;
}