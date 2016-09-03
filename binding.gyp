{
    "targets": [
        {
            "target_name": "node-freerdp",
            "sources": [
              "generator.cc",
              "rdp.cc",
              "bridge.cc",
              "node-freerdp.cc"
            ],
            "libraries": [
              "-lfreerdp-cache",
              "-lfreerdp-rail",
              "-lfreerdp-gdi",
              "-lfreerdp-primitives",
              "-lfreerdp-core",
              "-lfreerdp-common",
              "-lfreerdp-locale",
              "-lfreerdp-codec",
              "-lfreerdp-client",
              "-lfreerdp-gdi",
              "-lfreerdp-utils",
              "-lfreerdp-crypto",
              "-lwinpr-crt",
              "-lxfreerdp-client",
              "-lwinpr-utils",
              "-lwinpr-synch",
              "-lwinpr-sspi"
            ],
            "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ]
        }
    ],
}