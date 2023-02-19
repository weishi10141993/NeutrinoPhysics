# Instruction for running GEC on DUNE FNAL machines (dunegpvm*)

## DUNE FNAL machines (dunegpvm*)

Install these packeges
using ```venv``` or ```virtualenv``` for python packages

ups also has a lot of packges can probaby just setup cmake/numpy etc

## nnhome

Make trees,
```
cd /home/wshi/repos/MuonEffNN  # ~/repos at dunegpvm

# Install python dependencies, default path: ~/.local/lib/python3.9/site-packages/
pip install uproot4
pip install uproot3
pip install torch

# this is not needed: export PYTHONPATH=~/.local/lib/python3.9/site-packages:$PYTHONPATH

python3 new_hadron_muon_mktree.py # Python 3.9.2

# In the same dir, there are 2 other files: muonEffModel.py and muonEff30.nn. They store the neural network training.
# ~/repos directory on Fermilab

# the python script will create a list of efficiency files at /storage/shared/wshi/files4baron
```

Make file GEC spectrum:

```
root new_hist_fill.C
```
