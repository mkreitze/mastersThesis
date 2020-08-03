#Matthew Kreitzer 2020 Masters code (Rajesh/Dan supervisors)
#This code is made to generate the phase plot of a two colour FBCA system. 

#imports of libraries
from PIL import Image
import random
import copy
import math
import os 
import time #for debugging
import numpy
import matplotlib.pyplot as plt
from matplotlib import cm
#x and y are chosen between start and end. 
#R is also arbitrary. 
#global inits
R=100 #note this is R_o^2... because it tends to be easier. 100 is standard
startXVal= -10 # value we want to vary x from-10 standard
endXVal= 10 #value we want to vary x to 10 standard
#startYVal= -10 # only used if a custom range is specified
#endYVal= 10 # only used if a custom range is specified
sizeOfStepX = 0.2 #size of step for x; standard 0.5
sizeOfStepY = 0.2 #size of step for y; standard 0.5 want ~8000 for good image
accuracy=10 # num of decimals
CALength=20 #Length of the generated image (for min use 5)
CAWidth=20 #Width of the generated image (for min use 4)
useMinMap=0 #checks if youre using 'min' map
useImages=0 #checks if you want images
numOfGens=20 #number of generations
numOfStates=2 #number of states (good until 10)... I cant use more than 2
amountOffsetX=0.5
amountOffsetY=0.5
QUANTIFER="8000PhasePortDeci"
random.seed(1) #1 is considered fixed

#each cell has a score and a state (calculated by standard FBCA methods)
class CACell:
    state=0
    score=0
   
#FBCA are described by their score matrix (decided by x and y but its useful to be able to plot later) and its resulting level map 
#That being said, the level map and score matrix are technically decided y the global inits above
class Fbca:
    x=0
    y=0
    levelMap=[]
    scoreMatrix=[]

#for colouring later
def colourConvert(x):
    return {
        0: (0,0,0),
        1: (255,0,0),
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

### Functions ###

#took folder creation function from https://gist.github.com/keithweaver/562d3caa8650eefe7f84fa074e9ca949
def createFolder(directory):
    try:
        if not os.path.exists(directory):
            os.makedirs(directory)
    except OSError:
        print ('Error: Creating directory. ' +  directory)


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

#compares to FBCAs of the same size after a certain number of runs 
#while it works if it isnt, its assumed that theyre the same size
def compareFBCAs(fbcaA,fbcaB,length1,width1,length2=10000,width2=10000):
    isSame=True
    #checks for the smaller of the lengths and widths (uses ternary operator)
    trueLength = length1 if length1<=length2 else length2
    trueWidth = width1 if width1<=width2 else width2
    for x in range(trueLength):
        for y in range(trueWidth):
            if fbcaA[x][y].state != fbcaB[x][y].state :
                isSame=False
    return(isSame)


def copyOver(CAMapInit):
    CAMap=[]
    for x in range(0,CALength):
        holder=[]
        for y in range(0,CAWidth):
            holder.append(CACell())
            holder[y].state=CAMapInit[x][y].state
        CAMap.append(holder)
    return(CAMap)
#Main Function#
#data structure inits
endFBCAs=[] #will hold all of the completed FBCAs as a lists after their updates
behaviours=[] #each entry should be a listof fbcas that have the same behaviour
ignoreList=[] #used to avoid double counting for the behaviours


#generate all score matricies    
print("Generating score matricies")
CAMapInit=[]
CAMapInit=initCA(CAMapInit)
for x in numpy.arange(startXVal,endXVal,sizeOfStepX):
    dontForget=0 
    if(R-pow(x,2)<0): #finds possible y values (makes sure not to break math first)
        continue
    endYVal=math.sqrt(R-pow(x,2))
    startYVal=(-1)*endYVal
    #incase were using a predetermined y val set mute above
    for y in numpy.arange(startYVal,endYVal,sizeOfStepY): #binds as -sqrt(R_o^2-x^2) to sqrt(R_o^2-x^2)  
        scoreMatrix=[]
        fileName=[]
        CAMap=copyOver(CAMapInit)
        gif=[]
        if((R-pow(x,2)-pow(y,2))<0): #because cancellation problems
            continue
        z=math.sqrt(R-pow(x,2)-pow(y,2))
        #generates score matrix
        scoreMatrix.append(x+y+z) #NOTE: This sets the standard for 0 = top left, 1 = top right, 2 = bottom left, 3 = bottom right
        scoreMatrix.append(y-x-z) #changed varb name to jive with updateFBCA
        scoreMatrix.append(z-x-y)
        scoreMatrix.append(x-y-z) #at this point the score matrix is made, now we make the CA

        if(useImages==1): #possibly makes images
            fileName=str(x)+"xVal"+str(y)+"yVal"
            createFolder(fileName)
            #since the x and y value 
            d = os.getcwd()+"/"+str(fileName)+"/"

        #runs FBCA to desired number of gens
        for n in range(numOfGens):
            if (useImages==1):
                gif.append(genIm(CAMap,n,d))
            CAMap=updateMap(CAMap,scoreMatrix)
        if (useImages==1):
            gif.append(genIm(CAMap,n,d))
        
        #collect finished FBCAs for later
        endFBCAs.append(Fbca()) 
        endFBCAs[len(endFBCAs)-1].x=x
        endFBCAs[len(endFBCAs)-1].y=y
        endFBCAs[len(endFBCAs)-1].levelMap= copyOver(CAMap) #this copy command copies the complete data stucture (true from tests)
        endFBCAs[len(endFBCAs)-1].scoreMatrix=[] #this copy command copies the complete data stucture (true from tests)        
        endFBCAs[len(endFBCAs)-1].scoreMatrix.append(x+y+z) #NOTE: This sets the standard for 0 = top left, 1 = top right, 2 = bottom left, 3 = bottom right
        endFBCAs[len(endFBCAs)-1].scoreMatrix.append(y-x-z) #changed varb name to jive with updateFBCA
        endFBCAs[len(endFBCAs)-1].scoreMatrix.append(z-x-y)
        endFBCAs[len(endFBCAs)-1].scoreMatrix.append(x-y-z) #at this point the score matrix is made, now we make the CA
        if (useImages==1):
            gif[0].save(d+'finalRun'+str(fileName)+'.gif',save_all=True,append_images=gif[1:],optimize=False,duration=100,loop=0)
        print("Finished points x>"+str(x)+" y>"+str(y)+" with "+str(scoreMatrix))

#sort be similar behaviours
for idx1,fbcaCur in enumerate(endFBCAs):
    behaviourSet=[] 
    ignore=False

    for val in ignoreList: #checks if we should ignore it by checking the ignoreList
        if idx1 == val:
            ignore=True
    if ignore==False:
        for idx2,fbcaNew in enumerate(endFBCAs):
            #compare the state of every cell to every other FBCA. Generates a list of similar FBCAs
            isSame=True
            for x in range(CALength):
                for y in range(CAWidth):
                    if fbcaNew.levelMap[x][y].state != fbcaCur.levelMap[x][y].state:
                        isSame=False
            if isSame == True: #if they are the same
                behaviourSet.append(fbcaNew) #add in a new Fbca
                ignoreList.append(idx2)
        behaviourSet.append(fbcaCur) #add in progenitor 
        ignoreList.append(idx1)                
        behaviours.append(behaviourSet)
    print("Finished comparison on "+str(idx1)+" out of " +str(len(endFBCAs)-1)+str(" total"))

#Generate plot
fig = plt.figure()
ax1=plt.axes()
ax1.set_xlim(startXVal-amountOffsetX,endXVal+amountOffsetX)
#change to y if custom
ax1.set_ylim(startXVal-amountOffsetX,endXVal+amountOffsetX)
#method to always colour my graph well.
increment=0
behaviourNum=0
behaviourTotal=0
for behaviour in behaviours: 
    behaviourTotal+=1
increment=float(20000)/float(behaviourTotal)
for behaviour in behaviours:
    xs=[]
    ys=[]
    for fbca in behaviour: #get xs and ys of one behaviour set   
        xs.append(fbca.x)
        ys.append(fbca.y)
    behaviourNum+=1
    #give the behaviour set a unique colour from the colour map and plot the xy points in this colour
    magma = cm.get_cmap('gist_ncar', 20000) #mod work is (100*(behaviourNum%100)+behaviourNum)
    ax1.scatter(xs,ys,c=magma(int(increment*behaviourNum)),label=str(behaviourNum))
#save the image
plt.savefig(str(QUANTIFER)+".png")

#for sanity im going to record the behaviours down on a paper (obviously switch QUANTIFER)
behaviourNum=0
bRec= open(str(QUANTIFER)+ "bR "+".txt",'a')
bRec.write("Run "+str(QUANTIFER)+"\n")
for behaviourSet in behaviours:
    behaviourNum+=1
    bRec.write("New behaviour set:"+str(behaviourNum)+" \n")
    for fbca in behaviourSet:
        bRec.write("Points x>"+str(fbca.x)+" y>"+str(fbca.y)+" with "+str(fbca.scoreMatrix)+"\n")
