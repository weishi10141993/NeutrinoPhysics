#!/bin/bash
#module load gcc/5.4.0
#module load gsl/1.16
#export PATH=/home/barrow/MaCh3/procmail-3.22/new:${PATH}
#source /home/barrow/MaCh3/CMT/setup.sh
#source /home/barrow/MaCh3/root/bin/thisroot.sh
#module load cuda
#export CUDAPATH=${CUDA_HOME}
#export MACH3_DATA=/home/barrow/MaCh3/P6Data
#export MACH3_MC=/home/barrow/MaCh3/P6MC
# source PATH_TO_YOUR_THISROOT.sh
# source PATH_TO_YOUR_CMT_SETUP.sh

#source ~/vols/software/highland2/ROOT/v5r34p34n00/Linux-x86_64/bin/thisroot.sh
#source ~/vols/software/CMT/setup.sh

# CVS username
myusername="anoncvs"
if [ -z "$CMTROOT" ]; then
    echo "Can't find CMT!"
    echo "If installed, run setup.sh, else:"
    echo " "
    echo "set your CVSROOT to the ND280 repository e.g. export CVSROOT=:ext:${MYNAME}@repo.nd280.org:/home/trt2kmgr/ND280Repository;export CVS_RSH=ssh; unset CVS_SERVER"
    echo "cvs co CMT"
    echo "cd CMT"
    echo "./install_cmt"
    echo "source setup.sh"
    return 1
fi

# Set the CVS repo for psyche
export CVSROOT=:ext:${myusername}@repo.nd280.org:/home/trt2kmgr/ND280Repository
export CVS_RSH=ssh
unset CVS_SERVER

# Set the relative CMT path
export CMTPATH=$(pwd)
export MACH3=$(pwd)

cmt co -r v3r3 psyche/psycheROOT
sleep 60
cmt co -r v3r3 psyche/psychePolicy
sleep 60
cmt co -r v3r35 psyche/psycheCore
sleep 60
cmt co -r v3r33 psyche/psycheEventModel
sleep 60
cmt co -r v3r29 psyche/psycheUtils
sleep 60
cmt co -r v3r47 psyche/psycheND280Utils
sleep 60
cmt co -r v3r25 psyche/psycheIO
sleep 60
cmt co -r v3r41 psyche/psycheSelections
sleep 60
cmt co -r v3r37 psyche/psycheSystematics
sleep 60
cmt co -r v3r33 psyche/psycheSteering
# Function to find the version
# If version does not have a patch number at the end, add it
function FindVersion() {
  sed -e "s|.*\(v[0-9]r[0-9]\?[0-9]\?p\?[0-9]\?[0-9]\?\).*|\1|g"
}

# Get nd280Psyche head, correct as of 14 Jan 2020
export ND280PSYCHEVERSION=v3r47
cmt co -r ${ND280PSYCHEVERSION} -R nd280Psyche
# Then run the script in nd280Psyche to get the psyche head
# ___Warning, this will require user input___
export ND280PSYCHEVERSION=$(find ./nd280Psyche -type d -name "v[0-9]*" -printf '%f\n')
echo "Got ND280Psyche ${ND280PSYCHEVERSION}"
cd nd280Psyche/${ND280PSYCHEVERSION}/cmt
#./checkout_HEAD
echo "Checking out psyche ${ND280PSYCHEVERSION}..."
#find -name "checkout_HEAD_*" -type f -exec ./{} \;

# Find the different psyche versions we've got in the head
cd ${MACH3}
export PSYCHECOREVERSION=$(grep psycheCore nd280Psyche/${ND280PSYCHEVERSION}/cmt/requirements | FindVersion)
export PSYCHEEVENTMODELVERSION=$(grep psycheEventModel nd280Psyche/$ND280PSYCHEVERSION/cmt/requirements | FindVersion)
export PSYCHEIOVERSION=$(grep psycheIO nd280Psyche/${ND280PSYCHEVERSION}/cmt/requirements | FindVersion)
export PSYCHEND280UTILSVERSION=$(grep psycheND280Utils nd280Psyche/$ND280PSYCHEVERSION/cmt/requirements | FindVersion)
export PSYCHESELECVERSION=$(grep psycheSelections nd280Psyche/${ND280PSYCHEVERSION}/cmt/requirements | FindVersion)
export PSYCHESTEERVERSION=$(grep psycheSteering nd280Psyche/${ND280PSYCHEVERSION}/cmt/requirements | FindVersion)
export PSYCHESYSTVERSION=$(grep psycheSystematics nd280Psyche/${ND280PSYCHEVERSION}/cmt/requirements | FindVersion)
export PSYCHEUTILVERSION=$(grep psycheUtils nd280Psyche/${ND280PSYCHEVERSION}/cmt/requirements | FindVersion)

# Overwrite the cmt requirements so we don't need the ND280 version of ROOT, GSL etc
cp -v setup_psyche/requirements psyche/psycheCore/${PSYCHECOREVERSION}/cmt/.
mkdir psyche/psycheCore/${PSYCHECOREVERSION}/fragments
cp -v setup_psyche/rootcint* psyche/psycheCore/${PSYCHECOREVERSION}/fragments/.

# Then comment out the applications in psycheSteering which we never use
sed -i.bak 's,application,#application,g' psyche/psycheSteering/${PSYCHESTEERVERSION}/cmt/requirements
sed -i.bak 's,library psycheSteering.*,library psycheSteering AnalysisManager.cxx ToyMakerExample.cxx,g' psyche/psycheSteering/${PSYCHESTEERVERSION}/cmt/requirements
# Then comment out building the broken xml source
cd nd280Psyche/${ND280PSYCHEVERSION}/cmt && cmt broadcast cmt config && cmt broadcast cmt make
source setup.sh
cd ${MACH3}
# Overwrite the supplied psycheSteering parameters file with our own
cp -v psyche/psycheSteering/${PSYCHESTEERVERSION}/parameters/psycheSteering.parameters.dat psyche/psycheSteering/${PSYCHESTEERVERSION}/parameters/psycheSteering.parameters.dat.bak
cp -v psycheSteering.parameters.dat psyche/psycheSteering/${PSYCHESTEERVERSION}/parameters/

# Make the psycheinc and lib dirs
mkdir -v ${MACH3}/psycheinc
mkdir -v ${MACH3}/lib

if [[ ${ND280PSYCHEVERSION} == "v3r47" ]]; then
  echo "Patching psyche run numbering..."
  sed -i.bak 's/80910020)/80910024)/g; s/80910021 \&\&/80910025 \&\&/g; s/80910079)/80910094)/g' psyche/psycheND280Utils/v3r47/src/ND280AnalysisUtils.cxx
fi

# Copy the libraries and includes to the joint folders
# First make sure nd280Psyche is built with all the customisations above
cd nd280Psyche/${ND280PSYCHEVERSION}/cmt && cmt broadcast cmt clean && cmt broadcast make clean && cmt broadcast cmt make && cd ${MACH3}
# Function to copy libs and incs from psyche into common MaCh3 folder
function CopyLibsAndIncs() {
  path=$1
  find $1* -name *.hxx -exec cp {} ${MACH3}/psycheinc/ \;
  find $1* -name "lib*.so" -exec cp {} ${MACH3}/lib \;
}
CopyLibsAndIncs "psyche"
#CopyLibsAndIncs "psyche/psycheCore/${PSYCHECOREVERSION}"
#CopyLibsAndIncs "psyche/psycheEventModel/${PSYCHEEVENTMODELVERSION}"
#CopyLibsAndIncs "psyche/psycheIO/${PSYCHEIOVERSION}"
#CopyLibsAndIncs "psyche/psycheND280Utils/${PSYCHEND280UTILSVERSION}"
#CopyLibsAndIncs "psyche/psycheSelections/${PSYCHESELECVERSION}"
#CopyLibsAndIncs "psyche/psycheSteering/${PSYCHESTEERVERSION}"
#CopyLibsAndIncs "psyche/psycheSystematics/${PSYCHESYSTVERSION}"
#CopyLibsAndIncs "psyche/psycheUtils/${PSYCHEUTILVERSION}"

# Print results to user
echo "nd280Psyche:         " $ND280PSYCHEVERSION
echo "  psycheCore:        " $PSYCHECOREVERSION
echo "  psycheEventModel:  " $PSYCHEEVENTMODELVERSION
echo "  psycheIO:          " $PSYCHEIOVERSION
echo "  psycheND280Utils:  " $PSYCHEND280UTILSVERSION
echo "  psychePolicy:      " $PSYCHEPOLICYVERSION
echo "  psycheSelections:  " $PSYCHESELECVERSION
echo "  psycheSteering:    " $PSYCHESTEERVERSION
echo "  psycheSystematics: " $PSYCHESYSTVERSION
echo "  psycheUtils:       " $PSYCHEUTILVERSION
