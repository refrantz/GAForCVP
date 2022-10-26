//geneticTrucks.c
//genetic algorithm for capacitated vehicle routing problem
//made by Renan Frantz

#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <random>
#include <algorithm>
#include <ctime>
#include <experimental/random>
using namespace std;

const int numTrucks = 9;
const int numNodes = 54;
const int crossoverChance = 50;
const int mutationChance = 10;
const int crossOverSize = 5;
const int genSize = 32;
const int tournamentSize = 6;
const int epochs = 1000000;

int depot[4];
int allNodes[numNodes][4];
double distanceBetweenNodes[numNodes][numNodes];
double distanceToDepot[numNodes];

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
bool contains(int e, int arr[]);
void calculateDistancesToDepot();

//debugging function
void printTeam(Team team);


int main(){
    //for(int i = 0; i<30;i++){
        clock_t c_start = clock();
        readFile();
        calculateAllDistances();
        calculateDistancesToDepot();

        //make first generation
        Team oldGen[genSize];
        Team newGen[genSize];

        for (int i = 0; i<genSize; i++){
            oldGen[i] = makeRandomTeam();
        }

        for(int epoch = 0; epoch < epochs; epoch++){
            //select two parents for every two elements in generation, then crossover them and mutate their children
            for (int i = 0; i<genSize-1; i+=2){

                int max1 = calculateFitness(oldGen[0]);
                int max1Index = 0;

                int max2 = calculateFitness(oldGen[0]);
                int max2Index = 0;

                //tournament1
                for(int j = 0; j<tournamentSize; j++){
                    int index = experimental::randint(0, genSize-1);
                    int participantFitness = calculateFitness(oldGen[index]);
                    if(participantFitness > max1){
                        max1 = participantFitness;
                        max1Index = index;
                    }
                }

                //tournament2
                for(int j = 0; j<tournamentSize; j++){
                    int index = experimental::randint(0, genSize-1);
                    int participantFitness = calculateFitness(oldGen[index]);
                    if(participantFitness > max2){
                        max2 = participantFitness;
                        max2Index = index;
                    }
                }

                auto children = crossOver(oldGen[max1Index], oldGen[max2Index]);

                newGen[i] = mutate(get<0>(children));
                newGen[i+1] = mutate(get<1>(children));

            }

            //make sure the best is passed through and update old gen;
            for (int i = 0; i<genSize; i++){

    /* guarantees the best never gets dies but makes the algorithm 3x slower for some reason
                //get worst team index from newGen
                int min = calculateFitness(newGen[0]);
                int minTeamIndex = 0;
                for(int i = 0; i < genSize; i++){
                    if(calculateFitness(newGen[i])<min){
                        minTeamIndex = i;
                    }
                }

                //get best team from oldGen
                int max = calculateFitness(newGen[0]);
                Team maxTeam = newGen[0];
                for(int i = 0; i < genSize; i++){
                    if(calculateFitness(oldGen[i])>max){
                        maxTeam = oldGen[i];
                    }
                }

                if(max > min){
                    newGen[minTeamIndex] = maxTeam;
                }
    */

                oldGen[i] = newGen[i];
            }

            if(epoch % 100000 == 0){
                cout<<calculateFitness(newGen[0])<<endl;
            }
        }

        //get best from last generation
        int max = calculateFitness(newGen[0]);
        Team maxTeam = newGen[0];
        for(int i = 0; i < genSize; i++){
            if(calculateFitness(newGen[i])>max){
                maxTeam = newGen[i];
            }
        }
        cout<<"\n"<<calculateFitness(maxTeam)<<endl;
        printTeam(maxTeam);

        clock_t c_end = clock();

        double time_elapsed_ms = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
        cout << "CPU time used: " << time_elapsed_ms /1000<< " ms\n";
    //}

}

void printTeam(Team team){
    int startIndex = 0;
    
    for(int i = 0; i < numTrucks;i++){
        cout << "\nSizes: " << team.eachTruckSize[i] <<endl;
        double totalDistance = 0;

        int totalCargo = 0;

        if(i==0){
            startIndex == 0;
        }else{
            startIndex += team.eachTruckSize[i-1];
        }

        totalDistance -= distanceToDepot[team.sequenceNodes[startIndex][0]-2];
        
        for(int j = startIndex; j < startIndex+team.eachTruckSize[i]-1; j++){

            cout << "|" << team.sequenceNodes[j][0]-1 << "|";
            totalCargo += team.sequenceNodes[j][1];

            totalDistance -= distanceBetweenNodes[team.sequenceNodes[j][0]-2][team.sequenceNodes[j+1][0]-2];
                
        }
        cout << "|" << team.sequenceNodes[startIndex+team.eachTruckSize[i]-1][0] << "|";
        totalCargo += team.sequenceNodes[startIndex+team.eachTruckSize[i]-1][1];

        cout << "\nTotal cargo shipped: " << totalCargo;
        cout << "\nTotal distance traveled: " << totalDistance;
        cout << "\n----------------------------" << endl;

    }
}

bool contains(int e, int arr[]){
    for (int i = 0; i<crossOverSize; i++){
        if (arr[i] == e){
            return true;
        }
    }
    return false;
}

void readFile(){

    string nodeString;
    ifstream nodesFile ("nodes_clean.txt");

    //get depot
    getline(nodesFile, nodeString);

    stringstream ss(nodeString);
    string nodeAttr;

    for (int j = 0; ss >> nodeAttr;j++) {
        depot[j] = stoi(nodeAttr);
    }

    //getallNodes
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

void calculateDistancesToDepot(){
    for (int i = 0;i < numNodes; i++){
        distanceToDepot[i] = calculateDistanceNodes(depot, allNodes[i]);
    }
}

tuple<Team,Team> crossOver(Team team1, Team team2){
    Team childteam1 = team1;
    Team childteam2 = team2;
        
    int substring1[crossOverSize];
    int substring2[crossOverSize];

    if (experimental::randint(0,99) < crossoverChance){

        int beginIndexSubstring = experimental::randint(0,numNodes-crossOverSize);
        int endIndexSubstring = beginIndexSubstring+crossOverSize;
        int stringIndex = 0;
        //copy randomly selected subsequence to children
        for(int i = beginIndexSubstring; i < endIndexSubstring;i++){
            substring1[stringIndex] = team1.sequenceNodes[i][0];
            substring2[stringIndex] = team2.sequenceNodes[i][0];
            stringIndex++;

            for(int j = 0; j<4; j++){
                childteam1.sequenceNodes[i][j] = team1.sequenceNodes[i][j];
                childteam2.sequenceNodes[i][j] = team2.sequenceNodes[i][j];
            }
        }

        //copy rest of the sequence from other parent
        int k1 = 0;
        int k2 = 0;
        for(int i = 0; i<numNodes; i++){

            //if arrived at substring inherited from other parent, skip
            if(contains(childteam1.sequenceNodes[k1][0],substring1)){
                if(k1+crossOverSize < numNodes){
                    k1+=crossOverSize;
                }else{
                    break;
                }
            }

            //if node not in other parent's substring, append to child
            if(!contains(team2.sequenceNodes[i][0],substring1)){
                for(int j = 0; j<4; j++){
                   childteam1.sequenceNodes[k1][j] = team2.sequenceNodes[i][j];
                }
                k1++;
            }

            //same thing but for child 2
            if(contains(childteam2.sequenceNodes[k2][0],substring2)){
                if(k2+crossOverSize < numNodes){
                    k2+=crossOverSize;
                }else{
                    break;
                }
            }

            if(!contains(team1.sequenceNodes[i][0],substring2)){
                for(int j = 0; j<4; j++){
                   childteam2.sequenceNodes[k2][j] = team1.sequenceNodes[i][j];
                }
                k2++;
            }
        }


    }

    return make_tuple(childteam1,childteam2);
}

Team mutate(Team team){
    Team childteam = team;

    //mutation of the sequence
    for(int i = 0; i < numNodes-4;i+=5){
        //check for mutation every 5 nodes
        if(experimental::randint(0,99) < mutationChance){
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

    int total = 0;
    //mutation of the truck sizes
    for(int i = 0; i < numTrucks;i++){
        if(experimental::randint(0,99) < mutationChance){

            int indexToTakeFrom = experimental::randint(0, numTrucks-1);
            int quantityToExchange = experimental::randint(0,min(5,childteam.eachTruckSize[indexToTakeFrom])-1);

            childteam.eachTruckSize[indexToTakeFrom] -= quantityToExchange;
            childteam.eachTruckSize[i] += quantityToExchange;
            

        } 
    }


    return childteam;
}

double calculateFitness(Team team){
    double totalDistance = 0;
    int startIndex = 0;

    for(int i = 0; i < numTrucks;i++){

        int totalCargo = 0;
        if(i==0){
            startIndex == 0;
        }else{
            startIndex += team.eachTruckSize[i-1];
        }

        totalDistance -= distanceToDepot[team.sequenceNodes[startIndex][0]-2];

        for(int j = startIndex; j < startIndex+team.eachTruckSize[i]-1; j++){
            totalCargo += team.sequenceNodes[j][1];

            if(totalCargo > 100){

                totalDistance -= 10000.0;

            }else{

                totalDistance -= distanceBetweenNodes[team.sequenceNodes[j][0]-2][team.sequenceNodes[j+1][0]-2];
            } 
        }

        totalCargo += team.sequenceNodes[startIndex+team.eachTruckSize[i]-1][1];

        if(totalCargo > 100){

            totalDistance -= 10000.0;

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
    generatedTeam.eachTruckSize[0] += numNodes-lastTruckSize;

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