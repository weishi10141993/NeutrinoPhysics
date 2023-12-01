// Use this example job configuration file as a starting point for your own
// files.
{
  // Pure 40Ar target
  target: {
    nuclides: [ 1000180400 ],
    atom_fractions: [ 1.0 ],
  },

  // Simulate CC ve scattering on 40Ar
  reactions: [ "ve40ArCC_Bhattacharya2009.react" ],

  // Neutrino source specification
  // Valid values for the "neutrino" key are neutrino PDG codes (±12, ±14, ±16)
  // and the strings "ve", "vebar", "vu", "vubar", "vt", and "vtbar".
  source: {
    type: "mono",
    neutrino: "ve",
    energy: 30,
  },

  // An isotropic neutrino direction may be randomly sampled for each event by
  // using the following configuration:
  //
  // direction: "isotropic",
  // Incident neutrino direction 3-vector
  direction: { x: 0.0, y: 0.0, z: 1.0 },

  // Settings for marley command-line executable
  executable_settings: {

    // The number of events to generate
    events: 10,

    // Event output configuration
    output: [ { file: "marley_gen_hepevt.txt", format: "hepevt", mode: "overwrite" } ],

  },
}
