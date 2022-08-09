#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jan 24 16:30:37 2022

@author: barwu
"""

from uproot4 import concatenate, exceptions
from uproot3 import recreate, newtree
import numpy as np
from glob import glob
from sys import argv

# The code is currently quite slow, so it uses multiprocessing to speed things up
from multiprocessing import Pool
# SET NUMBER OF PROCESSORS HERE
NUM_PROCS = 40

import torch
from muonEffModel import muonEffModel
from os.path import splitext, basename, exists
from scipy.spatial.transform import Rotation as R
from scipy.interpolate import interp1d
#import math

#from pickle import dump

# ND coordinate offset.
offset = [ 0., 5.5, 411. ]

# Whether or not to apply FV cut to throws. Should be set to true.
APPLY_FV_CUT = True

#from ROOT import TGraph
from array import array

# Average neutrino decay position in beam coordinates as a function of vertex x (from Luke): Will be used to set the decay position event-by-event.
OffAxisPoints = array('f', [-2, 0.5, 3,    5.5, 8, 10.5, 13, 15.5, 18,  20.5, 23,  25.5, 28,   30.5])
meanPDPZ = array('f', [ 93.6072, 93.362,  90.346, 85.6266, 81.1443, 76.6664, 73.0865, 69.8348, 67.5822, 65.005, 62.4821, 60.8336, 59.1433, 57.7352]) #why are these discreet data sets?
#gDecayZ = TGraph(14, OffAxisPoints, meanPDPZ)
gDecayZ=interp1d(OffAxisPoints,meanPDPZ,fill_value='extrapolate')

# These are used to translate between the near detector coordinate system and the neutrino beamline coordinate system. We use this to calculate the average neutrino direction, assuming the mean neutrino production point as a function of neutrino interaction x, which is given in the arrays above.
beamRefDetCoord = [0.0, 0.05387, 6.66] #spherical coords, radians
detRefBeamCoord = [0, 0, 562.1179] #xyz coords of reference detector position
beamLineRotation = -0.101

# Fiducial volume definition
def isFV(x, y, z) :
    inDeadRegion = False

    for i in [-3, -2, -1, 0, 1, 2, 3] :
        cathode_center = i*102.1

        if (x > cathode_center - 0.75) and (x < cathode_center + 0.75) :
            inDeadRegion = True

        module_boundary = i*102.1+51.05
        if (i <= 2) and (x > module_boundary - 1.3) and (x < module_boundary + 1.3) :
            inDeadRegion = True

    for i in [1, 2, 3, 4] :
        module_boundary = i*101.8 - 0.6
        if (z > module_boundary - 1.7) and (z < module_boundary + 1.7) :
            inDeadRegion = True

    return (abs(x) < 300) and (abs(y) < 100) and (z > 50) and (z < 350) and (not inDeadRegion)

# Vectorize fiducial volume function
isFV_vec = np.vectorize(isFV)

# Simple muon containment cut
def isContained(x, y, z) :
    if abs(x) > 350 :
        return False
    if abs(y) > 150 :
        return False
    if z < 0 or z > 500 :
        return False
    return True

treeVarsToRead = ['isCC',
                  'nuPDG',
                  'Ev',
                  'LepE',
                  "LepPDG",
                  'LepMomX',
                  'LepMomY', 
                  'LepMomZ',
                  'NuMomX',
                  'NuMomY', 
                  'NuMomZ',
                  'vtx_x',
                  'vtx_y',
                  'vtx_z',
                  'muon_tracker',
                  'LepNuAngle',
                  'geoEffThrowResults',
                  'event',
                  'Ehad_veto',
                  'muon_endpoint']

#CAF_FHC_fName="/home/barwu/repos/MuonEffNN/FHC.1001666.CAF.root"
#CAF_FHC_fName="/storage/shared/cvilela/CAF/ND_v7/03/FHC.10030*.CAF.root"
#CAF_FHC_fName="/storage/shared/cvilela/CAF/ND_v7/*/FHC.*.CAF.root"
CAF_FHC_fName="/storage/shared/cvilela/CAF/ND_v7/0"+argv[1]+"/FHC.100"+argv[1]+"*.CAF.root"
#CAF_FHC_fName="/storage/shared/cvilela/CAF/ND_v7/00/FHC.1000[1-2]*.CAF.root"
#CAF_FHC_fName="/storage/shared/cvilela/CAF/ND_v7/0"+argv[1]+"/FHC.100"+argv[1]+argv[2]+"*.CAF.root"
#CAF_RHC_fName="/storage/shared/cvilela/CAF/ND_v7/0"+argv[1]+"/RHC.100"+argv[1]+argv[2]+"*.CAF.root"
allFiles=glob(CAF_FHC_fName)
#allFiles+=glob(CAF_RHC_fName)
#cpu processing is set up later in the script

"""
allFiles=[]
file_list=open("/home/barwu/repos/MuonEffNN/9thTry/missed.files.txt","r")
for file_num in file_list:
    n=int(file_num)
    d=str(n//1000-1000)
    print(d,n)
    allFiles.append("/storage/shared/cvilela/CAF/ND_v7/0"+d+"/FHC."+str(n)+".CAF.root")
file_list.close()
""" #discovered that there are 76 CAF files without TTrees

# Same set of throws is used for every 100 events
N_EVENTS_PER_THROW = 100

# Just for plotting
efficiencyProjections = [ ["Ev", 40, 0, 8],
                          ["LepE", 40, 0, 8],
                          ["vtx_x", 80, -400, 400],
                          ["vtx_y", 60, -150, 150],
                          ["vtx_z", 80, 0, 400] ,
                          ["LepMomX", 40, -2, 2],
                          ["LepMomY", 40, -3, 1],
                          ["LepMomZ", 60, -0.5, 5.5] ]

# This is the function where everything happens
def processFiles(f):
    # Analyse one file at a time, otherwise memory explodes!
    #f is only 1 file, each file get assigned to a different cpu
    #for f in f_list :
        output="/home/barwu/repos/MuonEffNN/9thTry/test/"+splitext(basename(f))[0]+"_MuonEff.root" #need to come up with a new place to put the TTrees
        if exists(output)==True:
            #print("testing")
            return None
        try :
            # Get caf TTree
            CAF = concatenate("{0}:caf".format(f), treeVarsToRead, library = "np")
            #CAF=uproot4.open(f)['caf']
            #fUprootIn = uproot.open(f)
            #CAF = fUprootIn['caf']
        except exceptions.KeyInFileError: #leave except condition specification so that code crashes when there is another exception condition
            #print("Couldn't find CAF TTree in file {0}. Skipping.".format(f))
            return None
            #continue
        #print(f)

        # Figure out what events pass the fiducial volume cut
        if APPLY_FV_CUT :
            CAF["inFV"] = isFV_vec(CAF["vtx_x"], CAF["vtx_y"], CAF["vtx_z"])
        else :
            CAF["inFV"] = [True]*len(CAF["vtx_x"])

        # Get tree with x, y and phi of geometric efficiency throws.
        geoThrows = concatenate("{0}:geoEffThrows".format(f), ['geoEffThrowsY', 'geoEffThrowsZ', 'geoEffThrowsPhi'], library = "np")

        # Arrays to store the efficiencies
        effs = np.array([0.]*len(CAF['geoEffThrowResults']), dtype = np.float16)
        effs_tracker = np.array([0.]*len(CAF['geoEffThrowResults']), dtype = np.float16)
        effs_contained = np.array([0.]*len(CAF['geoEffThrowResults']), dtype = np.float16)
        effs_combined = np.array([0.]*len(CAF['geoEffThrowResults']), dtype = np.float16)

        # Event loop
        for i_event in range(len(CAF['geoEffThrowResults'])):
            #if i_event==10: break #use when debugging
        #for i_event in range(200) :
            #print(i_event)

            # Accumulators for efficiency calculation
            # Hadronic efficiency
            thisEff = 0.
            # Tracker-match efficiency
            thisEff_tracker = 0.
            # Contained muon efficiency
            thisEff_contained = 0.
            # Combined efficiency
            thisEff_combined = 0.

            # Check which throws are in the FV. throws_FV is a boolean array with one element per throw.
            throws_FV = isFV_vec([CAF["vtx_x"][i_event]]*len(geoThrows["geoEffThrowsY"][int(i_event/N_EVENTS_PER_THROW)]),
                                 geoThrows["geoEffThrowsY"][int(i_event/N_EVENTS_PER_THROW)]-offset[1],
                                 geoThrows["geoEffThrowsZ"][int(i_event/N_EVENTS_PER_THROW)]-offset[2])

            # Count how many throws were in the FV. Will be useful later.
            NthrowsInFV = sum(throws_FV)
            if NthrowsInFV==0 and APPLY_FV_CUT:
                effs[i_event]=-1.
                effs_tracker[i_event]=-1.
                effs_contained[i_event]=-1.
                effs_combined[i_event]=-1.
                continue

            # Loop through the hadronic geometric efficiency throw results. Each bitfield corresponds to 64 throws. There are 64*78 = 4992 throws in total
            for i_bitfield, bitfield in enumerate(CAF['geoEffThrowResults'][i_event][0][1]) :
                # Convert 64 bit integer into "bit" array
                bitfield = np.array([bitfield], dtype = np.uint64)
                bitfield = np.unpackbits(np.array(bitfield, dtype='>i8').view(np.uint8)) #unpackbits converts a 256-basevalue nto an array of binary integers
                bitfieldTemp = np.copy(bitfield)

                # Annoyingly, the array is backwards. Invert array order...
                for j_bitfield in range(len(bitfield)) :
                    bitfield[-(1+j_bitfield)] = bitfieldTemp[j_bitfield]

                # Calculate hadron efficiency. Sum the number of throws where the hadronic system was contained (bit in bitfield is 1) and the throw was in the fiducial volume.
                if APPLY_FV_CUT :
                    thisEff += np.sum(np.logical_and(bitfield, throws_FV[i_bitfield*64:(i_bitfield+1)*64]))
                else :
                    thisEff += np.sum(bitfield)

                # Get variables needed to evaluate muon neural network for each throw.
                
                # Get new XYZ from throws
                # x is not randomized. This is a convoluted way of repeating vtx_x the correct number of times
                throw_x = [CAF["vtx_x"][i_event]]*len(geoThrows["geoEffThrowsY"][int(i_event/N_EVENTS_PER_THROW)][i_bitfield*64:(i_bitfield+1)*64])
                # Get y for each random throw.
                throw_y = geoThrows["geoEffThrowsY"][int(i_event/N_EVENTS_PER_THROW)][i_bitfield*64:(i_bitfield+1)*64]-offset[1]
                # Get z for each random throw
                throw_z = geoThrows["geoEffThrowsZ"][int(i_event/N_EVENTS_PER_THROW)][i_bitfield*64:(i_bitfield+1)*64]-offset[2]

                # Get rotation matrices from throws
                # Get phi for each random throw
                throw_phi = geoThrows["geoEffThrowsPhi"][int(i_event/N_EVENTS_PER_THROW)][i_bitfield*64:(i_bitfield+1)*64]
                #where is the randomization? these throw_... parameters should be a randomized set of digits.

                # Use vertex to determine mean decay point
                # Get z-coordinate of neutrino production point *in beamline coordinates*
                #decayZbeamCoord = gDecayZ.Eval(CAF["vtx_x"][i_event] / 100 - detRefBeamCoord[0])*100 # in cm
                decayZbeamCoord = gDecayZ((CAF["vtx_x"][i_event] / 100 - detRefBeamCoord[0])*100) # in cm
                # Convert neutrino production point to *near detector coordinates*
                decayZdetCoord = (-1*detRefBeamCoord[2]*100+decayZbeamCoord)*np.cos(beamLineRotation) - (-1*detRefBeamCoord[1]*100*np.sin(beamLineRotation)) + beamRefDetCoord[2]*100
                decayYdetCoord = (-1*detRefBeamCoord[1]*100*np.cos(beamLineRotation)) + (-1*detRefBeamCoord[2]*100+decayZbeamCoord)*np.sin(beamLineRotation) + beamRefDetCoord[1]*100
                decayXdetCoord = -1*detRefBeamCoord[0]*100 + beamRefDetCoord[0]*100

                # Vector from neutrino production point to original event vertex
                decayToVertex = [CAF["vtx_x"][i_event] - decayXdetCoord,
                                 CAF["vtx_y"][i_event] - decayYdetCoord,
                                 CAF["vtx_z"][i_event] - decayZdetCoord]
                
                # Vector from neutrino production point to randomly thrown vertex.
                decayToTranslated = [ [throw_x[i] - decayXdetCoord, throw_y[i] - decayYdetCoord, throw_z[i] - decayZdetCoord] for i in range(len(throw_x)) ]

                magDecayToVertex = np.sqrt(np.sum(np.square(decayToVertex)))
                magDecayToTranslated = np.sqrt(np.sum(np.square(decayToTranslated), axis = 1))
                
                translationAngle = np.dot(decayToTranslated, decayToVertex)
                translationAngle = np.divide(translationAngle, np.multiply(magDecayToVertex,magDecayToTranslated));
                #for angleval in translationAngle:
                    #if angleval<=-1 or angleval>=1: print(i_event, angleval)
                translationAngle = np.arccos(translationAngle);

                translationAxis = np.cross(decayToTranslated, decayToVertex)
                translationAxis = [ thisV/np.linalg.norm(thisV) for thisV in translationAxis ]

                translation_rot_vec = np.multiply(translationAxis, translationAngle[...,None])

                decayToTranslated = [ thisV/np.linalg.norm(thisV) for thisV in decayToTranslated ]

                phi_rot_vec = np.multiply(decayToTranslated, throw_phi[...,None])
                
                this_px = CAF["LepMomX"][i_event]
                this_py = CAF["LepMomY"][i_event]
                this_pz = CAF["LepMomZ"][i_event]

                this_p = [this_px, this_py, this_pz] #lepton momentum list

                # Get rotation matrices due to:
                # Vertex translation (which "rotates" the average neutrino direction)
                translation_rot = R.from_rotvec(translation_rot_vec)
                # Random phi rotation around average neutrino direction
                phi_rot = R.from_rotvec(phi_rot_vec)

                # Rotate momentum
                this_p = translation_rot.apply(this_p)
                this_p = phi_rot.apply(this_p)

                # Features contains randomized momentum and vertex, ready to be used in neural network.
                features = np.column_stack((this_p[:,0], this_p[:,1], this_p[:,2],
                                            throw_x, throw_y, throw_z))

                # Convert to Pytorch tensor
                features = torch.as_tensor(features).type(torch.FloatTensor)

                # Evaluate neural network
                with torch.no_grad() :
                    netOut = net(features)
                    netOut = torch.nn.functional.softmax(netOut).detach().numpy()

                # Get contained probability for each throw
                nnContained = np.array(netOut[:,0], dtype = float)
                # Get tracker probability for each throw
                nnTracker = np.array(netOut[:,1], dtype = float)

                # MUON CONTAINED AND HADRON CONTAINED, throw by throw
                combinedEfficiencyContained = np.multiply(nnContained, bitfield)
                # OR: MUON TRACKER AND HADRON CONTAINED, throw by throw
                combinedEfficiencyTracker = np.multiply(nnTracker, bitfield)
                # Combined hadron containment and muon selection efficiency, throw by throw
                combinedEfficiency = np.add(combinedEfficiencyContained, combinedEfficiencyTracker)

                # Count only throws which were in the fiducial volume and add them to the efficiency accumulators
                if APPLY_FV_CUT :
                    thisEff_tracker += np.sum(np.multiply(nnTracker, throws_FV[i_bitfield*64:(i_bitfield+1)*64]))
                    thisEff_contained += np.sum(np.multiply(nnContained, throws_FV[i_bitfield*64:(i_bitfield+1)*64]))
                    thisEff_combined += np.sum(np.multiply(combinedEfficiency, throws_FV[i_bitfield*64:(i_bitfield+1)*64]))
                else :
                    thisEff_tracker += np.sum(nnTracker)
                    thisEff_contained += np.sum(nnContained)
                    thisEff_combined += np.sum(combinedEfficiency)
                    
            # After looping through all throws, divide by number of throws in the fiducial volume to get average efficiency.
            if APPLY_FV_CUT :
                effs[i_event] = float(thisEff)/NthrowsInFV
                effs_tracker[i_event] = float(thisEff_tracker)/NthrowsInFV
                effs_contained[i_event] = float(thisEff_contained)/NthrowsInFV
                effs_combined[i_event] = float(thisEff_combined)/NthrowsInFV
            else :
                effs[i_event] = thisEff/(78.*64)
                effs_tracker[i_event] = float(thisEff_tracker)/(78.*64)
                effs_contained[i_event] = float(thisEff_contained)/(78.*64)
                effs_combined[i_event] = float(thisEff_combined)/(78.*64)
            #print("still running")

        # What follows is all related to making histograms with matplotlib
        fv = np.logical_and(CAF["inFV"], CAF["isCC"])
        numu = np.absolute(CAF["LepPDG"]) == 13
        fv = np.logical_and(fv, numu)
        no_weird_events = np.absolute(np.sum(CAF["muon_endpoint"], axis = 1)) > 0.
        fv = np.logical_and(fv, no_weird_events)
        
        had_containment = CAF["Ehad_veto"] < 30
        sel = np.logical_and(had_containment, fv)
        sel_tracker = np.logical_and(CAF['muon_tracker'] > 0, fv)
        
        isContained_vec = np.vectorize(isContained)
        sel_contained = np.logical_and(isContained_vec(CAF["muon_endpoint"][:,0], CAF["muon_endpoint"][:,1], CAF["muon_endpoint"][:,2]), fv)
        
        sel_combined = np.logical_and(np.logical_or(sel_tracker, sel_contained), sel)
        
        print("Number in FV {0}, number contained {1}, number in FV and contained {2}".format(sum(fv), sum(had_containment), sum(sel)))
    
        effs_sel = effs[sel]
        print("HADRON")
        print(effs_sel)
        effs_sel_inv = np.reciprocal(effs_sel)
    
        effs_sel_tracker = effs_tracker[sel_tracker]
        effs_sel_tracker_inv = np.reciprocal(effs_sel_tracker)

        effs_sel_tracker_inv[effs_sel_tracker_inv > 500] = 500
        
        effs_sel_contained = effs_contained[sel_contained]
        effs_sel_contained_inv = np.reciprocal(effs_sel_contained)

        effs_sel_contained_inv[effs_sel_contained_inv > 500] = 500
    
        effs_sel_combined = effs_combined[sel_combined]
        effs_sel_combined_inv = np.reciprocal(effs_sel_combined)

        i=0
        while i<len(effs):
            if np.isinf(effs[i]):effs[i]=-8.
            if np.isinf(effs_tracker[i]):effs_tracker[i]=-8.
            if np.isinf(effs_contained[i]):effs_contained[i]=-8.
            if np.isinf(effs_combined[i]):effs_combined[i]=-8.
            if np.isnan(effs[i]):effs[i]=-1.
            if np.isnan(effs_tracker[i]):effs_tracker[i]=-1.
            if np.isnan(effs_contained[i]):effs_contained[i]=-1.
            if np.isnan(effs_combined[i]):effs_combined[i]=-1.
            if effs[i]>1:
                print("hadron selected efficiency of event",i,"is greater than 1")
                effs[i]=-11.
            if effs_tracker[i]>1:
                print("muon tracker efficiency of event",i,"is greater than 1")
                effs_tracker[i]=-11.
            if effs_contained[i]>1:
                print("muon contained efficiency of event",i,"is greater than 1")
                effs_contained[i]=-11.
            if effs_combined[i]>1:
                print("combined efficiency of event",i,"is greater than 1")
                effs_combined[i]=-11.
            #if CAF['isCC'][i]==False: print("CAF event",i,"is not an isCC event")
            i+=1

# NOTE: IF saving the efficiencies to ROOT TTree for further processing, this is the place to Fill the TTree with the effs[i_event] variables. This might interfere with multi-processing, so I recommend doing this only with NUM_PROCS set to 1.
        #output="/home/barwu/repos/MuonEffNN/9thTry/test/"+splitext(basename(f))[0]+"_MuonEff.root" #need to come up with a new place to put the TTrees
        with recreate(output) as tree:
            tree["event_data"]=newtree({#'LepMomX':np.float64,
                                                #'LepMomY':np.float64, 
                                                #'LepMomZ':np.float64,
                                                #'NuMomX':np.float64,
                                                #'NuMomY':np.float64, 
                                                #'NuMomZ':np.float64,
                                                #'vtx_x':np.float64,
                                                #'vtx_y':np.float64,
                                                #'vtx_z':np.float64,
                                                'isCC':np.int32,
                                                'inFV':np.int32,
                                                #'Ev':np.int32,
                                                #'LepE':np.float64,
                                                #'LepNuAngle':np.float64,
                                                'cos_LepNuAngle':np.float64,
                                                'TotMom':np.float64,
                                                'LongMom':np.float64,
                                                'muon_contained_eff':np.float64,
                                                'muon_tracker_eff':np.float64,
                                                #'muon_selected_eff':np.float64,
                                                'hadron_selected_eff':np.float64,
                                                'combined_eff':np.float64,
                                                'hadron_selected':np.int32,
                                                'muon_contained':np.int32,
                                                'muon_tracker':np.int32,
                                                'muon_selected':np.int32,
                                                'combined':np.int32})

            extend_dict={}
            #for var in ['LepMomX','LepMomY','LepMomZ','NuMomX','NuMomY','NuMomZ','vtx_x','vtx_y','vtx_z','isCC','Ev','LepE','LepNuAngle']: extend_dict[var]=CAF[var]
            totmom=np.sqrt(np.power(CAF['LepMomX'],2)+np.power(CAF['LepMomY'],2)+np.power(CAF['LepMomZ'],2))
            cosangle=np.cos(CAF['LepNuAngle'])

            extend_dict['inFV']=CAF['inFV']
            extend_dict['isCC']=CAF['isCC']
            extend_dict['hadron_selected_eff']=effs
            extend_dict['muon_contained_eff']=effs_contained
            extend_dict['muon_tracker_eff']=effs_tracker
            #extend_dict['muon_selected_eff']=np.reciprocal(np.reciprocal(effs_contained)+np.reciprocal(effs_tracker))
            extend_dict['combined_eff']=effs_combined
            extend_dict['hadron_selected']=sel
            extend_dict['muon_contained']=sel_contained
            extend_dict['muon_tracker']=sel_tracker
            extend_dict['muon_selected']=np.add(sel_contained,sel_tracker)
            extend_dict['combined']=sel_combined
            extend_dict['TotMom']=totmom
            extend_dict['cos_LepNuAngle']=cosangle
            extend_dict['LongMom']=np.multiply(totmom,cosangle)

            tree["event_data"].extend(extend_dict)

        #print("TRACKER")
        #print(effs_sel_tracker)
        #print("CONTAINED")
        #print(effs_sel_contained)
        #print("COMBINED")
        #print(effs_sel_combined)

if __name__ == "__main__" :

    net = muonEffModel()
    net.load_state_dict(torch.load("/home/barwu/repos/MuonEffNN/8thTry/muonEff30.nn", map_location=torch.device('cpu')))
    net.eval()
    
    #if len(allFiles) < NUM_PROCS :
        #print("Fewer files than processes, setting NUM_PROC to {0}".format(len(allFiles)))
        #NUM_PROCS = len(allFiles)

    #filesPerProc = int(np.ceil(float(len(allFiles))/NUM_PROCS))
    #print(filesPerProc, NUM_PROCS)

    pool=Pool(NUM_PROCS)
    pool.map(processFiles, allFiles)
        #don't use multiprocessing for debugging
    #for file in allFiles:
     #   processFiles(file)
    #processFiles("/storage/shared/cvilela/CAF/ND_v7/00/FHC.1000999.CAF.root")
