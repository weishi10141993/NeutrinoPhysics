# Impact of noise light on VD PDS resolution

## Set up

The main module is ```duneopdet/OpticalDetector/SIPMOpSensorSim_module.cc```, where we are interested in the parameter ```DarkNoiseRate```(default to 10Hz). This is equivalently the noise rate, can be treated as a proxy for PoF light noise.

```
cd /dune/app/users/weishi
mkdir VDPDSRes
cd VDPDSRes

source /grid/fermiapp/products/dune/setup_dune.sh
ups list -aK+ dunesw
setup dunesw v09_62_00d01 -q e20:prof
mrb newDev
source /dune/app/users/weishi/VDPDSRes/localProducts_larsoft_v09_62_00_e20_prof/setup
cd srcs
mrb g duneopdet
cd $MRB_BUILDDIR
setup ninja
mrbsetenv
mrb i --generator ninja
```

Marley events will be generated via the following xml file

```
/dune/app/users/apaudel/code/dune_vd_eresjobs/geng4/geng4_10new.xml

projectgui.py file.xml
```
This will go through gen, detsim, reco, ana.

To produce resolution plot:
```
/dune/app/users/apaudel/code/PDEnergyresolution_tdr/PDEcal_Sept11/.C ==> produce voxel LY calibration
/dune/app/users/apaudel/code/PDEnergyresolution_tdr/PDEcal_Sept11/*Res ==> produce resolution
```
