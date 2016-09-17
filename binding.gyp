{
    "targets": [
        {
            "target_name": "node-freerdp",
            "sources": [
              "context.cc",
              "generator.cc",
              "rdp.cc",
              "bridge.cc",
              "cliprdr.cc",
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