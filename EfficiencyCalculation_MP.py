import uproot4

import numpy as np
from glob import glob

import matplotlib.pyplot as plt

from multiprocessing import Process, Array

offset = [ 0., 5.5, 411. ]

APPLY_FV_CUT = True

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

treeVarsToRead = ['isCC',
                  'nuPDG',
                  'Ev',
                  'LepE',
                  "LepPDG",
                  'LepMomX',
                  'LepMomY', 
                  'LepMomZ',
                  'vtx_x',
                  'vtx_y',
                  'vtx_z',
                  'geoEffThrowResults',
                  'event',
                  'Ehad_veto']

#fNamesCAF = "/Users/cvilela/DUNE/ND_MC/nd_offaxis_v7/CAF/0m/00/FHC.*.CAF.root"
fNamesCAF = "/Users/cvilela/DUNE/ND_MC/nd_offaxis_v7/CAF/0m/00/FHC.10000*.CAF.root"
#fNamesCAF = "/Users/cvilela/DUNE/ND_MC/nd_offaxis_v7/CAF/0m/00/FHC.100000*.CAF.root"
#fNamesCAF = "/Users/cvilela/DUNE/ND_MC/nd_offaxis_v7/CAF/0m/00/FHC.1000000.CAF.root"

NUM_PROCS = 8

N_THROWS_PER_EVENT = 100

efficiencyProjections = [ ["Ev", 100, 0, 10],
                          ["LepE", 100, 0, 10],
                          ["vtx_x", 200, -400, 400],
                          ["vtx_y", 100, -200, 200],
                          ["vtx_z", 150, 0, 600] ,
                          ["Ev-LepE", 100, 0, 10]]

def processFiles(f_list, h_denoms, h_selected, h_corrected) :
    
    # Analyse one file at a time, otherwise memory explodes!
    for f in f_list :
 
        try :
            CAF = uproot4.concatenate("{0}:caf".format(f), treeVarsToRead, library = "np")
        except uproot4.exceptions.KeyInFileError :
            print("Couldn't find CAF TTree in file {0}. Skipping.".format(f))
            continue
        print(f)
        CAF["Ev-LepE"] = np.subtract(CAF["Ev"], CAF["LepE"])

        isFV_vec = np.vectorize(isFV)

        if APPLY_FV_CUT :
            CAF["inFV"] = isFV_vec(CAF["vtx_x"], CAF["vtx_y"], CAF["vtx_z"])
        else :
            CAF["inFV"] = [True]*len(CAF["vtx_x"])
    
        geoThrows = uproot4.concatenate("{0}:geoEffThrows".format(f), ['geoEffThrowsY', 'geoEffThrowsZ', 'geoEffThrowsPhi'], library = "np")

        effs = np.array([0.]*len(CAF['geoEffThrowResults']), dtype = np.float16)
    
        for i_event in range(len(CAF['geoEffThrowResults'])) :
#        for i_event in range(100) :
#            print(i_event)
            thisEff = 0.

            throws_FV = isFV_vec([CAF["vtx_x"][i_event]]*len(geoThrows["geoEffThrowsY"][int(i_event/N_THROWS_PER_EVENT)]),
                                 geoThrows["geoEffThrowsY"][int(i_event/N_THROWS_PER_EVENT)]-offset[1],
                                 geoThrows["geoEffThrowsZ"][int(i_event/N_THROWS_PER_EVENT)]-offset[2])
        
            NthrowsInFV = sum(throws_FV)
        
            for i_bitfield, bitfield in enumerate(CAF['geoEffThrowResults'][i_event][0][1]) :
                bitfield = np.array([bitfield], dtype = np.uint64)
                bitfield = np.unpackbits(np.array(bitfield, dtype='>i8').view(np.uint8))
                
                bitfieldTemp = np.copy(bitfield)

                for j_bitfield in range(len(bitfield)) :
                    bitfield[-(1+j_bitfield)] = bitfieldTemp[j_bitfield]
            
                if APPLY_FV_CUT :
                    thisEff += np.sum(np.logical_and(bitfield, throws_FV[i_bitfield*64:(i_bitfield+1)*64]))
                else :
                    thisEff += np.sum(bitfield)

            if APPLY_FV_CUT :
                effs[i_event] = float(thisEff)/NthrowsInFV
            else :
                effs[i_event] = thisEff/(78.*64)

        fv = CAF["inFV"]
        had_containment = CAF["Ehad_veto"] < 30
        sel = np.logical_and(had_containment, fv)

        print("Number in FV {0}, number contained {1}, number in FV and contained {2}".format(sum(fv), sum(had_containment), sum(sel)))
    
        effs_sel = effs[sel]
        print(effs_sel)
        effs_sel_inv = np.reciprocal(effs_sel)
    
        for iproj, proj in enumerate(efficiencyProjections) :
            print(len(CAF[proj[0]][fv]), len(CAF[proj[0]][sel]))
        
            this_h_denom, _ = np.histogram(CAF[proj[0]][fv], bins = proj[1], range = (proj[2], proj[3]))
            this_h_selected, _ = np.histogram(CAF[proj[0]][sel], bins = proj[1], range = (proj[2], proj[3]))
            this_h_corrected, _ = np.histogram(CAF[proj[0]][sel], weights = effs_sel_inv, bins = proj[1], range = (proj[2], proj[3]))

            h_denoms_temp = np.add(this_h_denom, h_denoms[iproj])
            for i in range(len(h_denoms[iproj])) :
                h_denoms[iproj][i] = h_denoms_temp[i]

            h_selected_temp = np.add(this_h_selected, h_selected[iproj])
            for i in range(len(h_selected[iproj])) :
                h_selected[iproj][i] = h_selected_temp[i]

            h_corrected_temp = np.add(this_h_corrected, h_corrected[iproj])
            for i in range(len(h_corrected[iproj])) :
                h_corrected[iproj][i] = h_corrected_temp[i]
                                       

if __name__ == "__main__" :
    
    bins = []
    h_denoms = []
    h_selected = []
    h_corrected = []
    
    for proj in efficiencyProjections :
        this_h_denom, this_bins = np.histogram([], bins = proj[1], range = (proj[2], proj[3]))
        this_h_selected, _ = np.histogram([], bins = proj[1], range = (proj[2], proj[3]))
        this_h_corrected, _ = np.histogram([], bins = proj[1], range = (proj[2], proj[3]))

        bins.append(this_bins)
        h_denoms.append(Array('f', this_h_denom))
        h_selected.append(Array('f', this_h_selected))
        h_corrected.append(Array('f', this_h_corrected))

    allFiles = glob(fNamesCAF)

    if len(allFiles) < NUM_PROCS :
        print("Fewer files than processes, setting NUM_PROC to {0}".format(len(allFiles)))
        NUM_PROCS = len(allFiles)

    filesPerProc = int(np.ceil(float(len(allFiles))/NUM_PROCS))

    print(filesPerProc, NUM_PROCS)
    
    ps = []
    for i in range(NUM_PROCS) :
        ps.append(Process(target = processFiles, args = (allFiles[i*filesPerProc:(i+1)*filesPerProc], h_denoms, h_selected, h_corrected,)))
        ps[-1].start()
    for p in ps :
        p.join()
        
    for iproj, proj in enumerate(efficiencyProjections) :
        thisfig = plt.figure()
        plt.subplot(2, 1, 1)
        plt.step(bins[iproj][:-1], h_denoms[iproj], label = "Generated")
        plt.step(bins[iproj][:-1], h_selected[iproj], label = "Selected")
        plt.step(bins[iproj][:-1], h_corrected[iproj], label = "Efficiency corrected")
        plt.xlim((proj[2], proj[3]))
        plt.xlabel(proj[0])
        plt.legend()
        plt.subplot(2, 1, 2)
        plt.step(bins[iproj][:-1], np.divide(h_corrected[iproj], h_denoms[iproj]), label = "Efficiency corrected / Generated")
        plt.xlabel(proj[0])
        plt.ylim((0,1.2))
        plt.xlim((proj[2], proj[3]))
        plt.legend()
        thisfig.savefig("eff_{0}.png".format(proj[0]))
        thisfig.savefig("eff_{0}.pdf".format(proj[0]))


    plt.show()
