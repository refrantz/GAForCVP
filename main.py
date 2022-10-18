from math import pow
from math import sqrt
import random
import numpy as np
from tqdm import tqdm

nodesFile = open("nodes_clean.txt")

nodesList = nodesFile.readlines()

numTrucks = 9
numTrucks += 1

nodes = []

nodesVisitedRuntime = []

genSize = 16

epochs = 10000

mutationRate = 5
crossOverRate = 50
tournamentSize = 4

depot = [int(x) for x in nodesList[0].split()]
del nodesList[0]

for nodeString in nodesList:
    nodeInt = [int(x) for x in nodeString.split()]
    nodes.append(nodeInt)

nodesFile.close()

def calculateTotalDistance(truck):

    totalDistanceTraveled = 0

    previousNode = depot

    if(checkValidity(truck)):
        for node in truck.nodesVisited:
            totalDistanceTraveled += sqrt(pow(node[2]-previousNode[2],2)+pow(node[3]-previousNode[3],2))
    else:
        return 100000
    
    return totalDistanceTraveled

def calculateSystemFitness(trucks):
    systemTotalDistance = 0
    systemNodes = []
    systemTotalNodesVisited = 0

    for truck in trucks:
        systemTotalDistance += calculateTotalDistance(truck)
        for node in truck.nodesVisited:
            if node not in systemNodes:
                systemTotalNodesVisited += 1000
                systemNodes.append(node)

    fitness = systemTotalNodesVisited-systemTotalDistance

    #if not all(node in allNodes(trucks) for node in nodes):
    #    fitness -= 10000000

    return fitness

def allNodes(trucks):
    result = []
    for truck in trucks:
        for node in truck.nodesVisited:
            result.append(node)
    return result

def checkValidity(truck):
    sum = 0

    for node in truck.nodesVisited:
        sum += node[1]

        if sum > 100:
            return False

    return True
        
def crossOver(trucks1, trucks2):
    child = []

    for i in range(0,numTrucks-1):
        crossoverPoint = int(min(len(trucks1[i].nodesVisited), len(trucks2[i].nodesVisited))/2)
        childNodes = trucks1[i].nodesVisited[:crossoverPoint]+trucks2[i].nodesVisited[crossoverPoint:]

        childTruck = Truck()
        for node in childNodes:
            childTruck.visitNode(node)
        child.append(childTruck)

    return child

def crossOverGeneration(gen):
    child = []
    for i in range(0, len(gen)-1):
        if i%2 == 0:
            if random.randint(0,100) < crossOverRate:         
                child.append(crossOver(gen[i], gen[i+1]))
                child.append(crossOver(gen[i+1], gen[i]))
            else:
                child.append(gen[i])
                child.append(gen[i+1])
    return child

def mutation(trucks):
    for truck in trucks:
        for i in range(2, len(truck.nodesVisited)-2):
            if random.randint(0, 100) <  mutationRate:
                truck.nodesVisited = np.array(truck.nodesVisited)
                np.random.shuffle(truck.nodesVisited[i-2:i+2])
                truck.nodesVisited = truck.nodesVisited.tolist()
            if random.randint(0, 100) <  mutationRate:
                truck.nodesVisited[i] = nodes[random.randint(0,len(nodes)-1)]

def mutateGeneration(gen):
    for trucks in gen:
        mutation(trucks)

class Truck():

    remainingCapacity = 100
    nodesVisited = []

    def __init__(self):
        self.remainingCapacity = 100
        self.nodesVisited = []

    def visitNode(self, node):

        if len(self.nodesVisited) > 0 and self.nodesVisited[len(self.nodesVisited)-1] == node:
            pass
        else:
            allNodes(trucks).append(node)
            self.nodesVisited.append(node)

            self.remainingCapacity -= int(node[1])

    def __str__(self):
        retorno = ""
        for node in self.nodesVisited:
            retorno += "|" + str(node[0])
        
        return retorno

oldGen = []

for i in range(0, genSize):
    trucks = [Truck() for x in range(numTrucks)]

    for node in nodes:  
        truckId = random.randint(0, numTrucks-1)
        trucks[truckId].visitNode(node)

    oldGen.append(trucks)

sum = 0
for trucks in oldGen:
    sum += calculateSystemFitness(trucks)

print('first gen fitness: ' + str(sum/len(oldGen)))

for epoch in tqdm(range(epochs)):
    newGen = []

    #elitism: guarantee the best of generation
    max = oldGen[0]
    for trucks in oldGen:
        if calculateSystemFitness(max) < calculateSystemFitness(trucks):
            max = trucks
    newGen.append(max)

    #contest select fitest
    for i in range(0, genSize-1):
        ids = [random.randint(0, genSize-2) for x in range(tournamentSize)]

        max = oldGen[0]
        for id in ids:
            candidate = oldGen[id]
            if calculateSystemFitness(candidate) > calculateSystemFitness(max):
                max = oldGen[id]
        
        newGen.append(max)

    
    #crossover and mutation
    newGen = crossOverGeneration(newGen)
    mutateGeneration(newGen)

    oldGen = newGen

    if epoch % 1000 == 0:
        sum = 0
        for trucks in newGen:
            sum += calculateSystemFitness(trucks)
        
        print(sum/len(newGen))

for trucks in newGen:
    print('---------')
    print(calculateSystemFitness(trucks))
    for truck in trucks:
        print(truck)
