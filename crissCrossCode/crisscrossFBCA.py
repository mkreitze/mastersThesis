#to quickly generate FBCAs that I can hardcode

#imports of libraries
from PIL import Image
import random
import copy
import math
import os 
import time #for debugging
import numpy

#global inits
CALength=300 #Length of the generated image (for min use 5)
CAWidth=300 #Width of the generated image (for min use 4)
useMinMap=0 #checks if youre using 'min' map
useImages=1 #checks if you want images
numOfGens=20 #number of generations
numOfStates=2 #number of states (good until 10)
random.seed(1)  #1 for held     
sMs=[[0.320205,0.952292,0.351335,0.837774],[0.390741,0.728013,0.614486,0.378596]]

# possible combinations for matrix 1 and matrix 2:
# 1 2  2 1  2 2  1 1  1 2  2 1
# 2 1  1 2  1 1  2 2  1 2  2 1


#each cell has a score and a state (calculated by standard FBCA methods)
class CACell:
    state=0
    score=0

#for colouring later
def colourConvert(x):
    return {
        0: (0,204,102),
        1: (204,102,0),
        2: (0,255,0),
        3: (0,0,255),
        4: (255,255,255),
        5: (51,255,255),
        6: (0,255,255),
        7: (255,69,0),
        8: (0,102,0),
        9: (153,0,153),
        10: (255,255,51),
    }[x]    

#Cellular Automata initalization
#Fills with empty cells that have a random state and no score. 
def initCA(CAMap):
    #fills empty list with random states (x then y: downward strips)
    for x in range(0,CALength):
        holder=[]
        for y in range(0,CAWidth):
            holder.append(CACell())
            holder[y].state=random.randint(0,numOfStates-1)
        CAMap.append(holder)
    if(useMinMap==1):
        CAMap[0][0].state=1;CAMap[1][0].state=1;CAMap[2][0].state=0;CAMap[3][0].state=0;CAMap[4][0].state=1
        CAMap[0][1].state=1;CAMap[1][1].state=0;CAMap[2][1].state=0;CAMap[3][1].state=0;CAMap[4][1].state=1
        CAMap[0][2].state=0;CAMap[1][2].state=1;CAMap[2][2].state=0;CAMap[3][2].state=1;CAMap[4][2].state=1
        CAMap[0][3].state=1;CAMap[1][3].state=0;CAMap[2][3].state=0;CAMap[3][3].state=0;CAMap[4][3].state=1
    return(CAMap)

def copyOver(CAMapInit):
    CAMap=[]
    for x in range(0,CALength):
        holder=[]
        for y in range(0,CAWidth):
            holder.append(CACell())
            holder[y].state=CAMapInit[x][y].state
        CAMap.append(holder)
    return(CAMap)


#Take cellular automata filled with states, and a score matrix, and assigns scores
#Done in 3 steps, assign cell scores, generates CA copy, updates each state to highest scoring neighbour
#Von Neumann Neighbourhood used 
def updateMap(CAMap,scoreMatrix):

    #To start, hardcode the neighbourhood. It is a list of tuples representing the x and y offset from the center square
    neighbours=[]
    neighbours.append((0,1))#top (1 up)
    neighbours.append((0,-1))#bot (1 down)
    neighbours.append((-1,0))#left (1 left)
    neighbours.append((1,0))#right (1 right)
    for x in range(0,CALength):
        for y in range(0,CAWidth):
            #Need to get score from center square and its neighbours.
            row=0; row = CAMap[x][y].state*numOfStates #the center colour determines the row of the score matrix used 
            col=0; #the col of the score matrix used will depend on the  neighbours state

            CAMap[x][y].score=0 #resets center square's score

            #sums scores
            for z in neighbours:
                col=CAMap[(x+z[0])%CALength][(y+z[1])%CAWidth].state #mod used to connect edges
                CAMap[x][y].score+=scoreMatrix[row+col] #since the score matrix is a list, row+col gives the correct entry
    #start by copying the map
    CAMapCopy=copy.copy(CAMap)
    #for every cell, find the highest score among neighbours
    for x in range(0,CALength):
        for y in range(0,CAWidth):
            #NOTE: We give priority to the center square on ties. Priority continues up with the last defined neighbour to have the worst priority
            bigScore=0;bigScore=CAMap[x][y].score
            #compares neighbours scores, reassigning bigScore and state if someone is bigger
            for z in neighbours:
                if(bigScore<CAMap[(x+z[0])%CALength][(y+z[1])%CAWidth].score):
                    bigScore=CAMap[(x+z[0])%CALength][(y+z[1])%CAWidth].score
                    CAMapCopy[x][y].state=CAMap[(x+z[0])%CALength][(y+z[1])%CAWidth].state
    return(CAMapCopy)


#generates an image at every time step
#pulled from school project
def genIm(CAMap,n,directory):
    im= Image.new('RGB', (CALength, CAWidth))
    for x in range(CALength):
        for y in range(CAWidth):
            im.putpixel((x,y),colourConvert(CAMap[x][y].state))
    im.save(directory+"Gen"+str(n)+".png")
    return(im)

#took folder creation function from https://gist.github.com/keithweaver/562d3caa8650eefe7f84fa074e9ca949
def createFolder(directory):
    try:
        if not os.path.exists(directory):
            os.makedirs(directory)
    except OSError:
        print ('Error: Creating directory. ' +  directory)

#generates a new score matrix given a string
#example 1,2,2,1 would generate the 
# 1 2
# 2 1
def generateScoreMatrix(sMs,string):
    newScoreMatrix=[]
    for x in string:
        newScoreMatrix+=(sMs[x-1])
    return(newScoreMatrix)

#MAIN 
CAMapInit=[]
CAMapInit=initCA(CAMapInit)
#generates original matrices 
for idx,sM in enumerate(sMs):
    CAMap=copyOver(CAMapInit)
    gif=[]
    if(useImages==1): #possibly makes images
        fileName=str(sM)+" "+str(idx)
        createFolder(fileName)
        #since the x and y value 
        d = os.getcwd()+"/"+str(fileName)+"/"
    #runs FBCA to desired number of gens
    for n in range(numOfGens):
        if (useImages==1):
            gif.append(genIm(CAMap,n,d))
        CAMap=updateMap(CAMap,sM)
    if (useImages==1):
        gif.append(genIm(CAMap,n,d))
    print("Finished "+str(sM))
    if (useImages==1):
        gif[0].save(d+'finalRun'+str(fileName)+'.gif',save_all=True,append_images=gif[1:],optimize=False,duration=100,loop=0)

crissCrossSMs=[]
#generates new scorematrices
# 1 2  2 1  2 2  1 1  1 2  2 1
# 2 1  1 2  1 1  2 2  1 2  2 1
#going to hard code these strings for now, should be an easy way to generate them in the future
strings=[[1,2,2,1],[2,1,1,2],[2,2,1,1],[1,1,2,2],[1,2,1,2],[2,1,2,1]]
for string in strings:
    crissCrossSMs.append(generateScoreMatrix(sMs,string))

numOfStates*=2 #number of states (good until 10)
CAMapInit=[]
CAMapInit=initCA(CAMapInit)
for idx,sM in enumerate(crissCrossSMs):
    CAMap=copyOver(CAMapInit)
    gif=[]
    if(useImages==1): #possibly makes images
        fileName=str(strings[idx])+" "+str(idx)
        createFolder(fileName)
        #since the x and y value 
        d = os.getcwd()+"/"+str(fileName)+"/"
    #runs FBCA to desired number of gens
    for n in range(numOfGens):
        if (useImages==1):
            gif.append(genIm(CAMap,n,d))
        CAMap=updateMap(CAMap,sM)
    if (useImages==1):
        gif.append(genIm(CAMap,n,d))
    print("Finished "+str(sM))
    if (useImages==1):
        gif[0].save(d+'finalRun'+str(fileName)+'.gif',save_all=True,append_images=gif[1:],optimize=False,duration=100,loop=0)


