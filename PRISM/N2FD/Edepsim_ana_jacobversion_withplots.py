#! /usr/bin/env python
"""
Runs over ROOT files created by edepsim and places events in various
configurations to be used in machine learning studies for correlated
signals between the DUNE Near Detector (ND) and Far Detector (FD).
There is also the option to produce validation plots to see if the
data is being manipulated correctly. At the time of writing this
(Aug. 18, 2023), I (Jacob Smith, smithja) was looking at if 
translations and rotations distorted the event or affected all energy
deposits in an event equally.

This code was written by Wei Shi and Jacob Smith.
Jacob Smith can be reached at jacob.a.smith@stonybrook.edu.
"""
import time
start_time = time.time()

import numpy as np
import matplotlib.pyplot as plt
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

with open("UserConfig.py") as infile:
    exec(infile.read())
import pyGeoEff

from ROOT import gROOT # for creating the output file

ROOT.gROOT.ProcessLine('#include<vector>') # for outputting data to ROOT file


def quality_check_graphs( nEDeps, all_before_trans_coords, all_after_trans_coords, had_before_trans_coords, had_after_trans_coords, 
                            root_hists, title_delineator, save_file_delineator):

    # unpack number of energy deposits
    n_allEdeps = nEdeps[0]
    n_hadEdeps = nEdeps[1]
    #print( "n_allEdeps: ", n_allEdeps)
    #print( "n_hadEdeps: ", n_hadEdeps)

    # unpack the coordinates
    all_before_trans_x = all_before_trans_coords[0] 
    all_before_trans_y = all_before_trans_coords[1]
    all_before_trans_z = all_before_trans_coords[2]
    all_after_trans_x  = all_after_trans_coords[0] 
    all_after_trans_y  = all_after_trans_coords[1]
    all_after_trans_z  = all_after_trans_coords[2]
    
    had_before_trans_x = had_before_trans_coords[0] 
    had_before_trans_y = had_before_trans_coords[1]
    had_before_trans_z = had_before_trans_coords[2]
    had_after_trans_x  = had_after_trans_coords[0] 
    had_after_trans_y  = had_after_trans_coords[1]
    had_after_trans_z  = had_after_trans_coords[2]

    # calculate the distances each energy deposit is from the point of reference,
    # e.g. detector origin, event vertex
    all_distdep_before = np.sqrt( all_before_trans_x**2 + all_before_trans_y**2 + all_before_trans_z**2)
    all_distdep_after  = np.sqrt( all_after_trans_x**2 + all_after_trans_y**2 + all_after_trans_z**2)
    
    had_distdep_before = np.sqrt( had_before_trans_x**2 + had_before_trans_y**2 + had_before_trans_z**2)
    had_distdep_after  = np.sqrt( had_after_trans_x**2 + had_after_trans_y**2 + had_after_trans_z**2)

    # calculate the distance from the point of reference to the midpoints of 
    # adjacent energy deposits before the transformation
    all_distdep_mid_before = ( all_distdep_before[1:] + all_distdep_before[:-1]) / 2
    had_distdep_mid_before = ( had_distdep_before[1:] + had_distdep_before[:-1]) / 2

    # calculate positional midpoints of adjacent energy deposits
    # relative to the point of reference before the transformation
    # used for plotting the 3d graphs showing the difference in
    # energy deposit distances
    all_xmid_before = ( all_before_trans_x[1:] + all_before_trans_x[:-1]) / 2
    all_ymid_before = ( all_before_trans_y[1:] + all_before_trans_y[:-1]) / 2
    all_zmid_before = ( all_before_trans_z[1:] + all_before_trans_z[:-1]) / 2

    had_xmid_before = ( had_before_trans_x[1:] + had_before_trans_x[:-1]) / 2
    had_ymid_before = ( had_before_trans_y[1:] + had_before_trans_y[:-1]) / 2
    had_zmid_before = ( had_before_trans_z[1:] + had_before_trans_z[:-1]) / 2

    # create the arrays that will contain the differences between adjacent 
    # entries before and after the transformation
    all_adjDiff_before_x = np.diff( all_before_trans_x)
    all_adjDiff_before_y = np.diff( all_before_trans_y)
    all_adjDiff_before_z = np.diff( all_before_trans_z)
    all_adjDiff_after_x  = np.diff( all_after_trans_x)
    all_adjDiff_after_y  = np.diff( all_after_trans_y)
    all_adjDiff_after_z  = np.diff( all_after_trans_z)
    
    had_adjDiff_before_x = np.diff( had_before_trans_x)
    had_adjDiff_before_y = np.diff( had_before_trans_y)
    had_adjDiff_before_z = np.diff( had_before_trans_z)
    had_adjDiff_after_x  = np.diff( had_after_trans_x)
    had_adjDiff_after_y  = np.diff( had_after_trans_y)
    had_adjDiff_after_z  = np.diff( had_after_trans_z)

    # calculate the distance between adjacent energy deposits before and after the transformation
    all_adjDiff_before = np.sqrt( all_adjDiff_before_x**2 + all_adjDiff_before_y**2 + all_adjDiff_before_z**2)
    all_adjDiff_after  = np.sqrt( all_adjDiff_after_x**2 + all_adjDiff_after_y**2 + all_adjDiff_after_z**2)
    
    had_adjDiff_before = np.sqrt( had_adjDiff_before_x**2 + had_adjDiff_before_y**2 + had_adjDiff_before_z**2)
    had_adjDiff_after  = np.sqrt( had_adjDiff_after_x**2 + had_adjDiff_after_y**2 + had_adjDiff_after_z**2)

    # calculate the difference of distances between adjacent energy deposits before and after the transformation
    all_distdep_diff = all_adjDiff_after - all_adjDiff_before
    had_distdep_diff = had_adjDiff_after - had_adjDiff_before
    
    # fill TH1D objects with the array data above
    # if statements make sure that histos are not double-filled 
    # since this function, quality_check_graphs(), can be called
    # for multiple vertex positions (e.g. LArBath, ndPlusCurve, FD, etc.)
    # for a given event
    if root_hists[0].GetEntries() < n_allEdeps - 1:
        print( root_hists[0].GetName() + " BEING FILLED FOR EVENT {0}!!!".format( jentry))
        for iDiff in range( n_allEdeps - 1): # number of adjacent differences is n_allEdeps - 1
            root_hists[0].Fill( all_distdep_diff[ iDiff])
    if root_hists[1].GetEntries() < n_hadEdeps - 1:
        print( root_hists[1].GetName() + " BEING FILLED FOR EVENT {0}!!!".format( jentry))
        for jDiff in range( n_hadEdeps - 1): # number of adjacent differences is n_hadEdeps - 1
            root_hists[1].Fill( had_distdep_diff[ jDiff])


    all_distdep_diff_abs_max = np.max( np.abs( all_distdep_diff))
    had_distdep_diff_abs_max = np.max( np.abs( had_distdep_diff))
    all_distdep_diff_norm_abs_max = np.max( np.abs( all_distdep_diff / all_distdep_mid_before))
    had_distdep_diff_norm_abs_max = np.max( np.abs( had_distdep_diff / had_distdep_mid_before))
    plt_lim_scale = 1.1 

    all_fig = plt.figure( figsize=(10, 6))
    all_gs = all_fig.add_gridspec( 2, hspace=0.15)
    all_axs = all_gs.subplots(sharex=True)
    for ax in all_axs: 
        ax.label_outer() # only label bottom plot's x-axis
        ax.grid()
        ax.set_axisbelow( True) # put the grid below the data
    all_axs[0].scatter( all_distdep_mid_before, all_distdep_diff)
    all_axs[0].set_ylabel('difference [cm]')
    all_axs[0].set_ylim( -all_distdep_diff_abs_max * plt_lim_scale, all_distdep_diff_abs_max * plt_lim_scale)
    all_axs[1].scatter( all_distdep_mid_before, all_distdep_diff / all_distdep_mid_before)
    all_axs[1].set_ylabel('difference over total \n distance to POI')
    all_axs[1].set_ylim( -all_distdep_diff_norm_abs_max * plt_lim_scale, all_distdep_diff_norm_abs_max * plt_lim_scale)
    all_axs[1].set_xlabel('Adj. EDep Midpoint Dist. to POI before Transform [cm]')
    all_fig.suptitle('Difference in Distance Between EDeps ('+title_delineator+') \n v. Adjacent EDep Midpoint\'s Distance to POI before Transform', fontsize=15)
    plt.savefig( plotpath + "/all_"+save_file_delineator+"_adj_distdep_diff_evt{0}.png".format( jentry))
    plt.clf() # clear figure
    plt.close()

    all_3dFig = plt.figure(figsize=(14, 12))
    all_3dAx = all_3dFig.add_subplot(projection='3d')
    all_3d_p = all_3dAx.scatter( all_zmid_before, all_xmid_before, all_ymid_before, s=20, c=all_distdep_diff, vmin=-all_distdep_diff_abs_max, vmax=all_distdep_diff_abs_max)
    all_3d_cbar = plt.colorbar( all_3d_p)
    all_3d_cbar.ax.set_ylabel('difference [cm]', fontsize=20)
    all_3dAx.set_xlabel('z-midpoint [cm]')
    all_3dAx.set_ylabel('x-midpoint [cm]')
    all_3dAx.set_zlabel('y-midpoint [cm]')
    all_3dFig.suptitle('Difference in Distance Between EDeps ('+title_delineator+') \n v. Adjacent EDep Midpoint (Relative to POI) before Transform', fontsize=20)
    all_3dAx.set_xlim( [-dimension/2, dimension/2])
    all_3dAx.set_ylim( [-dimension/2, dimension/2])
    all_3dAx.set_zlim( [-dimension/2, dimension/2])
    all_3dFig.savefig( plotpath + "/all_"+save_file_delineator+"_adj_distdep_diff_evt{0}_xyz.png".format( jentry))
    plt.clf() # clear figure
    plt.close()



    had_fig = plt.figure( figsize=(10, 6))
    had_gs = had_fig.add_gridspec( 2, hspace=0.15)
    had_axs = had_gs.subplots(sharex=True)
    for ax in had_axs: 
        ax.label_outer() # only label bottom plot's x-axis
        ax.grid()
        ax.set_axisbelow( True) # put the grid below the data
    had_axs[0].scatter( had_distdep_mid_before, had_distdep_diff)
    had_axs[0].set_ylabel('difference [cm]')
    had_axs[0].set_ylim( -had_distdep_diff_abs_max * plt_lim_scale, had_distdep_diff_abs_max * plt_lim_scale)
    had_axs[1].scatter( had_distdep_mid_before, had_distdep_diff / had_distdep_mid_before)
    had_axs[1].set_ylabel('difference over total \n distance to POI')
    had_axs[1].set_ylim( -had_distdep_diff_norm_abs_max * plt_lim_scale, had_distdep_diff_norm_abs_max * plt_lim_scale)
    had_axs[1].set_xlabel('Adj. EDep Midpoint Dist. to POI before Transform [cm]')
    had_fig.suptitle('Difference in Distance Between EDeps ('+title_delineator+') \n v. Adjacent EDep Midpoint\'s Distance to POI before Transform', fontsize=15)
    plt.savefig( plotpath + "/had_"+save_file_delineator+"_adj_distdep_diff_evt{0}.png".format( jentry))
    plt.clf() # clear figure
    plt.close()

    had_3dFig = plt.figure(figsize=(14, 12))
    had_3dAx = had_3dFig.add_subplot(projection='3d')
    had_3d_p = had_3dAx.scatter( had_zmid_before, had_xmid_before, had_ymid_before, s=20, c=had_distdep_diff, vmin=-had_distdep_diff_abs_max, vmax=had_distdep_diff_abs_max)
    had_3d_cbar = plt.colorbar( had_3d_p)
    had_3d_cbar.ax.set_ylabel('difference [cm]', fontsize=20)
    had_3dAx.set_xlabel('z-midpoint [cm]')
    had_3dAx.set_ylabel('x-midpoint [cm]')
    had_3dAx.set_zlabel('y-midpoint [cm]')
    had_3dFig.suptitle('Difference in Distance Between EDeps ('+title_delineator+') \n v. Adjacent EDep Midpoint (Relative to POI) before Transform', fontsize=20)
    had_3dAx.set_xlim( [-dimension/2, dimension/2])
    had_3dAx.set_ylim( [-dimension/2, dimension/2])
    had_3dAx.set_zlim( [-dimension/2, dimension/2])
    had_3dFig.savefig( plotpath + "/had_"+save_file_delineator+"_adj_distdep_diff_evt{0}_xyz.png".format( jentry))
    plt.clf() # clear figure
    plt.close()
# end of quality_check_graphs()




# contine main block
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
a = str(sys.argv[1])
#print( a)
#print( sys.argv[1])

# this works interactively
#edep_file = TFile("edep.*.root")
edep_file = TFile( a, "OPEN")
if not edep_file:
  print ("Error: could not open file", a)
  sys.exit()
inputTree = edep_file.Get("EDepSimEvents")
event = TG4Event()
inputTree.SetBranchAddress("Event", event)
entries = inputTree.GetEntriesFast()
#print( entries)

trajectories_dtype = np.dtype([ ("pdgID", "i4"), ("trackID", "u4")]) #i4 - integer 4, u4 - unsigned integer(sort of abs value)

# create directory for plots to be stored if it doesn't already exist
user = os.getlogin()
out_path = "/pnfs/dune/scratch/users/" + user + "/eDepSim_out"
plotpath = out_path + "/plots"
rootpath = out_path + "/root_out" # for output ROOT files
if not os.path.exists( out_path):
    os.makedirs( out_path)
    print("out_path '" + out_path + "' did not exist. It has been created!")
if not os.path.exists( plotpath):
    os.makedirs( plotpath)
    print("plotpath '" + plotpath + "' did not exist. It has been created!")
if not os.path.exists( rootpath):
    os.makedirs( rootpath)
    print("rootpath '" + rootpath + "' did not exist. It has been created!")
print(" \n") # separate output

# create the ROOT file that will hold the output of this script
# including but not limited to the output TH1D objects and raw data
f_out = gROOT.FindObject('{0}/n2fd_paired_out.root'.format( rootpath))
if f_out:
   f_out.Close()
f_out = TFile('{0}/n2fd_paired_out.root'.format( rootpath), 'RECREATE', 'Near-Far Paired Event Outputs' )
myEvents = TTree('myEvents', 'myEvents')

# Wei Shi:
# Perhaps you could declare the vector<double> objects here
# and create the corresponding branches. Alex and I thought
# to do something like the following for each of the
# transformations we apply:
#
# data_all_larbath_x = ROOT.vector("double")()
# data_all_larbath_y = ROOT.vector("double")()
# data_all_larbath_z = ROOT.vector("double")()
# data_had_larbath_x = ROOT.vector("double")()
# data_had_larbath_y = ROOT.vector("double")()
# data_had_larbath_z = ROOT.vector("double")()
# data_larbath_vtx_x = array('d', [0])
# data_larbath_vtx_y = array('d', [0])
# data_larbath_vtx_z = array('d', [0])
#
# myEvents.Branch("all_larbath_x", data_all_larbath_x)
# ...

gDecayZ = TGraph(27, OffAxisPoints, meanPDPZ)

# histograms for the raw data
hAll_posdep_x = TH1D('hAll_posdep_x', 'Energy Deposit Positions in X', dimension*10, -dimension/2, dimension/2) # cm
hAll_posdep_y = TH1D('hAll_posdep_y', 'Energy Deposit Positions in Y', dimension*10, -dimension/2, dimension/2) # cm
hAll_posdep_z = TH1D('hAll_posdep_z', 'Energy Deposit Positions in Z', dimension*10, -dimension/2, dimension/2) # cm
hAll_edep     = TH1D('hAll_edep', 'Energy Deposits', 5000, 0, 5)                                                # MeV

hHad_posdep_x = TH1D('hHad_posdep_x', 'Hadronic Energy Deposit Positions in X', dimension*10, -dimension/2, dimension/2) # cm
hHad_posdep_y = TH1D('hHad_posdep_y', 'Hadronic Energy Deposit Positions in Y', dimension*10, -dimension/2, dimension/2) # cm
hHad_posdep_z = TH1D('hHad_posdep_z', 'Hadronic Energy Deposit Positions in Z', dimension*10, -dimension/2, dimension/2) # cm
hHad_edep     = TH1D('hHad_edep', 'Hadronic Energy Deposits', 5000, 0, 5)                                                # MeV


# difference (diff) histograms are declared below
# all histograms are in units of cm
# the naming scheme is as follows:
# first transformation name is before
# second transformation name is after
#
# hAll: all Edeps, hHad: hadronic Edeps
#
# specifications of what transformations have been
# applied are:
# 
# larbath: original, large volume of liquid argon
# ndPlusCurve: this is equivalent to being placed
#              at the FD origin
# ndPlusCurveNDEvtVtx: ndPlusCurve but
#              translated back to the event vertex
#              position before applying the rotation
#              for Earth's curvature; note Earth's
#              curvature has been applied here
# nd: randomly thrown event in the ND
# fd: randomly thrown event in the FD
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
# set of histograms for after ndPlusCurve transformation has been applied
hAll_larbath_ndPlusCurve_distdep_diff = TH1D('hAll_larbath_ndPlusCurve_distdep_diff', 'hAll_larbath_ndPlusCurve_distdep_diff', \
                                               dimension*20*3, -3*dimension, 3*dimension) 
hHad_larbath_ndPlusCurve_distdep_diff = TH1D('hHad_larbath_ndPlusCurve_distdep_diff', 'hHad_larbath_ndPlusCurve_distdep_diff', \
                                               dimension*20*3, -3*dimension, 3*dimension) 
larbath_ndPlusCurve_hists = [ hAll_larbath_ndPlusCurve_distdep_diff, 
                                hHad_larbath_ndPlusCurve_distdep_diff]

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
# set of histograms for after ndPlusCurveNDEvtVtx transformation has been applied
# differences taken between ndPlusCurveNDEvtVtx and all previous transformations
hAll_ndPlusCurve_ndPlusCurveNDEvtVtx_distdep_diff = TH1D('hAll_ndPlusCurve_ndPlusCurveNDEvtVtx_distdep_diff', 'hAll_ndPlusCurve_ndPlusCurveNDEvtVtx_distdep_diff', \
                                                                dimension*20*3, -3*dimension, 3*dimension) 
hHad_ndPlusCurve_ndPlusCurveNDEvtVtx_distdep_diff = TH1D('hHad_ndPlusCurve_ndPlusCurveNDEvtVtx_distdep_diff', 'hHad_ndPlusCurve_ndPlusCurveNDEvtVtx_distdep_diff', \
                                                                dimension*20*3, -3*dimension, 3*dimension) 
ndPlusCurve_ndPlusCurveNDEvtVtx_hists = [ hAll_ndPlusCurve_ndPlusCurveNDEvtVtx_distdep_diff, 
                                                hHad_ndPlusCurve_ndPlusCurveNDEvtVtx_distdep_diff]

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
# set of histograms for after the final ND and FD randomly thrown transformations have been applied
# differences taken between nd/fd and all previous transformations
hAll_larbath_nd_distdep_diff = TH1D('hAll_larbath_nd_distdep_diff', 'hAll_larbath_nd_distdep_diff', dimension*20*3, -3*dimension, 3*dimension)  
hHad_larbath_nd_distdep_diff = TH1D('hHad_larbath_nd_distdep_diff', 'hHad_larbath_nd_distdep_diff', dimension*20*3, -3*dimension, 3*dimension)  
larbath_nd_hists = [ hAll_larbath_nd_distdep_diff, 
                        hHad_larbath_nd_distdep_diff]

hAll_larbath_fd_distdep_diff = TH1D('hAll_larbath_fd_distdep_diff', 'hAll_larbath_fd_distdep_diff', dimension*20*3, -3*dimension, 3*dimension)  
hHad_larbath_fd_distdep_diff = TH1D('hHad_larbath_fd_distdep_diff', 'hHad_larbath_fd_distdep_diff', dimension*20*3, -3*dimension, 3*dimension)   
larbath_fd_hists = [ hAll_larbath_fd_distdep_diff, 
                        hHad_larbath_fd_distdep_diff]


entries = 2 # hardcode entries to test code
if entries != inputTree.GetEntriesFast(): 
    print("'entries' hardcoded to be: ", entries)


loop_time = time.time()
# Loop over events
for jentry in range( entries):
    print("jentry = " + str( jentry))

    nb = inputTree.GetEntry( jentry) #number of bytes read
    #print("nb =" + str(nb))
    #print("event number: ", event.EventId)
    #print("number of primaries: ", event.Primaries.size())
    #print("number of trajectories: ", event.Trajectories.size())
    #print("number of segments: ", event.SegmentDetectors.size())
    all_posdep_list = list()
    had_posdep_list = list()
    all_edep_list   = list()
    had_edep_list   = list()

    larbath_vtx = (None, None, None) # initialize LArBath vertex

    for primary in event.Primaries:
        #print("number of particles: ", primary.Particles.size())
        for ipart, particle in enumerate( primary.Particles):
            #print("ipart here = " +str(ipart))
            PDGCode = particle.GetPDGCode()
            #print("pdgcode: ", PDGCode)
            if abs( PDGCode) >= 11 and abs( PDGCode) <= 16:
                posx = primary.GetPosition().X() * edep2cm
                posy = primary.GetPosition().Y() * edep2cm
                posz = primary.GetPosition().Z() * edep2cm
                momx = particle.GetMomentum().X()
                momy = particle.GetMomentum().Y()
                momz = particle.GetMomentum().Z()
                trackIDforPrimary = particle.GetTrackId()
                #print("pos x: ", posx, " y: ", posy, " z: ", posz, " [cm]")
                #print("mom x: ", momx, " y: ", momy, " z: ", momz, " [MeV]")
                #print("trackIDforPrimary: ", trackIDforPrimary)

                larbath_vtx = ( posx, posy, posz)

    trajectories = np.empty( len( event.Trajectories), dtype=trajectories_dtype)
    for iTraj, trajectory in enumerate( event.Trajectories):
        start_pt, end_pt = trajectory.Points[0], trajectory.Points[-1]
        trajectories[iTraj]["pdgID"] = trajectory.GetPDGCode()
        trajectories[iTraj]["trackID"] = trajectory.GetTrackId()

    for containerName, hitSegments in event.SegmentDetectors:
        # iHit is the index, hitSEgment is the data stored at the index in the second item in event.SegementDectectors
        for iHit, hitSegment in enumerate( hitSegments):
            primaryID = hitSegment.GetPrimaryId()
            # Energy deposit from primary particles
            edep_x = hitSegment.GetStop().X() * edep2cm
            edep_y = hitSegment.GetStop().Y() * edep2cm
            edep_z = hitSegment.GetStop().Z() * edep2cm
            edep = hitSegment.GetEnergyDeposit()

            # Later need these hadronic edeps to evaluate hadronic veto
            all_posdep_list.append( edep_x)
            all_posdep_list.append( edep_y)
            all_posdep_list.append( edep_z)
            all_edep_list.append(   edep)
            
            hAll_posdep_x.Fill(     edep_x)
            hAll_posdep_y.Fill(     edep_y)
            hAll_posdep_z.Fill(     edep_z)
            hAll_edep.Fill(         edep)

            # Hadronic part of edepsim
            # Later need these hadronic edeps to evaluate hadronic veto
            if primaryID != trackIDforPrimary:
                had_posdep_list.append( edep_x)
                had_posdep_list.append( edep_y)
                had_posdep_list.append( edep_z)
                had_edep_list.append(   edep)

                hHad_posdep_x.Fill(     edep_x)
                hHad_posdep_y.Fill(     edep_y)
                hHad_posdep_z.Fill(     edep_z) 
                hHad_edep.Fill(         edep)

    # for use in processing events before and after transformations
    nEdeps_all = len( all_edep_list)
    nEdeps_had = len( had_edep_list)
    nEdeps = ( nEdeps_all, nEdeps_had)


    # unpack the LArBath coordinates (relative to LArBath origin)
    all_larbath_coords = ( np.array( all_posdep_list[::3]), 
                             np.array( all_posdep_list[1::3]), 
                             np.array( all_posdep_list[2::3]))
    had_larbath_coords = ( np.array( had_posdep_list[::3]), 
                             np.array( had_posdep_list[1::3]), 
                             np.array( had_posdep_list[2::3]))

    # use LArBath coordinates to define distance to LArBath event vertex
    all_larbath_dist2larbathEvtVtx = tuple( map( lambda p, q: p - q, all_larbath_coords, larbath_vtx))
    had_larbath_dist2larbathEvtVtx = tuple( map( lambda p, q: p - q, had_larbath_coords, larbath_vtx))
   
    # plot the energy deposit data
    all_edep_list_abs_max = np.max( np.abs( all_edep_list)) # used in setting plot bounds

    fig_xy = plt.figure( figsize=(10, 6))
    plt.scatter( all_larbath_coords[0], all_larbath_coords[1], s=5, c=all_edep_list, vmin=-all_edep_list_abs_max, vmax=all_edep_list_abs_max)
    cbar_xy = plt.colorbar()
    cbar_xy.ax.set_ylabel('Energy Deposited [MeV]')
    plt.xlabel('x [cm]')
    plt.ylabel('y [cm]')
    plt.title('LArBath EDeps in XY-plane (Evt {0})'.format( jentry), fontsize=15)
    plt.xlim( [-dimension/2, dimension/2])
    plt.ylim( [-dimension/2, dimension/2])
    plt.xticks( rotation=30)
    plt.savefig( plotpath + "/larbath_evt{0}_xy.png".format( jentry))
    plt.clf() # clear figure
    plt.close()

    fig_xz = plt.figure( figsize=(10, 6))
    plt.scatter( all_larbath_coords[2], all_larbath_coords[0], s=5, c=all_edep_list, vmin=-all_edep_list_abs_max, vmax=all_edep_list_abs_max)
    cbar_xz = plt.colorbar()
    cbar_xz.ax.set_ylabel('Energy Deposited [MeV]')
    plt.xlabel('z [cm]')
    plt.ylabel('x [cm]')
    plt.title('LArBath EDeps in ZX-plane (Evt {0})'.format( jentry), fontsize=15)
    plt.xlim( [-dimension/2, dimension/2])
    plt.ylim( [-dimension/2, dimension/2])
    plt.xticks( rotation=30)
    plt.savefig( plotpath + "/larbath_evt{0}_zx.png".format( jentry))
    plt.clf()
    plt.close()

    fig_yz = plt.figure( figsize=(10, 6))
    plt.scatter(all_larbath_coords[2], all_larbath_coords[1], s=5, c=all_edep_list, vmin=-all_edep_list_abs_max, vmax=all_edep_list_abs_max)
    cbar_yz = plt.colorbar()
    cbar_yz.ax.set_ylabel('Energy Deposited [MeV]')
    plt.xlabel('z [cm]')
    plt.ylabel('y [cm]')
    plt.title('LArBath EDeps in ZY-plane (Evt {0})'.format( jentry), fontsize=15)
    plt.xlim( [-dimension/2, dimension/2])
    plt.ylim( [-dimension/2, dimension/2])
    plt.xticks( rotation=30)
    plt.savefig( plotpath + "/larbath_evt{0}_zy.png".format( jentry))
    plt.clf()
    plt.close()

    fig = plt.figure(figsize=(14, 12))
    ax = fig.add_subplot(projection='3d')
    p = ax.scatter( all_larbath_coords[2], all_larbath_coords[0], all_larbath_coords[1], s=20, c=all_edep_list, vmin=-all_edep_list_abs_max, vmax=all_edep_list_abs_max)
    cbar = fig.colorbar( p)
    cbar.set_label('Energy Deposited [MeV]', fontsize=15)
    ax.set_xlabel('z [cm]')
    ax.set_ylabel('x [cm]')
    ax.set_zlabel('y [cm]')
    ax.set_title('LArBath EDeps (Evt {0})'.format( jentry), fontsize=20)
    ax.set_xlim( [-dimension/2, dimension/2])
    ax.set_ylim( [-dimension/2, dimension/2])
    ax.set_zlim( [-dimension/2, dimension/2])
    fig.savefig( plotpath + "/larbath_evt{0}_xyz.png".format( jentry))
    plt.clf()
    plt.close()

    # Initialize geometric efficiency module to manipulate energy deps
    # Only consider ND LAr on axis
    geoEff = pyGeoEff.geoEff( 123, False)

    # Use neutrino decay position, rather than fixed neutrino direction as symmetry axis
    geoEff.setUseFixedBeamDir(False)

    # 30 cm veto region for hadronic veto
    geoEff.setVetoSizes([30])

    # 30 MeV E threshold for hadronic veto
    geoEff.setVetoEnergyThresholds([30])

    # Near detector active dimensions for hadronic veto
    geoEff.setActiveX( NDActiveVol_min[0], NDActiveVol_max[0])
    geoEff.setActiveY( NDActiveVol_min[1], NDActiveVol_max[1])
    geoEff.setActiveZ( NDActiveVol_min[2], NDActiveVol_max[2])

    # Far detector active dimensions for hadronic veto
    geoEff.setFDActiveX( FDActiveVol_min[0], FDActiveVol_max[0])
    geoEff.setFDActiveY( FDActiveVol_min[1], FDActiveVol_max[1])
    geoEff.setFDActiveZ( FDActiveVol_min[2], FDActiveVol_max[2])

    # Range for random translation throws in ND fiducial volume
    geoEff.setRangeX( ND_FV_min[0], ND_FV_max[0])
    geoEff.setRangeY( ND_FV_min[1], ND_FV_max[1])
    geoEff.setRangeZ( ND_FV_min[2], ND_FV_max[2])

    # Range for random translation throws in FD fiducial volume
    geoEff.setRangeXFD( FD_FV_min[0], FD_FV_min[0])
    geoEff.setRangeYFD( FD_FV_min[1], FD_FV_min[1])
    geoEff.setRangeZFD( FD_FV_min[2], FD_FV_min[2])

    # Set offset between ND MC coordinate system and volumes defined above.
    # Is this still needed for Alex's Genie Gen??? To be validated/discussed
    geoEff.setOffsetX( NDLAr_OnAxis_offset[0])
    geoEff.setOffsetY( NDLAr_OnAxis_offset[1])
    geoEff.setOffsetZ( NDLAr_OnAxis_offset[2])

    # Original Genie Gen event vtx, not guaranteed to be at (0,0,0)!
    # Later we need the vertex to calculate the rotation axis when event is randomly put at a new position in ND
    geoEff.setVertex( posx, posy, posz) # cm

    # Interpolate event neutrino production point (beam coordinate)
    # Input needs to be in unit of meters
    decayZbeamCoord = gDecayZ.Eval( posx/100. - NDLAr_OnAxis_offset[0]/100. - detRefBeamCoord[0] )

    # Calculate neutrino production point in detector coordinate
    decayXdetCoord = beamRefDetCoord[0] - detRefBeamCoord[0]
    decayYdetCoord = beamRefDetCoord[1] - detRefBeamCoord[1] * cos( beamLineRotation) + ( decayZbeamCoord - detRefBeamCoord[2] ) * sin( beamLineRotation)
    decayZdetCoord = beamRefDetCoord[2] + detRefBeamCoord[1] * sin( beamLineRotation) + ( decayZbeamCoord - detRefBeamCoord[2] ) * cos( beamLineRotation)

    # Set production point in unit: cm
    # Later we need this decay position to calculate the rotation axis when event is randomly put at a new position in ND
    geoEff.setDecayPos( decayXdetCoord*100., decayYdetCoord*100., decayZdetCoord*100.)

    # Randomly throw the Genie Gen event in ND
    tot_nd_throw = 0

    # If after max_throws still don't pass at nd, stop and move to next event (otherwise too much computing resources)
    while tot_nd_throw < max_throws:
        print ("-- tot nd throw:", tot_nd_throw)
        
        # Only do one throw in ND
        geoEff.setNthrows( 1)
        geoEff.throwTransforms() # this randomly generates new vtx position and a rotation angle w.r.t. the neutrino direction
        
        # Get the randomly generated vtx x, y, z, and the angle
        throwVtxX_nd  = geoEff.getCurrentThrowTranslationsX() # cm
        throwVtxY_nd  = geoEff.getCurrentThrowTranslationsY() # cm
        throwVtxZ_nd  = geoEff.getCurrentThrowTranslationsZ() # cm
        throwAngle = geoEff.getCurrentThrowRotations()

        # Require random thrown vtx pos outside in ND dead regions
        if ( IsInNDFV( throwVtxX_nd[0] - NDLAr_OnAxis_offset[0], throwVtxY_nd[0] - NDLAr_OnAxis_offset[1], throwVtxZ_nd[0] - NDLAr_OnAxis_offset[2]) ):
            print ("-- nd throw", tot_nd_throw, "is in nd FV")
        
            # Interpolate neutrino production point (beam coordinate) for the random throw, unit meter
            RandomthrowdecayZbeamCoord = gDecayZ.Eval( throwVtxX_nd[0]/100. - NDLAr_OnAxis_offset[0]/100. - detRefBeamCoord[0] )

            # Calculate neutrino production point in detector coordinate, unit meter
            RandomthrowdecayXdetCoord = beamRefDetCoord[0] - detRefBeamCoord[0]
            RandomthrowdecayYdetCoord = beamRefDetCoord[1] - detRefBeamCoord[1]*cos(beamLineRotation) + ( RandomthrowdecayZbeamCoord - detRefBeamCoord[2] ) * sin( beamLineRotation)
            RandomthrowdecayZdetCoord = beamRefDetCoord[2] + detRefBeamCoord[1]*sin(beamLineRotation) + ( RandomthrowdecayZbeamCoord - detRefBeamCoord[2] ) * cos( beamLineRotation)

            # We have a new decay position because the above random throw changes x in ND
            # Need this to calculate the rotation axis for the random throw
            geoEff.setDecayPos4RandomThrowX(RandomthrowdecayXdetCoord*100., RandomthrowdecayYdetCoord*100., RandomthrowdecayZdetCoord*100.)
        
            # Set the hadronic part list of edeps to evaluate ND hadronic veto
            # NOTE: in the future may need to add lepton selection at ND as well (to be discussed)
            geoEff.setHitSegEdeps( had_edep_list)
            geoEff.setHitSegPoss(  had_posdep_list)
            ndrandthrowresulthad = geoEff.getNDContainment4RandomThrowX() # returns a struct

            if (ndrandthrowresulthad.containresult[0][0][0] != 0):
                print ("-- nd throw", tot_nd_throw, "passed nd had veto")
                # Random throw passed ND hadronic veto

                # Now change to the full list of edeps (leptonic + hadronic)
                # the random thrown x/y/z/angle should reamin the same because throw is done above already
                geoEff.setHitSegEdeps( all_edep_list)
                geoEff.setHitSegPoss(  all_posdep_list)
             
                # And call the function again to get new transformed positions for all edeps
                ndrandthrowresultall = geoEff.getNDContainment4RandomThrowX()
            
                # Tell the module where is the random thrown vertex
                geoEff.setNDrandVertex( throwVtxX_nd[0], throwVtxY_nd[0], throwVtxZ_nd[0])

                # Now translate this event vertex to ND det coordinate (0,0,0) (and the edeps accordingly)
                # Do it for all edeps and also had part edeps (because later we need had part only to evaluate FD had veto)
                all_posdep_ndorig_matrix = geoEff.move2ndorigin( ndrandthrowresultall.thrownEdepspos[0]) # returns Eigen::Matrix3Xf
                had_posdep_ndorig_matrix = geoEff.move2ndorigin( ndrandthrowresulthad.thrownEdepspos[0])

                # Apply earth curvature correction to translate into FD coordinate system, vtx now at (0,0,0) in FD det coordinate
                had_posdep_fdorig_matrix = geoEff.getn2fEarthCurvatureCorr( had_posdep_ndorig_matrix, beamLineRotation) #returns Eigen::Matrix3Xf
                all_posdep_fdorig_matrix = geoEff.getn2fEarthCurvatureCorr( all_posdep_ndorig_matrix, beamLineRotation)

                # unpack the energy deposit coordinates relative to the FD origin
                # NOTE: ndPlusCurve is before the event is randomly thrown in the FD
                all_ndPlusCurve_coords = ( np.array( all_posdep_fdorig_matrix[0,:]),
                                             np.array( all_posdep_fdorig_matrix[1,:]),
                                             np.array( all_posdep_fdorig_matrix[2,:]))
                had_ndPlusCurve_coords = ( np.array( had_posdep_fdorig_matrix[0,:]),
                                             np.array( had_posdep_fdorig_matrix[1,:]),
                                             np.array( had_posdep_fdorig_matrix[2,:]))
                
                # get the event vertex position after all transformations including
                # the Earth's curvature; represented as a rotation between ND and FD
                throwVtxX_ndPlusCurve = geoEff.getCurrentThrowTranslationsX()
                throwVtxY_ndPlusCurve = geoEff.getCurrentThrowTranslationsY()
                throwVtxZ_ndPlusCurve = geoEff.getCurrentThrowTranslationsZ()
               
                throwVtx_ndPlusCurve = ( throwVtxX_ndPlusCurve[0], throwVtxY_ndPlusCurve[0], throwVtxZ_ndPlusCurve[0])
 
                # calculate the distance each energy deposit is relative to the event vertices after various transformations
                all_ndPlusCurve_dist2larbathEvtVtx     = tuple( map( lambda p, q: p - q, all_ndPlusCurve_coords, larbath_vtx))
                had_ndPlusCurve_dist2larbathEvtVtx     = tuple( map( lambda p, q: p - q, had_ndPlusCurve_coords, larbath_vtx))
                all_ndPlusCurve_dist2ndPlusCurveEvtVtx = tuple( map( lambda p, q: p - q, all_ndPlusCurve_coords, throwVtx_ndPlusCurve))
                had_ndPlusCurve_dist2ndPlusCurveEvtVtx = tuple( map( lambda p, q: p - q, had_ndPlusCurve_coords, throwVtx_ndPlusCurve))

                all_larbath_dist2ndPlusCurveEvtVtx = tuple( map( lambda p, q: p - q, all_larbath_coords, (throwVtxX_ndPlusCurve[0], throwVtxY_ndPlusCurve[0], throwVtxZ_ndPlusCurve[0])))
                had_larbath_dist2ndPlusCurveEvtVtx = tuple( map( lambda p, q: p - q, had_larbath_coords, (throwVtxX_ndPlusCurve[0], throwVtxY_ndPlusCurve[0], throwVtxZ_ndPlusCurve[0])))
                
      
                quality_check_graphs( nEdeps, all_larbath_dist2larbathEvtVtx, all_ndPlusCurve_dist2larbathEvtVtx, had_larbath_dist2larbathEvtVtx, had_ndPlusCurve_dist2larbathEvtVtx, \
                                        larbath_ndPlusCurve_hists, '[ND + Curve] - LArBath, POI = LArBath Vertex', 'larbath_ndPlusCurve_larbathEvtVtx')
                quality_check_graphs( nEdeps, all_larbath_dist2ndPlusCurveEvtVtx, all_ndPlusCurve_dist2ndPlusCurveEvtVtx, \
                                            had_larbath_dist2ndPlusCurveEvtVtx, had_ndPlusCurve_dist2ndPlusCurveEvtVtx, \
                                        larbath_ndPlusCurve_hists, '[ND + Curve] - LArBath, POI = [ND + Curve] Vertex', 'larbath_ndPlusCurve_ndPlusCurveEvtVtx')

                # Translate the event vertex back to the ramdonly thrown ND position.
                # NOTE: this data will be used by Alex Wilkinson for his ML study.
                all_posdep_ndPlusCurveNDEvtVtx = geoEff.moveBack2ndVertex( all_posdep_fdorig_matrix) # returns Eigen::Matrix3Xf
                had_posdep_ndPlusCurveNDEvtVtx = geoEff.moveBack2ndVertex( had_posdep_fdorig_matrix)

                # unpack the energy deposit coordinates after they have been translated from the FD origin back to the
                # ND vertex position before applying the rotation due to Earth's curvature 
                all_ndPlusCurveNDEvtVtx_coords = ( np.array( all_posdep_ndPlusCurveNDEvtVtx[0,:]),
                                                         np.array( all_posdep_ndPlusCurveNDEvtVtx[1,:]),
                                                         np.array( all_posdep_ndPlusCurveNDEvtVtx[2,:]))
                had_ndPlusCurveNDEvtVtx_coords = ( np.array( had_posdep_ndPlusCurveNDEvtVtx[0,:]),
                                                         np.array( had_posdep_ndPlusCurveNDEvtVtx[1,:]),
                                                         np.array( had_posdep_ndPlusCurveNDEvtVtx[2,:]))
                
                # get the event vertex position after all transformations including
                # the translation back to the ND vertex position before applying the
                # rotation due to Earth's curvature.
                throwVtxX_ndPlusCurveNDEvtVtx = geoEff.getCurrentThrowTranslationsX() 
                throwVtxY_ndPlusCurveNDEvtVtx = geoEff.getCurrentThrowTranslationsY() 
                throwVtxZ_ndPlusCurveNDEvtVtx = geoEff.getCurrentThrowTranslationsZ()
 
                throwVtx_ndPlusCurveNDEvtVtx = ( throwVtxX_ndPlusCurveNDEvtVtx[0], throwVtxY_ndPlusCurveNDEvtVtx[0], throwVtxZ_ndPlusCurveNDEvtVtx[0])

                # calculate the distance each energy deposit is relative to the event vertices after various transformations
                all_ndPlusCurveNDEvtVtx_dist2larbathEvtVtx     = tuple( map( 
                        lambda p, q: p - q, all_ndPlusCurveNDEvtVtx_coords, larbath_vtx))
                had_ndPlusCurveNDEvtVtx_dist2larbathEvtVtx     = tuple( map( 
                        lambda p, q: p - q, had_ndPlusCurveNDEvtVtx_coords, larbath_vtx))
                all_ndPlusCurveNDEvtVtx_dist2ndPlusCurveEvtVtx = tuple( map( 
                        lambda p, q: p - q, all_ndPlusCurveNDEvtVtx_coords, throwVtx_ndPlusCurve))
                had_ndPlusCurveNDEvtVtx_dist2ndPlusCurveEvtVtx = tuple( map( 
                        lambda p, q: p - q, had_ndPlusCurveNDEvtVtx_coords, throwVtx_ndPlusCurve))
                all_ndPlusCurveNDEvtVtx_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                        lambda p, q: p - q, all_ndPlusCurveNDEvtVtx_coords, throwVtx_ndPlusCurveNDEvtVtx))
                had_ndPlusCurveNDEvtVtx_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                        lambda p, q: p - q, had_ndPlusCurveNDEvtVtx_coords, throwVtx_ndPlusCurveNDEvtVtx))

                all_ndPlusCurve_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                        lambda p, q: p - q, all_ndPlusCurve_coords, throwVtx_ndPlusCurveNDEvtVtx))
                had_ndPlusCurve_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                        lambda p, q: p - q, had_ndPlusCurve_coords, throwVtx_ndPlusCurveNDEvtVtx))


                quality_check_graphs( nEdeps, all_ndPlusCurve_dist2larbathEvtVtx, all_ndPlusCurveNDEvtVtx_dist2larbathEvtVtx, \
                                                had_ndPlusCurve_dist2larbathEvtVtx, had_ndPlusCurveNDEvtVtx_dist2larbathEvtVtx, \
                               ndPlusCurve_ndPlusCurveNDEvtVtx_hists, '[ND + Curve + Trans.] - [ND + Curve], POI = LArBath Vertex', 'ndPlusCurveAndTrans_ndPlusCurve_larbathEvtVtx')
                quality_check_graphs( nEdeps, all_ndPlusCurve_dist2ndPlusCurveEvtVtx, all_ndPlusCurveNDEvtVtx_dist2ndPlusCurveEvtVtx, \
                                                had_ndPlusCurve_dist2ndPlusCurveEvtVtx, had_ndPlusCurveNDEvtVtx_dist2ndPlusCurveEvtVtx, \
                               ndPlusCurve_ndPlusCurveNDEvtVtx_hists, '[ND + Curve + Trans.] - [ND + Curve], POI = [ND + Curve] Vertex', 'ndPlusCurveAndTrans_ndPlusCurve_ndPlusCurveEvtVtx')
                quality_check_graphs( nEdeps, all_ndPlusCurve_dist2ndPlusCurveNDEvtVtx, all_ndPlusCurveNDEvtVtx_dist2ndPlusCurveNDEvtVtx, \
                                                had_ndPlusCurve_dist2ndPlusCurveNDEvtVtx, had_ndPlusCurveNDEvtVtx_dist2ndPlusCurveNDEvtVtx, \
                               ndPlusCurve_ndPlusCurveNDEvtVtx_hists, \
                               '[ND + Curve + Trans.] - [ND + Curve], POI = [ND + Curve + Trans.] Vertex', 'ndPlusCurveAndTrans_ndPlusCurve_ndPlusCurveNDEvtVtx')


                # Tell the module where the vertex is in FD
                geoEff.setVertexFD( 0, 0, 0) # it's at 000 because we moved it to ND 000 and then just rotated at 000

                tot_fd_throw = 0

                while tot_fd_throw < max_throws:
                    print ("---- tot fd throw:", tot_fd_throw)
                    # Below do random throw (translate only) in FD similar to ND:
                    geoEff.setNthrowsFD( 1)
                    geoEff.throwTransformsFD() # this randomly generates new vtx position in FD FV

                    # Check if it passes FD hadronic veto
                    geoEff.setHitSegEdeps( had_edep_list) # use the same had edep list
                    fdrandthrowresulthad = geoEff.getFDContainment4RandomThrow( had_posdep_fdorig_matrix)

                    if (fdrandthrowresulthad.containresult[0][0][0] != 0):
                        print ("---- fd throw", tot_fd_throw, "passed fd had veto")
                        # FD rand throw passes veto, write paired evt info

                        # Now change to the full list of edeps (leptonic + hadronic)
                        # the random thrown x/y/z should reamin the same because throw is done above already
                        geoEff.setHitSegEdeps( all_edep_list)
                        fdrandthrowresultall = geoEff.getFDContainment4RandomThrow( all_posdep_fdorig_matrix)

                        print ("Found paired nd-fd random thrown events, saving...")


                        # unpack the energy deposit coordinates for the randomly thrown ND and FDe
                        all_nd_coords = ( np.array( ndrandthrowresultall.thrownEdepspos[0][0,:]), 
                                            np.array( ndrandthrowresultall.thrownEdepspos[0][1,:]), 
                                            np.array( ndrandthrowresultall.thrownEdepspos[0][2,:]))
                        had_nd_coords = ( np.array( ndrandthrowresulthad.thrownEdepspos[0][0,:]), 
                                            np.array( ndrandthrowresulthad.thrownEdepspos[0][1,:]), 
                                            np.array( ndrandthrowresulthad.thrownEdepspos[0][2,:]))
                        
                        all_fd_coords = ( np.array( fdrandthrowresultall.thrownEdepspos[0][0,:]), 
                                            np.array( fdrandthrowresultall.thrownEdepspos[0][1,:]), 
                                            np.array( fdrandthrowresultall.thrownEdepspos[0][2,:]))
                        had_fd_coords = ( np.array( fdrandthrowresulthad.thrownEdepspos[0][0,:]), 
                                            np.array( fdrandthrowresulthad.thrownEdepspos[0][1,:]), 
                                            np.array( fdrandthrowresulthad.thrownEdepspos[0][2,:]))

                       
                        throwVtx_nd = ( throwVtxX_nd[0], throwVtxY_nd[0], throwVtxZ_nd[0])

                        throwVtxX_fd = geoEff.getCurrentFDThrowTranslationsX() 
                        throwVtxY_fd = geoEff.getCurrentFDThrowTranslationsY() 
                        throwVtxZ_fd = geoEff.getCurrentFDThrowTranslationsZ() 

                        throwVtx_fd = ( throwVtxX_fd[0], throwVtxY_fd[0], throwVtxZ_fd[0])

                        # calculate the distance each energy deposit is relative to the event vertices after various transformations
                        all_nd_dist2larbathEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_nd_coords, larbath_vtx))
                        had_nd_dist2larbathEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_nd_coords, larbath_vtx))
                        all_nd_dist2ndPlusCurveEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_nd_coords, throwVtx_ndPlusCurve))
                        had_nd_dist2ndPlusCurveEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_nd_coords, throwVtx_ndPlusCurve))
                        all_nd_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_nd_coords, throwVtx_ndPlusCurveNDEvtVtx))
                        had_nd_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_nd_coords, throwVtx_ndPlusCurveNDEvtVtx))
                        all_nd_dist2ndEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_nd_coords, throwVtx_nd))
                        had_nd_dist2ndEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_nd_coords, throwVtx_nd))

                        all_fd_dist2larbathEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_fd_coords, larbath_vtx))
                        had_fd_dist2larbathEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_fd_coords, larbath_vtx))
                        all_fd_dist2ndPlusCurveEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_fd_coords, throwVtx_ndPlusCurve))
                        had_fd_dist2ndPlusCurveEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_fd_coords, throwVtx_ndPlusCurve))
                        all_fd_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_fd_coords, throwVtx_ndPlusCurveNDEvtVtx))
                        had_fd_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_fd_coords, throwVtx_ndPlusCurveNDEvtVtx))
                        all_fd_dist2ndEvtVtx = tuple( map(
                                lambda p, q: p - q, all_fd_coords, throwVtx_nd))
                        had_fd_dist2ndEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_fd_coords, throwVtx_nd))
                        all_fd_dist2fdEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_fd_coords, throwVtx_fd))
                        had_fd_dist2fdEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_fd_coords, throwVtx_fd))

                        all_larbath_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_larbath_coords, throwVtx_ndPlusCurveNDEvtVtx))
                        had_larbath_dist2ndPlusCurveNDEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_larbath_coords, throwVtx_ndPlusCurveNDEvtVtx))
                        all_larbath_dist2ndEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_larbath_coords, throwVtx_nd))
                        had_larbath_dist2ndEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_larbath_coords, throwVtx_nd))
                        all_larbath_dist2fdEvtVtx = tuple( map( 
                                lambda p, q: p - q, all_larbath_coords, throwVtx_fd))
                        had_larbath_dist2fdEvtVtx = tuple( map( 
                                lambda p, q: p - q, had_larbath_coords, throwVtx_fd))


                        quality_check_graphs( nEdeps , all_larbath_dist2larbathEvtVtx, all_nd_dist2larbathEvtVtx, had_larbath_dist2larbathEvtVtx, had_nd_dist2larbathEvtVtx, \
                                                larbath_nd_hists, 'ND - LArBath, POI = LArBath Vertex', 'larbath_nd_dist2larbathEvtVtx')
                        quality_check_graphs( nEdeps , all_larbath_dist2ndPlusCurveEvtVtx, all_nd_dist2ndPlusCurveEvtVtx, had_larbath_dist2ndPlusCurveEvtVtx, had_nd_dist2ndPlusCurveEvtVtx, \
                                                larbath_nd_hists, 'ND - LArBath, POI = [ND + Curve] Vertex', 'larbath_nd_dist2ndPlusCurveEvtVtx')
                        quality_check_graphs( nEdeps , all_larbath_dist2ndPlusCurveNDEvtVtx, all_nd_dist2ndPlusCurveNDEvtVtx, \
                                                        had_larbath_dist2ndPlusCurveNDEvtVtx, had_nd_dist2ndPlusCurveNDEvtVtx, \
                                                larbath_nd_hists, 'ND - LArBath, POI = [ND + Curve + Trans.] Vertex', 'larbath_nd_dist2ndPlusCurveNDEvtVtx')
                        quality_check_graphs( nEdeps , all_larbath_dist2ndEvtVtx, all_nd_dist2ndEvtVtx, had_larbath_dist2ndEvtVtx, had_nd_dist2ndEvtVtx, \
                                                larbath_nd_hists, 'ND - LArBath, POI = ND Vertex', 'larbath_nd_dist2ndEvtVtx')

                        quality_check_graphs( nEdeps , all_larbath_dist2larbathEvtVtx, all_fd_dist2larbathEvtVtx, had_larbath_dist2larbathEvtVtx, had_fd_dist2larbathEvtVtx, \
                                                larbath_fd_hists, 'ND - LArBath, POI = LArBath Vertex', 'larbath_fd_dist2larbathEvtVtx')
                        quality_check_graphs( nEdeps , all_larbath_dist2ndPlusCurveEvtVtx, all_fd_dist2ndPlusCurveEvtVtx, had_larbath_dist2ndPlusCurveEvtVtx, had_fd_dist2ndPlusCurveEvtVtx, \
                                                larbath_fd_hists, 'ND - LArBath, POI = [ND + Curve] Vertex', 'larbath_fd_dist2ndPlusCurveEvtVtx')
                        quality_check_graphs( nEdeps , all_larbath_dist2ndPlusCurveNDEvtVtx, all_fd_dist2ndPlusCurveNDEvtVtx, \
                                                        had_larbath_dist2ndPlusCurveNDEvtVtx, had_fd_dist2ndPlusCurveNDEvtVtx, \
                                                larbath_fd_hists, 'ND - LArBath, POI = [ND + Curve + Trans.] Vertex', 'larbath_fd_dist2ndPlusCurveNDEvtVtx')
                        quality_check_graphs( nEdeps , all_larbath_dist2ndEvtVtx, all_fd_dist2ndEvtVtx, had_larbath_dist2ndEvtVtx, had_fd_dist2ndEvtVtx, \
                                                larbath_fd_hists, 'ND - LArBath, POI = ND Vertex', 'larbath_fd_dist2ndEvtVtx')
                        quality_check_graphs( nEdeps , all_larbath_dist2fdEvtVtx, all_fd_dist2fdEvtVtx, had_larbath_dist2fdEvtVtx, had_fd_dist2fdEvtVtx, \
                                                larbath_fd_hists, 'ND - LArBath, POI = ND Vertex', 'larbath_fd_dist2fdEvtVtx')




                        # these are the random throwed x y z in ND for all edeps
                        # for print out/debug purposes
                        #geoEff.getCurrentThrowDepsX( 0) # we only have one throw (0th throw) ==> array x[ith edep]
                        #geoEff.getCurrentThrowDepsY( 0)
                        #geoEff.getCurrentThrowDepsZ( 0)

                        # Break the while loop, move on to next evt
                        print ("Paired data saved, breaking fd throw loop")
                        break
                    else:
                        print ("---- fd throw", tot_fd_throw, "failed fd had veto!")

                    # indentation is important!
                    # if don't, put it in another random FD pos...until it passes FD veto
                    tot_fd_throw = tot_fd_throw + 1

                print ("Breaking nd throw loop")
                break

            else:
                print ("-- nd throw", tot_nd_throw, "failed nd had veto!")

        else:
            print ("-- nd throw", tot_nd_throw, "outside nd FV!")

        # indentation is important!
        tot_nd_throw = tot_nd_throw + 1

    # fill event branch with all the data available (which depends on
    # which transformations the data underwent and when the loops were
    # broken
#     
# NOT DONE YET

    # time each loop
    print("Time spent processing this event: ", time.time() - loop_time)
    loop_time = time.time()

# save the output ROOT file
f_out.Write( rootpath + "/n2fd_paired_output.root")

# time the whole program
print("\n")
print("\n")
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n")
print("PROGRAM RUNTIME: ", time.time() - start_time)
