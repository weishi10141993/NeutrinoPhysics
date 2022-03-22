# Adapted from HDF5LIBS_TestReader.py
import sys

from hdf5libs import HDF5RawDataFile
import daqdataformats

import h5py

def main():
    ifile_name = "/dune/app/users/weishi/pdsnp02crt/np02_pds_run012451_0000_20220225T170210.hdf5"
    #ifile_name = "/dune/app/users/weishi/pdsnp02crt/np02_pds_run012452_0000_20220228T085242.hdf5"
    #ifile_name = "/dune/app/users/weishi/pdsnp02crt/np02_pds_run012453_0000_20220228T091757.hdf5"
    #ifile_name = "/dune/app/users/weishi/pdsnp02crt/np02_pds_run012454_0000_20220228T094438.hdf5"
    h5_file = HDF5RawDataFile(ifile_name)

    #get attributes using h5py
    #this is a bit messy at the moment: should try to combine interfaces here
    h5py_file = h5py.File(ifile_name,'r')
    for attr in h5py_file.attrs.items(): print ("File Attribute ",attr[0]," = ",attr[1])

    #get type of record
    record_type = h5_file.get_record_type()

    #records = h5_file.get_all_record_numbers()
    #print("Number of records: %d"%len(records))
    #print("\tRecord numbers: ",records)

    all_datasets = h5_file.get_dataset_paths()
    print("All datasets found:")
    for d in all_datasets: print(d)
    #/TriggerRecord00001/PDS/Region000/Element00
    #/TriggerRecord00001/PDS/Region000/Element02
    #...
    #/TriggerRecord00001/TriggerRecordHeader

    all_rh_paths = h5_file.get_record_header_dataset_paths()
    print("Record Headers:")
    for d in all_rh_paths:
        if record_type=="TriggerRecord":
            trh = h5_file.get_trh(d)
            print(d,": ",trh.get_trigger_number(),
                  trh.get_sequence_number(),trh.get_trigger_timestamp())
        elif record_type=="TimeSlice":
            tsh = h5_file.get_tsh(d)
            print(d,": ",tsh.timeslice_number)
    #/TriggerRecord00001/TriggerRecordHeader :  1 0 82290426051610956
    #/TriggerRecord00002/TriggerRecordHeader :  2 0 82290426071595142
    #/TriggerRecord00003/TriggerRecordHeader :  3 0 82290426074153717

    all_frag_paths = h5_file.get_all_fragment_dataset_paths()
    print("Fragments:")
    for d in all_frag_paths:
        frag = h5_file.get_frag(d)
        geoid = (frag.get_element_id().system_type,
                 frag.get_element_id().region_id,
                 frag.get_element_id().element_id)
        print(d,": ",frag.get_trigger_number(),
              frag.get_sequence_number(),geoid)
    #/TriggerRecord00001/PDS/Region000/Element00 :  1 0 (<SystemType.kPDS: 2>, 0, 0)
    #/TriggerRecord00001/PDS/Region000/Element02 :  1 0 (<SystemType.kPDS: 2>, 0, 2)
    #/TriggerRecord00001/PDS/Region000/Element03 :  1 0 (<SystemType.kPDS: 2>, 0, 3)

    # Adapted from ssp_processing.py

    # Extract event_header info
    for dpath in h5_file.get_all_fragment_dataset_paths():
        frag = h5_file.get_frag(dpath)
        if frag.get_element_id().system_type != daqdataformats.GeoID.SystemType.kPDS:
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

    sys.exit(0)

if __name__=="__main__":
    main()
