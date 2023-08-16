# Instruction for running Baron's code

## Near detector events

From nnhome machine:
```
mkdir testbaroncode
cd testbaroncode
```

The ND CAFs have been copied from Fermilab to nnhome under this path:
```
/storage/shared/barwu/10thTry/NDCAF
```
They are grouped into different folders, as well as numerical subfolders. You will need to check these names to decide which of the subfolders you will run.

The following scripts are used to produce raw, selected, and efficiency-corrected distributions. These are the red-green-blue curves for muon only (fully contained and tracker matched), hadron only, and combined.

Run the script using python3, this creates a ttree containing all efficiency information:
```
python new_hadron_muon_mktree.py [folder]/[subfolder]
```

A batch submission is also available, it usually tkaes half a ay to complete:
```
sbatch Slurm_nnhome_birch.sh [folder]/[subfolder]
```
The batch job should generate 1000 efficiency files. These files contain the efficiency information of each event, the cut information, as well as 3 additional sets of information
about the total lepton momentum, longitudinal lepton momentum and the cosine of the angle between the neutrino and lepton momentum vectors.


To convert all the efficiency files into a set of histograms, run:
```
sbatch Slurm_hists.sh
```
This will run the c++ script ```histogram_files.cpp``` where each histogram is saved into its individual TFile. You can see example histograms created here:
```
/storage/shared/barwu/10thTry/0m_histograms/0.005_eff_veto_cut
```
They have been organized depending on what parameter is being plotted, for example, neutrino energy or position. In each group, it contains graphs for raw, selection-cut and
geometrically-corrected distributions.


Now run the script to draw the histograms using the aforementioned TFiles:
```
root draw_histograms.cpp
```
There will be a set of all graphs available, and a copy of each of those graphs organized by selection-cut into
different TCanvases. Ratio plots for each graph is also available. The graphs and ratio plots will automatically save once they are finished loading.


### Muon NN

The network outputs how probable a muon is fully contained in ND LAr and tracker matched in TMS downstream: https://github.com/weishi10141993/MuonEffNN
The current used network file is located at ```/home/barwu/repos/MuonEffNN/8thTry/muonEff30.nn```

## Far detector events put in ND

Run this script:
```
python FD_maketree.py
```
This generates efficiency files for far detector events.

To submit a job:
```
sbatch Slurm_nnhome.sh
```

Run this script to draw histograms directly using the FD CAFs and efficiency files produced by the above shell script:
```
root FD_selection_cuts1.cpp
```
