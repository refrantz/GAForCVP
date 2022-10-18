import math
import random

file_object = open("nodes_clean.txt", 'r')
#This is all our nodes.
nodes = []
for x in file_object:
    temp = x.split()
    nodes.append([int(temp[0]), int(temp[1]), int(temp[2]), int(temp[3])])


#This is so we only have to calculate how close every node is to each other once, sqrt takes some time after all.
closeness_graph = []
for x in nodes:
    temp = []
    i = 0
    for y in nodes:
        temp += [math.sqrt((x[2] - y[2])**2 + (x[3] - y[3])**2)]
        i += 1
    closeness_graph += [temp]


# Custom class to implement a heuristic function, this heuristic function will decide which car is the next one to choose a node and what node is chosen
# depending on some parameters tuned by genetic algorithm.
class Car:
    def __init__(self, paramA, paramB, paramC) -> None:
        self.route = [0]
        self.length = 0
        self.passengers = 0
        self.distance = 0
        self.current_location = [0,0]

        #Genetic Parameters:
        self.paramA = paramA
        self.paramB = paramB
        self.paramC = paramC

        # This decides how high in priority this car is to choose next node (higher number is worse).
        self.priority = self.update_prio()

    def new_node(self, node):
        self.route.append(node[0])
        self.length += 1
        self.passengers += node[1]
        self.distance += closeness_graph[self.route[-1]][node[0]]
    def update_prio(self):
        # We decide prio of a car by some combination of how close the next node is and the distance the car has traveled.
        return self.paramA * sorted(closeness_graph[self.route[-1]])[1]   +    self.paramB * self.length   +   self.paramC * self.passengers
    def next_node(self):
        #should probably be randomized or weighted to be able to choose something else than only the closest.
        return closeness_graph[self.route[-1]].index(sorted(closeness_graph[self.route[-1]])[1])


def main():
    mycars = []
    for x in range(9):
        mycars.append(Car(1,1,1))
        print(mycars[x].priority)

main()