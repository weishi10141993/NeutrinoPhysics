import math
import numpy as np
##import pandas as pd
##import seaborn as sns
import matplotlib.pyplot as plt
#from scipy.fftpack import rfft, rfftfreq

#from kafka import KafkaProducer

import sys

from hdf5libs import HDF5RawDataFile
import daqdataformats
import detdataformats.ssp
from rawdatautils import *
import h5py


# Parameters
filename = "/dune/app/users/weishi/pdsnp02crt/np02_pds_run012451_0000_20220225T170210.hdf5"
#filename = "/dune/app/users/weishi/pdsnp02crt/np02_pds_run012452_0000_20220228T085242.hdf5"
#filename = "/dune/app/users/weishi/pdsnp02crt/np02_pds_run012453_0000_20220228T091757.hdf5"
#filename = "/dune/app/users/weishi/pdsnp02crt/np02_pds_run012454_0000_20220228T094438.hdf5"
#num_events = 1
save_path = "/dune/app/users/weishi/pdsnp02crt/"

# initialize variable lists
module_id = []
channel_id = []
ext_timestamp = []
peaksum = []
peaktime = []
prerise = []
intsum = []
baseline = []
baseline_sum = []

# Extract event_header info
ssp_file = HDF5RawDataFile(filename)

for dpath in ssp_file.get_all_fragment_dataset_paths():
    frag = ssp_file.get_frag(dpath)
    if frag.get_element_id().system_type!=daqdataformats.GeoID.SystemType.kPDS:
        print("WARNING: fragment is not PDS type")
        break

    ## Parse SSP frag
    event_header = detdataformats.ssp.EventHeader(frag.get_data())
    ts = (event_header.timestamp[3] << 48) + (event_header.timestamp[2] << 32) + (event_header.timestamp[1] << 16) + (event_header.timestamp[0] << 0)
    ext_timestamp.append(ts)
    module_id.append((event_header.group2 & 0xFFF0) >> 4)
    channel_id.append((event_header.group2 & 0x000F) >> 0)
    peaktime.append((event_header.group3 & 0xFF00) >> 8)
    prerise.append(((event_header.group4 & 0x00FF) << 16) + event_header.preriseLow)
    intsum.append((event_header.intSumHigh << 8) + ((event_header.group4 & 0xFF00) >> 8))
    baseline.append(event_header.baseline)
    baseline_sum.append(((event_header.group4 & 0x00FF) << 16) + event_header.preriseLow)

# Extract waveforms
ssp_data = SSPDecoder(filename)
ssp_frames = ssp_data.ssp_frames

if (len(module_id) != len(ssp_frames)):
    print("Fix Me!")

print(len(module_id)) # fragment count?
print(len(channel_id))
print(len(ssp_frames))

## Generate plots

## ADC values over time
fig = plt.figure(figsize=(35,15))
ax = fig.add_subplot()

for i in range(len(ssp_frames)):
    plt.scatter(np.arange(len(ssp_frames[i])), ssp_frames[i], s=40, label="Module_Channel_ID "+str(module_id[i])+"_"+str(channel_id[i]))

plt.ylabel("ADC value")
plt.xlabel('Time ticks')
plt.title("SSP")
plt.grid()
plt.legend()
#plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.20))

plt.savefig(save_path+"ADC_time.png")
plt.clf()

## FFT ADC values
#fig = plt.figure(figsize=(35,15))
#ax = fig.add_subplot()

#for i in range(n_frags_saved):
    ## sanity check
    ##if len(ssp_frames[i])<480:
    ##    print(ssp_frames[i])
    ##    continue
#    fft = np.abs(rfft(ssp_frames[i]))
#    freq = rfftfreq(482)
#    plt.plot(freq, fft, label="Module_Channel_ID "+str(module_id[i])+"_"+str(channel_id[i]))

#plt.ylabel("FFT")
#plt.xlabel("Frequency")
#plt.legend()

#plt.savefig(save_path+"FFT.png")

## Integrated sum ADC values
#if num_events==1:
#    fig = plt.figure(figsize=(35,15))

#    pivot_data = df.pivot(index="module_id", columns="channel_id", values="intsum")
#    ax = sns.heatmap(pivot_data)

#    plt.title("Integrated sum ADC values")
#    plt.ylabel("module")
#    plt.xlabel("channel")
#    plt.legend()
#
#    plt.savefig(save_path+"Intsum.png")
#else:
#    print("intsum only gets plotted when num_events is 1 for now")
