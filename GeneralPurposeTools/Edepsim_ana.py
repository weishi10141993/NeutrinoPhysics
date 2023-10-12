#! /usr/bin/env python
"""
Analyze edepsim files
"""
import time
start_time = time.time()

import numpy as np
import glob
import os
import os.path

import sys

import ROOT
from ROOT import TG4Event, TFile, TTree, TCanvas, TGraph, TH1D

from optparse import OptionParser
import xml.etree.ElementTree as ET

from array import array
from math import cos, sin
import random

from ROOT import gROOT # for creating the output file

#########
# I/O
#########
# Input edepsim file from user
input_file = str(sys.argv[1])
edep_file = TFile(input_file, "OPEN")
if not edep_file:
    print ("Error: could not open file", input_file)
    sys.exit()
inputTree = edep_file.Get("EDepSimEvents")
genieTree = edep_file.Get("DetSimPassThru/gRooTracker")

event = TG4Event()
inputTree.SetBranchAddress("Event", event)

entries = inputTree.GetEntriesFast()
genie_entries = genieTree.GetEntriesFast()
if entries != genie_entries:
    print("Edep-sim tree and GENIE tree number of entries do not match!")
    sys.exit()

# Create directory for plots to be stored if it doesn't already exist
out_path = "/dune/app/users/weishi/GeneralPurposeTools/output"
if not os.path.exists(out_path):
    os.makedirs(out_path)
    print("out_path '" + out_path + "' did not exist. It has been created!")
print(" \n") # separate output

# Create the ROOT file that will hold the output of this script
f_out = TFile('{0}/edepsim_ana.root'.format(out_path), 'RECREATE')

# Convert from EDepSim default units (mm, ns)
edep2cm = 0.1   # convert to cm
edep2us = 0.001 # convert to microseconds
# Convert GENIE to common units
gev2mev  = 1000 # convert GeV to MeV

##################################
# Create output TTree
##################################

myEvents = TTree('myEvents', 'myEvents')
maxEdeps = 100000 # max number of edeps for initialize arrays
maxGenieParts = 1000

# Genie evt info
Genie_nParts = array('i', [0])
myEvents.Branch('Genie_nParts', Genie_nParts, 'Genie_nParts/I')
GenieParts_pdg = np.zeros((maxGenieParts,), dtype=np.int32)
myEvents.Branch('GenieParts_pdg', GenieParts_pdg, 'GenieParts_pdg[Genie_nParts]/I')
GenieParts_p0_MeV = np.zeros((maxGenieParts,), dtype=np.float32)
myEvents.Branch('GenieParts_p0_MeV', GenieParts_p0_MeV, 'GenieParts_p0_MeV[Genie_nParts]/F')
GenieParts_p1_MeV = np.zeros((maxGenieParts,), dtype=np.float32)
myEvents.Branch('GenieParts_p1_MeV', GenieParts_p1_MeV, 'GenieParts_p1_MeV[Genie_nParts]/F')
GenieParts_p2_MeV = np.zeros((maxGenieParts,), dtype=np.float32)
myEvents.Branch('GenieParts_p2_MeV', GenieParts_p2_MeV, 'GenieParts_p2_MeV[Genie_nParts]/F')
GenieParts_p3_MeV = np.zeros((maxGenieParts,), dtype=np.float32)
myEvents.Branch('GenieParts_p3_MeV', GenieParts_p3_MeV, 'GenieParts_p3_MeV[Genie_nParts]/F')
Genie_final_lep_pdg = array('i', [0])
myEvents.Branch('Genie_final_lep_pdg', Genie_final_lep_pdg, 'Genie_final_lep_pdg/I')
Genie_final_lep_p0_MeV = array('f', [0.0])
myEvents.Branch('Genie_final_lep_p0_MeV', Genie_final_lep_p0_MeV, 'Genie_final_lep_p0_MeV/F')
Genie_final_lep_p1_MeV = array('f', [0.0])
myEvents.Branch('Genie_final_lep_p1_MeV', Genie_final_lep_p1_MeV, 'Genie_final_lep_p1_MeV/F')
Genie_final_lep_p2_MeV = array('f', [0.0])
myEvents.Branch('Genie_final_lep_p2_MeV', Genie_final_lep_p2_MeV, 'Genie_final_lep_p2_MeV/F')
Genie_final_lep_p3_MeV = array('f', [0.0])
myEvents.Branch('Genie_final_lep_p3_MeV', Genie_final_lep_p3_MeV, 'Genie_final_lep_p3_MeV/F')

# LArBath evt info
nEdeps = array('i', [0])
myEvents.Branch('nEdeps', nEdeps, 'nEdeps/I')
deps_trkID = np.zeros((maxEdeps,), dtype=np.int32)
myEvents.Branch('deps_trkID', deps_trkID, 'deps_trkID[nEdeps]/I')
deps_parentID = np.zeros((maxEdeps,), dtype=np.int32)
myEvents.Branch('deps_parentID', deps_parentID, 'deps_parentID[nEdeps]/I')
deps_pdg = np.zeros((maxEdeps,), dtype=np.int32)
myEvents.Branch('deps_pdg', deps_pdg, 'deps_pdg[nEdeps]/I')
deps_E_MeV = np.zeros((maxEdeps,), dtype=np.float32)
myEvents.Branch('deps_E_MeV', deps_E_MeV, 'deps_E_MeV[nEdeps]/F')
deps_start_t_us = np.zeros((maxEdeps,), dtype=np.float32)
myEvents.Branch('deps_start_t_us', deps_start_t_us, 'deps_start_t_us[nEdeps]/F')
deps_stop_t_us = np.zeros((maxEdeps,), dtype=np.float32)
myEvents.Branch('deps_stop_t_us', deps_stop_t_us, 'deps_stop_t_us[nEdeps]/F')
larbath_vtx_cm = array('f', 3*[0.0])
myEvents.Branch('larbath_vtx_cm', larbath_vtx_cm, 'larbath_vtx_cm[3]/F')
# edeps generated in LArBath (start point)
larbath_deps_start_x_cm = np.zeros((maxEdeps,), dtype=np.float32)
myEvents.Branch('larbath_deps_start_x_cm', larbath_deps_start_x_cm, 'larbath_deps_start_x_cm[nEdeps]/F') # larbath edeps x
larbath_deps_start_y_cm = np.zeros((maxEdeps,), dtype=np.float32)
myEvents.Branch('larbath_deps_start_y_cm', larbath_deps_start_y_cm, 'larbath_deps_start_y_cm[nEdeps]/F')
larbath_deps_start_z_cm = np.zeros((maxEdeps,), dtype=np.float32)
myEvents.Branch('larbath_deps_start_z_cm', larbath_deps_start_z_cm, 'larbath_deps_start_z_cm[nEdeps]/F')
# edeps generated in LArBath (stop point)
larbath_deps_stop_x_cm = np.zeros((maxEdeps,), dtype=np.float32)
myEvents.Branch('larbath_deps_stop_x_cm', larbath_deps_stop_x_cm, 'larbath_deps_stop_x_cm[nEdeps]/F')
larbath_deps_stop_y_cm = np.zeros((maxEdeps,), dtype=np.float32)
myEvents.Branch('larbath_deps_stop_y_cm', larbath_deps_stop_y_cm, 'larbath_deps_stop_y_cm[nEdeps]/F')
larbath_deps_stop_z_cm = np.zeros((maxEdeps,), dtype=np.float32)
myEvents.Branch('larbath_deps_stop_z_cm', larbath_deps_stop_z_cm, 'larbath_deps_stop_z_cm[nEdeps]/F')

###########################
# Loop over edepsim events
##########################

for jentry in range(entries):
    print("jentry = " + str(jentry))
    nb = inputTree.GetEntry(jentry)
    gb = genieTree.GetEntry(jentry)
    #print("nb =" + str(nb))
    #print("event number: ", event.EventId)

    ###################################
    # Access GENIE truth info
    ###################################
    Genie_nParts[0] = 0
    Genie_final_lep_pdg[0] = 0
    Genie_final_lep_p0_MeV[0] = 0
    Genie_final_lep_p1_MeV[0] = 0
    Genie_final_lep_p2_MeV[0] = 0
    Genie_final_lep_p3_MeV[0] = 0
    all_genie_pdg_list = list()
    all_genie_p0_list = list()
    all_genie_p1_list = list()
    all_genie_p2_list = list()
    all_genie_p3_list = list()
    for p in range(genieTree.StdHepN):
        # Get only final state particles
        if genieTree.StdHepStatus[p] == 1:
            #print("GENIE pdg: ", genieTree.StdHepPdg[p], " status: ", genieTree.StdHepStatus[p], " p0: ", genieTree.StdHepP4[p*4 + 0]*gev2mev, " p1: ", genieTree.StdHepP4[p*4 + 1]*gev2mev, " p2: ", genieTree.StdHepP4[p*4 + 2]*gev2mev, " p3: ", genieTree.StdHepP4[p*4 + 3]*gev2mev)
            all_genie_pdg_list.append(genieTree.StdHepPdg[p])
            all_genie_p0_list.append(genieTree.StdHepP4[p*4 + 0]*gev2mev)
            all_genie_p1_list.append(genieTree.StdHepP4[p*4 + 1]*gev2mev)
            all_genie_p2_list.append(genieTree.StdHepP4[p*4 + 2]*gev2mev)
            all_genie_p3_list.append(genieTree.StdHepP4[p*4 + 3]*gev2mev)

            if abs(genieTree.StdHepPdg[p]) >= 11 and abs(genieTree.StdHepPdg[p]) <= 16:
                #print("GENIE final lep pdg: ", genieTree.StdHepPdg[p], " status: ", genieTree.StdHepStatus[p], " p0: ", genieTree.StdHepP4[p*4 + 0]*gev2mev, " p1: ", genieTree.StdHepP4[p*4 + 1]*gev2mev, " p2: ", genieTree.StdHepP4[p*4 + 2]*gev2mev, " p3: ", genieTree.StdHepP4[p*4 + 3]*gev2mev)
                Genie_final_lep_pdg[0] = genieTree.StdHepPdg[p]
                Genie_final_lep_p0_MeV[0] = genieTree.StdHepP4[p*4 + 0]*gev2mev
                Genie_final_lep_p1_MeV[0] = genieTree.StdHepP4[p*4 + 1]*gev2mev
                Genie_final_lep_p2_MeV[0] = genieTree.StdHepP4[p*4 + 2]*gev2mev
                Genie_final_lep_p3_MeV[0] = genieTree.StdHepP4[p*4 + 3]*gev2mev
    # End particle loop in genie
    Genie_nParts[0] = len(all_genie_pdg_list)

    ###################################
    # Access Edep sim info
    ###################################
    all_dep_startpos_list = list()
    all_dep_stoppos_list = list()
    all_dep_trkID_list  = list()
    all_dep_parentID_list = list()
    all_dep_pdg_list = list()
    all_edep_list   = list()
    all_dep_starttime_list = list()
    all_dep_stoptime_list = list()

    # initialize LArBath vertex
    larbath_vtx_cm[0] = 0; larbath_vtx_cm[1] = 0; larbath_vtx_cm[2] = 0;
    nEdeps[0] = 0

    #print("number of primaries: ", event.Primaries.size())
    #print("number of trajectories: ", event.Trajectories.size())
    #print("number of segments: ", event.SegmentDetectors.size())

    for primary in event.Primaries:
        #print("number of particles: ", primary.Particles.size())
        for ipart, particle in enumerate(primary.Particles):
            #print("ipart here = " +str(ipart))
            PDGCode = particle.GetPDGCode()
            #print("pdgcode: ", PDGCode)
            if abs(PDGCode) >= 11 and abs(PDGCode) <= 16:
                posx = primary.GetPosition().X() * edep2cm
                posy = primary.GetPosition().Y() * edep2cm
                posz = primary.GetPosition().Z() * edep2cm
                momx = particle.GetMomentum().X()
                momy = particle.GetMomentum().Y()
                momz = particle.GetMomentum().Z()
                PrimaryLepTrackID = particle.GetTrackId()
                """
                print("pos x: ", posx, " y: ", posy, " z: ", posz, " [cm]")
                print("mom x: ", momx, " y: ", momy, " z: ", momz, " [MeV]")
                print("PrimaryLepTrackID: ", PrimaryLepTrackID)
                """

    trajectories_parentid = np.empty(len(event.Trajectories), dtype=np.int32)
    trajectories_pdg = np.empty(len(event.Trajectories), dtype=np.int32)
    for iTraj, trajectory in enumerate(event.Trajectories):
        # iTraj is same as TrackId, starts from #0
        trajectories_parentid[iTraj] = trajectory.GetParentId()
        trajectories_pdg[iTraj] = trajectory.GetPDGCode()
        #print("iTraj #", iTraj, " parentID: ", trajectories_parentid[iTraj], " pdg: ", trajectories_pdg[iTraj] )

    for containerName, hitSegments in event.SegmentDetectors:
        # iHit is the index, hitSEgment is the data stored at the index in the second item in event.SegementDectectors
        for iHit, hitSegment in enumerate(hitSegments):

            # Energy deposit from primary particles
            edep_start_x = hitSegment.GetStart().X() * edep2cm
            edep_start_y = hitSegment.GetStart().Y() * edep2cm
            edep_start_z = hitSegment.GetStart().Z() * edep2cm
            edep_start_t = hitSegment.GetStart().T() * edep2us
            edep_stop_x = hitSegment.GetStop().X() * edep2cm
            edep_stop_y = hitSegment.GetStop().Y() * edep2cm
            edep_stop_z = hitSegment.GetStop().Z() * edep2cm
            edep_stop_t = hitSegment.GetStop().T() * edep2us
            edep_x = (edep_start_x + edep_stop_x)/2 # use this for hadronic veto
            edep_y = (edep_start_y + edep_stop_y)/2
            edep_z = (edep_start_z + edep_stop_z)/2
            edep_trkID = hitSegment.Contrib[0] # what is different if use GetPrimaryId()?
            edep_parentID = trajectories_parentid[edep_trkID]
            edep_pdg = trajectories_pdg[edep_trkID]
            edep = hitSegment.GetEnergyDeposit()

            all_dep_startpos_list.append(edep_start_x)
            all_dep_startpos_list.append(edep_start_y)
            all_dep_startpos_list.append(edep_start_z)
            all_dep_starttime_list.append(edep_start_t)
            all_dep_stoppos_list.append(edep_stop_x)
            all_dep_stoppos_list.append(edep_stop_y)
            all_dep_stoppos_list.append(edep_stop_z)
            all_dep_stoptime_list.append(edep_stop_t)
            all_dep_trkID_list.append(edep_trkID)
            all_dep_parentID_list.append(edep_parentID)
            all_dep_pdg_list.append(edep_pdg)
            all_edep_list.append(edep)

    # for use in processing events before and after transformations
    nEdeps[0] = len(all_edep_list)


    #################################
    # Save to output
    #################################
    # Genie info
    GenieParts_pdg[:Genie_nParts[0]] = np.array(all_genie_pdg_list, dtype=np.int32)
    GenieParts_p0_MeV[:Genie_nParts[0]] = np.array(all_genie_p0_list, dtype=np.float32)
    GenieParts_p1_MeV[:Genie_nParts[0]] = np.array(all_genie_p1_list, dtype=np.float32)
    GenieParts_p2_MeV[:Genie_nParts[0]] = np.array(all_genie_p2_list, dtype=np.float32)
    GenieParts_p3_MeV[:Genie_nParts[0]] = np.array(all_genie_p3_list, dtype=np.float32)

    # Edep in LArBath info
    deps_trkID[:nEdeps[0]] = np.array(all_dep_trkID_list, dtype=np.int32)
    deps_parentID[:nEdeps[0]] = np.array(all_dep_parentID_list, dtype=np.int32)
    deps_pdg[:nEdeps[0]] = np.array(all_dep_pdg_list, dtype=np.int32)
    deps_E_MeV[:nEdeps[0]] = np.array(all_edep_list, dtype=np.float32)
    deps_start_t_us[:nEdeps[0]] = np.array(all_dep_starttime_list, dtype=np.float32)
    deps_stop_t_us[:nEdeps[0]] = np.array(all_dep_stoptime_list, dtype=np.float32)
    larbath_vtx_cm[0] = posx
    larbath_vtx_cm[1] = posy
    larbath_vtx_cm[2] = posz
    larbath_deps_start_x_cm[:nEdeps[0]] = np.array(all_dep_startpos_list[::3], dtype=np.float32) # every 3 element: x list
    larbath_deps_start_y_cm[:nEdeps[0]] = np.array(all_dep_startpos_list[1::3], dtype=np.float32) # y list
    larbath_deps_start_z_cm[:nEdeps[0]] = np.array(all_dep_startpos_list[2::3], dtype=np.float32) # z list
    larbath_deps_stop_x_cm[:nEdeps[0]] = np.array(all_dep_stoppos_list[::3], dtype=np.float32)
    larbath_deps_stop_y_cm[:nEdeps[0]] = np.array(all_dep_stoppos_list[1::3], dtype=np.float32)
    larbath_deps_stop_z_cm[:nEdeps[0]] = np.array(all_dep_stoppos_list[2::3], dtype=np.float32)

    # event level
    myEvents.Fill()

f_out.cd()
myEvents.Write()

print("\n")
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n")
