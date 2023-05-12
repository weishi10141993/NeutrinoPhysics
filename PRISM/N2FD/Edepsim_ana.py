#! /usr/bin/env python
"""
Runs over ROOT files created by edepsim
"""

from math import sqrt

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import glob
import os
import sys
from ROOT import TG4Event, TFile

a = str(sys.argv[1])
#print(a)
#print(sys.argv[1])

# this works interactively
#edep_file = TFile("edep.123456.root")
edep_file = TFile(a, "OPEN")
if not edep_file:
  print ("Error: could not open file", a)
  sys.exit()
inputTree = edep_file.Get("EDepSimEvents")
event = TG4Event()
inputTree.SetBranchAddress("Event", event)
entries = inputTree.GetEntriesFast()
#print(entries)

trajectories_dtype = np.dtype([("pdgID", "i4"), ("trackID", "u4")]) #i4 - integer 4, u4 - unsigned integer(sort of abs value) 
# Convert from EDepSim default units (mm, ns)
edep2cm = 0.1   # convert to cm
edep2us = 0.001 # convert to microseconds

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

# Loop over events
for jentry in range(entries):
    print("jentry here = " +str(jentry))
    nb = inputTree.GetEntry(jentry) #number of bytes read
    print("nb =" + str(nb))
    print("event number", event.EventId)
    print("number of trajectories", event.Trajectories.size())
    print("number of segments", event.SegmentDetectors.size())

    for primary in event.Primaries:
        for ipart, particle in enumerate(primary.Particles):
            print("ipart here = " +str(ipart))
            PDGCode = particle.GetPDGCode()
            print("pdgcode: ", PDGCode)
            if PDGCode==-13:
                XforPrimary = primary.GetPosition().X() * edep2cm
                YforPrimary = primary.GetPosition().Y() * edep2cm
                ZforPrimary = primary.GetPosition().Z() * edep2cm
                PxforPrimary = particle.GetMomentum().X()
                PyforPrimary = particle.GetMomentum().Y()
                PzforPrimary = particle.GetMomentum().Z()
                trackIDforPrimary = particle.GetTrackId()

    trajectories = np.empty(len(event.Trajectories), dtype=trajectories_dtype)
    for iTraj, trajectory in enumerate(event.Trajectories):
        start_pt, end_pt = trajectory.Points[0], trajectory.Points[-1]
        trajectories[iTraj]["pdgID"] = trajectory.GetPDGCode()
        trajectories[iTraj]["trackID"] = trajectory.GetTrackId()

    for containerName, hitSegments in event.SegmentDetectors:
        # iHit is the index, hitSEgment is the data stored at the index in the second item in event.SegementDectectors
        for iHit, hitSegment in enumerate(hitSegments):
            primaryID = hitSegment.GetPrimaryId()
            if primaryID == trackIDforPrimary:
                # Energy deposit from primary particles
                edep_x = hitSegment.GetStop().X() * edep2cm
                edep_y = hitSegment.GetStop().Y() * edep2cm
                edep_z = hitSegment.GetStop().Z() * edep2cm
                edep = hitSegment.GetEnergyDeposit()

# Create plot of charge deposits
