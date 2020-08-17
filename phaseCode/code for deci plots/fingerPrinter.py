import numpy
import math
import matplotlib.pyplot as plt
from matplotlib import cm
#generate score matricies
#Stolen from normal phase plotter
class Fbca:
    x=0
    y=0
    scoredFingerprint=[]
    scoreMatrix=[]
R=10000 #this is R_o^2, standard is 100


#x vals satisfy -R_o<=x<=R_o
#custom values allowed to zoom in 
startXVal=-100
endXVal=100
sizeOfStepX=1
amountOffsetX=1
#custom values allowed to zoom in (muted by default)
#startYVal=-10
#endYVal=10
sizeOfStepY=1
amountOffSetY=1
numOfStates=2
QUANTIFER="fingerprinter"
neighbourhoods=[]
fingerPrints=[]

#generate all score matricies    
print("Generating score matricies")
for x in numpy.arange(startXVal,endXVal,sizeOfStepX):
    #y vals are coded to follow -sqrt(R_o^2-x^2)<y<sqrt(R_o^2-x^2)    
    if(R-pow(x,2)<0): #only reals allowed
        continue
    endYVal=math.sqrt(R-pow(x,2))
    startYVal=(-1)*endYVal
    #incase were using a predetermined y val set mute above
    for y in numpy.arange(startYVal,endYVal,sizeOfStepY): #this is -sqrt(R_o^2-x^2)<=y<=(R_o^2-x^2)  
        if((R-pow(x,2)-pow(y,2))<0): #because cancellation problems
            continue
        z=math.sqrt(R-pow(x,2)-pow(y,2))
        #generates and adds it score matrix . Im hard coding it for sanity 
        fingerPrints.append(Fbca()) 
        fingerPrints[len(fingerPrints)-1].x=x
        fingerPrints[len(fingerPrints)-1].y=y
        fingerPrints[len(fingerPrints)-1].scoreMatrix=[[0,0],[0,0]]
        fingerPrints[len(fingerPrints)-1].scoreMatrix[0][0]=(x+y+z) #NOTE: This sets the standard for 0 = top left, 1 = top right, 2 = bottom left, 3 = bottom right
        fingerPrints[len(fingerPrints)-1].scoreMatrix[0][1]=(y-x-z) #changed varb name to jive with updateFBCA
        fingerPrints[len(fingerPrints)-1].scoreMatrix[1][0]=(z-x-y)
        fingerPrints[len(fingerPrints)-1].scoreMatrix[1][1]=(x-y-z) #at this point the score matrix is made, now we make the CA
        print("Finished points score matrix for x>"+str(x)+" y>"+str(y)+" with "+str(fingerPrints[len(fingerPrints)-1].scoreMatrix))

#generates the neighbourhood
#its going to be hard coded for sanity. Develop an automated system in the future
#If we account for varying through the possible center states we only care about the number of every state in the neighbourhood.
#by default we record number of state 1, number of state 2 etc...
neighbourhoods.append([4,0])
neighbourhoods.append([3,1])
neighbourhoods.append([2,2])
neighbourhoods.append([1,3])
neighbourhoods.append([0,4])

print("Generating fingerPrints")
#generates scoredFingerprint
for idx,fingy in enumerate(fingerPrints):
    tempDic={}
    for centerState in range(numOfStates):
        for neighbourhood in neighbourhoods:
            tempScore=0
            for neighbourState,stateNum in enumerate(neighbourhood):
                tempScore+=stateNum*fingy.scoreMatrix[centerState][neighbourState]
            tempDic[str(centerState)+":"+str(neighbourhood)]=tempScore
    fingy.scoredFingerprint=sorted(tempDic.items(), key = lambda kv:(kv[1], kv[0]))
    print("Finished generating "+str(idx)+" of "+str(len(fingerPrints))+" fingerprints")



print("Comparing fingerPrints")
#compares
ignoreList=[]
behaviourSets=[]
for fing1Idx,fingy1 in enumerate(fingerPrints):
    ignoreMe=False
    for ignore in ignoreList:
        if (fing1Idx==ignore):
            ignoreMe=True
    if ignoreMe != True:
        behaviourSet=[]
        behaviourSet.append(fingy1)
        ignoreList.append(fing1Idx)
        #explain this later ew.
        for fing2Idx,fingy2 in enumerate(fingerPrints):
            isSame=True
            for numInPrint in range(len(fingy.scoredFingerprint)):
                if(fingy1.scoredFingerprint[numInPrint][0]!=fingy2.scoredFingerprint[numInPrint][0]):
                    isSame=False
            if isSame==True:
                behaviourSet.append(fingy2)
                ignoreList.append(fing2Idx)
        #add the noticed behaviourSet
        behaviourSets.append(behaviourSet)
    print("Finished comparing "+str(fing1Idx)+" of "+str(len(fingerPrints)))

#using the plotter from the basic method

#Generate plot
fig = plt.figure()
ax1=plt.axes()
ax1.set_xlim(startXVal-amountOffsetX,endXVal+amountOffsetX)
#change to y if custom
ax1.set_ylim(startXVal-amountOffsetX,endXVal+amountOffsetX)
increment=0
behaviourNum=0
behaviourTotal=0
for behaviourSet in behaviourSets: 
    behaviourTotal+=1
increment=float(20000)/float(behaviourTotal)
for behaviourSet in behaviourSets: 
    xs=[]
    ys=[]
    for fbca in behaviourSet: #get xs and ys of one behaviour set   
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
bRec= open(str(QUANTIFER)+" fingerprint bR "+".txt",'a')
bRec.write("Run "+str(QUANTIFER)+" \n")
for behaviourSet in behaviourSets:
    behaviourNum+=1
    bRec.write("New behaviour set:"+str(behaviourNum)+" \n")
    for fbca in behaviourSet:
        bRec.write("Points x>"+str(fbca.x)+" y>"+str(fbca.y)+" with "+str(fbca.scoreMatrix)+"\n")
