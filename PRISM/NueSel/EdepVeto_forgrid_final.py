#! /usr/bin/env python
"""
Eelectron_veto code to feed into the neutral network to get probabilites to apply to exsiting CAFs. 

Runs over multiple ROOT files created by edepsim (GEANT wrapped), extracts list of

for every single-neutrino interaction:
for every true electron event (event with electron as primary path as defined by geant):
    What is the location of the energy deposition? (EVisTrue, MeV)
    What is the amount of the energy deposition? 

    What is the event vertex location? 
    What is the inital momentum of the primary electron? 
        Add up all the energy deposits in the collar (30 cm from edge of active LAr volume)
            If this amount is > 30 MeV, reject the electron event 
                 does not pass Eelectron_veto, do not select event  
        If < 30 MeV, keep it
                passes Eelectron_veto, event passes selection!

"""

from math import sqrt

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import glob
import os
import sys
#import fire
#import h5py
#from tqdm import tqdm
from ROOT import TG4Event, TFile


#path = '/pnfs/dune/persistent/physicsgroups/dunendsim/production/Mini_Production_Campaign/Active_Volume/FHC/Hall_ND-LAr_TMS/edep'
##file_location = s.path.join('/pnfs', 'dune', 'persistent', 'physicsgroups', 'dunendsim', 'Mini_Production_Campaign', 'Active_Volume', 'FHC', 'Hall_ND-LAr_TMS', 'edep', '*.root')
##print(file_location)
#file_location = os.path.join(path, '*.root') #all is *.root, few files '*89.edep.root'
#
#file_names = glob.glob(file_location)
#print(file_names)
#
#ALLFILES_XforPrimaryElectron_list=list()
#ALLFILES_YforPrimaryElectron_list=list()
#ALLFILES_ZforPrimaryElectron_list=list()
#ALLFILES_PxforPrimaryElectron_list=list()
#ALLFILES_PyforPrimaryElectron_list=list()
#ALLFILES_PzforPrimaryElectron_list=list()
#ALLFILES_edep_x_list=list()
#ALLFILES_edep_y_list=list()
#ALLFILES_edep_z_list=list()
#ALLFILES_Eelectron_veto_list=list()

#for f in file_names:

a = str(sys.argv[1])
#print(a)
#print(sys.argv[1])

# this works interactively
#edep_file = TFile("/pnfs/dune/tape_backed/dunepro/neardet/simulated/2022/mc/physics/dune_nd_production_2022_v1/00/00/00/00/neutrino.0_1659367852.edep.root")
edep_file = TFile(a, "OPEN")
if not edep_file:
  print ("Error: could not open file", a)
  sys.exit()
inputTree = edep_file.Get("EDepSimEvents")
event = TG4Event()
inputTree.SetBranchAddress("Event",event)
entries = inputTree.GetEntriesFast()
#print(entries)

segments_dtype = np.dtype([("dE", "f4")]) #f4 = float 4, has decimal place 
trajectories_dtype = np.dtype([("pdgID", "i4"), ("trackID", "u4")]) #i4 - integer 4, u4 - unsigned integer(sort of abs value) 
segments_list = list()
trajectories_list = list()
segments_trackIDs_list = list()

    #from larndsim -- Convert from EDepSim default units (mm, ns)
edep2cm = 0.1   # convert to cm
edep2us = 0.001 # convert to microseconds


offset = [ 0., -75., 411. ]
collarLo = [ -320., -120, 30. ]  #this is the edge of the collar region (30 cm from the edge of the active LAr region) in cm in X, Y, and Z
collarHi = [ 320., 120, 470. ]
    # above are the collar dimensions if active volume x = -350 to 350 cm, y = -150 to 150 cm, z = 0 to 500 cm
activeDetectorLo = [ -350., -150., 0.]
activeDetectorHi = [350., 150., 500.]

#for jentry in range(46,47): #entries, test #46,47 loops only over jentry = 46, one that I've already confirmed is a primaryelectron
XforPrimaryElectron_list=list()
YforPrimaryElectron_list=list()
ZforPrimaryElectron_list=list()
PxforPrimaryElectron_list=list()
PyforPrimaryElectron_list=list()
PzforPrimaryElectron_list=list()
edep_x_list=list()
edep_y_list=list()
edep_z_list=list()
Eelectron_veto_list=list()

for jentry in range(entries): #entries, test #46,47 loops only over jentry = 46, one that I've already confirmed is a primaryelectron
#    print("jentry here = " +str(jentry))
    nb = inputTree.GetEntry(jentry) #number of bytes read
#    print("nb =" + str(nb))
#    print("event number", event.EventId)
#    print("number of trajectories", event.Trajectories.size())
#    print("number of segments", event.SegmentDetectors.size())
#    trackIDsforSegementsgeneratedbyPrimaryElectron = list()
             
    #print(ipart)
    trackIDforPrimaryElectron=-9191
    for primary in event.Primaries:
        for ipart, particle in enumerate(primary.Particles):
            PDGCode = particle.GetPDGCode()
            #print(PDGCode)
            if PDGCode==11:
                    #print("Event with Electron Primary found",jentry)
                XforPrimaryElectron_anywhere = primary.GetPosition().X() * edep2cm - offset[0]
                YforPrimaryElectron_anywhere = primary.GetPosition().Y() * edep2cm - offset[1]
                ZforPrimaryElectron_anywhere = primary.GetPosition().Z() * edep2cm - offset[2]
                PxforPrimaryElectron_anywhere = particle.GetMomentum().X() 
                PyforPrimaryElectron_anywhere = particle.GetMomentum().Y()
                PzforPrimaryElectron_anywhere = particle.GetMomentum().Z() 
                if (activeDetectorLo[0] < XforPrimaryElectron_anywhere  and activeDetectorHi[0] > XforPrimaryElectron_anywhere) and (activeDetectorLo[1] < YforPrimaryElectron_anywhere and activeDetectorHi[1] > YforPrimaryElectron_anywhere) and (activeDetectorLo[2] < ZforPrimaryElectron_anywhere and activeDetectorHi[2] > ZforPrimaryElectron_anywhere):
                    trackIDforPrimaryElectron = particle.GetTrackId()
                    XforPrimaryElectron = XforPrimaryElectron_anywhere # vertex inside the detector                        YforPrimaryElectron = YforPrimaryElectron_anywhere
                    YforPrimaryElectron = YforPrimaryElectron_anywhere
                    ZforPrimaryElectron = ZforPrimaryElectron_anywhere
                    PxforPrimaryElectron = PzforPrimaryElectron_anywhere
                    PyforPrimaryElectron = PyforPrimaryElectron_anywhere
                    PzforPrimaryElectron = PzforPrimaryElectron_anywhere
                    trackIDforPrimaryElectron = particle.GetTrackId()
    if trackIDforPrimaryElectron == -9191:
        continue

#    print("jentry ", jentry)
#
    edep_incollar_total = 0.0
    for containerName, hitSegments in event.SegmentDetectors:        
        for iHit, hitSegment in enumerate(hitSegments):
            primaryID = hitSegment.GetPrimaryId()
            if primaryID == trackIDforPrimaryElectron:
                edep_x = hitSegment.GetStop().X() * edep2cm - offset[0]
                edep_y = hitSegment.GetStop().Y() * edep2cm - offset[1]
                edep_z = hitSegment.GetStop().Z() * edep2cm - offset[2]
                edep_x_list.append(edep_x)
                edep_y_list.append(edep_y)
                edep_z_list.append(edep_z)
                if edep_x > collarHi[0] or edep_x < collarLo[0] or edep_y > collarHi[1] or edep_y < collarLo[1] or edep_z > collarHi[2] or edep_z < collarLo[2]:
                    edep = hitSegment.GetEnergyDeposit()
                    edep_incollar_total = edep_incollar_total + edep
    
    Eelectron_veto=1 #1 if passes veto, i.e., keep the event!
    if edep_incollar_total > 30.0:
        Eelectron_veto=0 # 0 is does not pass veot, i.e., throw out the event! 
#        print("Eelectron_veto ", Eelectron_veto, "edep_incollar_total ", edep_incollar_total)


        XforPrimaryElectron_list.append(XforPrimaryElectron)
        YforPrimaryElectron_list.append(YforPrimaryElectron)
        ZforPrimaryElectron_list.append(ZforPrimaryElectron)
        PxforPrimaryElectron_list.append(PxforPrimaryElectron)
        PyforPrimaryElectron_list.append(PyforPrimaryElectron)
        PzforPrimaryElectron_list.append(PzforPrimaryElectron)
        edep_x_list.append(edep_x)    
        edep_y_list.append(edep_y)    
        edep_z_list.append(edep_z)    
        Eelectron_veto_list.append(Eelectron_veto)

XforPrimaryElectron_array=np.array(XforPrimaryElectron_list)    
YforPrimaryElectron_array=np.array(YforPrimaryElectron_list)    
ZforPrimaryElectron_array=np.array(ZforPrimaryElectron_list)
PxforPrimaryElectron_array=np.array(PxforPrimaryElectron_list)    
PyforPrimaryElectron_array=np.array(PyforPrimaryElectron_list)    
PzforPrimaryElectron_array=np.array(PzforPrimaryElectron_list)    
Eelectron_veto_array=np.array(Eelectron_veto_list)

#np.save("XforPrimaryElectron_list", XforPrimaryElectron_list)
#np.save("YforPrimaryElectron_list", YforPrimaryElectron_list)
#np.save("ZforPrimaryElectron_list", ZforPrimaryElectron_list)
#np.save("PxforPrimaryElectron_list", PxforPrimaryElectron_list)
#np.save("PyforPrimaryElectron_list", PyforPrimaryElectron_list)
#np.save("PzforPrimaryElectron_list", PzforPrimaryElectron_list)
#np.save("Eelectron_veto_list", Eelectron_veto_list)

alldata = np.column_stack([XforPrimaryElectron_array, YforPrimaryElectron_array, ZforPrimaryElectron_array, PxforPrimaryElectron_array, PyforPrimaryElectron_array, PzforPrimaryElectron_array, Eelectron_veto_array])
#print(XYZ)
dfalldata = pd.DataFrame(alldata,columns=['X','Y','Z','Px','Py','Pz','veto'])
#print(dfalldata)

#step 3: write to hdf

dfalldata.to_hdf('XYZPxPyPzveto.h5', key='dfalldata', mode='w')





#    ## Creating plot
#fig = plt.figure(figsize = (10, 7))
#ax = plt.axes(projection ="3d")
#    
#p=ax.scatter3D(ALLFILES_XforPrimaryElectron_list, ALLFILES_YforPrimaryElectron_list, ALLFILES_ZforPrimaryElectron_list, c = ALLFILES_Eelectron_veto_list,cmap=plt.cm.winter)
#
#fig.colorbar(p,ax=ax)
#
##ax.quiver(XforPrimaryElectron_list,YforPrimaryElectron_list,ZforPrimaryElectron_list,PxforPrimaryElectron_list,PyforPrimaryElectron_list,PzforPrimaryElectron_list,length=100,normalize=True)
#
#plt.title("Veto")
##plt.show()
#plt.savefig('veto_plt_10files.png')
#
#
##print('X', ALLFILES_XforPrimaryElectron_list)
##print('Y', ALLFILES_YforPrimaryElectron_list)
##print('Z', ALLFILES_ZforPrimaryElectron_list)
#
##print('veto list', ALLFILES_Eelectron_veto_list)


##    trajectories = np.empty(len(event.Trajectories), dtype=trajectories_dtype)
##    for iTraj, trajectory in enumerate(event.Trajectories):
##        start_pt, end_pt = trajectory.Points[0], trajectory.Points[-1]
##        trajectories[iTraj]["pdgID"] = trajectory.GetPDGCode()
##        trajectories[iTraj]["trackID"] = trajectory.GetTrackId()
##        if trajectories[iTraj]["pdgID"] == 11: #electron
##            trackIDsforelectrons_list.append(trajectory.GetTrackId())
#    
## make an empty (not NULL) list of lenght trackIDsforelectrons_list, 
#    electronsGetStopX_list=[None]*len(trackIDsforPRIMARYelectrons_list)
#    electronsGetStopY_list=[None]*len(trackIDsforPRIMARYelectrons_list)
#    electronsGetStopZ_list=[None]*len(trackIDsforPRIMARYelectrons_list)
#
#    #separate loop for SeementDetectors under jentry, same level as trajectories
#    for containerName, hitSegments in event.SegmentDetectors:
#        print("hitSegments size", hitSegments.size())
#        # iHit is the index, hitSEgment is the data stored at the index in the second item in event.SegementDectectors
#        for iHit, hitSegment in enumerate(hitSegments):
#            trackID_of_segment = trajectories[hitSegment.Contrib[0]]["trackID"]
#            #hitSegment.Contrib[] is iTraj from previous loop
#            if trackID_of_segment in trackIDsforPRIMARYelectrons_list:
#                myid = trackIDsforelectrons_list.index(trackID_of_segment)
#                #assign hitSegment.GetStart().X() to the myid index of electronsGetStarX_list
#                electronsGetStopX_list[myid]=hitSegment.GetStop().X() * edep2cm - offset[0]
#                electronsGetStopY_list[myid]=hitSegment.GetStop().Y() * edep2cm - offset[1]
#                electronsGetStopZ_list[myid]=hitSegment.GetStop().Z() * edep2cm - offset[2]
#                print(electronsGetStopX_list[myid])
#
##electronsGetStartX -- a list of starting points of the energy depisition for electrons
## and trackIDsforlectrons_list --a list of trackIDS for electrons
##and both of these lists have matching indexs so you can find for one electron event both the trackID and the GEtSTartX
#
##get veto region based on basic coordinates, can update later
##collar region from https://internal.dunescience.org/doxygen/ND__CAFMaker_2dumpTree_8py_source.html
#
##is the GetStopX within (lo) -320 to (hi) 320 and is GetStopY within (lo) -120 to (hi) 120 and is GEtStopZ wihin (lo) 30 to (hi) 470?
##if yes passes veto if no faile
##is the GEt
#
#    #again another on the jentry level
#    passes_veto_index = list()
#    for iedeploc in range(len(electronsGetStopX_list)):
#        if electronsGetStopX_list[iedeploc] < collarHi[0] and electronsGetStopX_list[iedeploc] > collarLo[0] and electronsGetStopY_list[iedeploc] < collarHi[1] and electronsGetStopY_list[iedeploc] > collarLo[1] and electronsGetStopZ_list[iedeploc] < collarHi[2] and electronsGetStopZ_list[iedeploc] > collarLo[2]:
#            passes_veto_index.append(iedeploc) # list of the indexs from electronsGetStopetc that pass the veto. 
#    print("passes_veto_index =", passes_veto_index)
#
#    trackIDs_thatpassveto_list = [trackIDsforPRIMARYelectrons_list[index] for index in passes_veto_index]
#    print("trackIDs_thatpassveto_list =", trackIDs_thatpassveto_list) 
#
# #           print("energy_dep = " + str(segment[iHit]["dE"]))
# #           print("entry = " + str(jentry))

