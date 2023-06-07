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
dimension = 40000 #cm
#print(a)
#print(sys.argv[1])

# this works interactively
#edep_file = TFile("edep.*.root")
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

# Loop over events
for jentry in range(entries):
    print("jentry here = " +str(jentry))
    nb = inputTree.GetEntry(jentry) #number of bytes read
    print("nb =" + str(nb))
    #print("event number: ", event.EventId)
    #print("number of primaries: ", event.Primaries.size())
    #print("number of trajectories: ", event.Trajectories.size())
    #print("number of segments: ", event.SegmentDetectors.size())
    edep_x_list=list()
    edep_y_list=list()
    edep_z_list=list()
    edep_list=list()

    for primary in event.Primaries:
        #print("number of particles: ", primary.Particles.size())
        for ipart, particle in enumerate(primary.Particles):
            #print("ipart here = " +str(ipart))
            PDGCode = particle.GetPDGCode()
            #print("pdgcode: ", PDGCode)
            if PDGCode == -13: #mu+
                posx = primary.GetPosition().X() * edep2cm
                posy = primary.GetPosition().Y() * edep2cm
                posz = primary.GetPosition().Z() * edep2cm
                momx = particle.GetMomentum().X()
                momy = particle.GetMomentum().Y()
                momz = particle.GetMomentum().Z()
                trackIDforPrimary = particle.GetTrackId()
                #print("mu+ pos x: ", posx, " y: ", posy, " z: ", posz, " [cm]")
                #print("mu+ mom x: ", momx, " y: ", momy, " z: ", momz, " [MeV]")
                #print("trackIDforPrimary: ", trackIDforPrimary)

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
                edep_x_list.append(edep_x)
                edep_y_list.append(edep_y)
                edep_z_list.append(edep_z)
                edep_list.append(edep)
                #print("edep: ", edep)

    #edep_array=np.array(edep_list)

    # Create plot of charge deposits per event
    plt.scatter(edep_x_list, edep_y_list, s=5, c=edep_list)
    cbar = plt.colorbar()
    cbar.ax.set_ylabel('[MeV]')
    plt.xlabel('x (cm)')
    plt.ylabel('y (cm)')
    plt.xlim([-dimension/2, dimension/2])
    plt.ylim([-dimension/2, dimension/2])
    plt.savefig("plots/evt_{0}_xy.png".format(jentry))
    plt.clf() # clear figure
    plt.close()
    plt.scatter(edep_z_list, edep_x_list, s=5, c=edep_list)
    cbar = plt.colorbar()
    cbar.ax.set_ylabel('[MeV]')
    plt.xlabel('z (cm)')
    plt.ylabel('x (cm)')
    plt.xlim([-dimension/2, dimension/2])
    plt.ylim([-dimension/2, dimension/2])
    plt.savefig("plots/evt_{0}_zx.png".format(jentry))
    plt.clf()
    plt.close()
    plt.scatter(edep_z_list, edep_y_list, s=5, c=edep_list)
    cbar = plt.colorbar()
    cbar.ax.set_ylabel('[MeV]')
    plt.xlabel('z (cm)')
    plt.ylabel('y (cm)')
    plt.xlim([-dimension/2, dimension/2])
    plt.ylim([-dimension/2, dimension/2])
    plt.savefig("plots/evt_{0}_zy.png".format(jentry))
    plt.clf()
    plt.close()

    fig = plt.figure(figsize=(12, 12))
    ax = fig.add_subplot(projection='3d')
    p = ax.scatter(edep_z_list, edep_x_list, edep_y_list, s=20, c=edep_list)
    cbar = fig.colorbar(p)
    cbar.set_label('[MeV]')
    ax.set_xlabel('z (cm)')
    ax.set_ylabel('x (cm)')
    ax.set_zlabel('y (cm)')
    ax.set_xlim([-dimension/2, dimension/2])
    ax.set_ylim([-dimension/2, dimension/2])
    ax.set_zlim([-dimension/2, dimension/2])
    fig.savefig("plots/evt_{0}_xyz.png".format(jentry))
    plt.clf()
    plt.close()
