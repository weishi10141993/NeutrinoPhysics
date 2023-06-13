include_directories ( $ENV{GENIE_INC}/GENIE )

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

find_package(Eigen3 REQUIRED)

add_library(geoEff SHARED IMPORTED)
set_target_properties(geoEff PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/DUNE_ND_GeoEff/lib/libgeoEff.so)

cet_build_plugin(CAFMaker   art::module
                        duneanaobj_StandardRecord
                        duneanaobj_StandardRecordFlat
                        nugen::NuReweight
                        nugen::NuReweight_art
                        nusimdata::SimulationBase
                        ART_FRAMEWORK_CORE
                        ART_FRAMEWORK_PRINCIPAL
                        ART_FRAMEWORK_SERVICES_REGISTRY
                        ART_ROOT_IO_TFILE_SUPPORT
                        ROOT::Core
                        ART_ROOT_IO_TFILESERVICE_SERVICE
                        art::Persistency_Common canvas
                        art::Persistency_Provenance canvas
                        art::Utilities canvas
                        messagefacility::MF_MessageLogger
                        cetlib::cetlib cetlib_except
                        ROOT_BASIC_LIB_LIST
                        ${GENIE_LIB_LIST}
                        ${PYTHON_LIBRARIES}
                        Eigen3::Eigen
                        geoEff
                        systematicstools::interface
                        systematicstools::interpreters
                        systematicstools::utility
                        dunereco::CVN_func
                        BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()
install_scripts()
