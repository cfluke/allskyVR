Which mode should I use?

- If you have, or can create, a text file containing RA, Dec values for your all-sky catalogue, then you may wish to start with the Quickstart mode.
- If you have access to, or a willing to install, the <tt>s2plot</tt> interactive graphics library, and want to be able to modify source code to add new features, than you may wish to try the Fully Customisable mode.

# Quickstart

Manifest:
- allskyVR.c         C program file (minimal inline comments on first release)
- allskyVR.h         C header file
- allskyVRmaker.csh  Required by allskyVR executable to gather relevant assets into the export directory
- deploy.csh         Vanilla script to compile allskyVR source code
- exoplanets.csv     Contains an example comma separated variable (CSV) input file
- format.txt         Contains an example colour mapping table input file
- allskyVRBase       Directory containing HTML, Javascript, and image assets for simplifying A-Frame resource building

Usage:

deploy.csh

allskyVR -i exoplanet.csv -f format.txt [-o outputdir]

Creates:

Directory VR-allsky (or user-define VR-outputdir) containing all relevant assets.

# Fully Customisable

Requires installation of <a href="https://github.com/mivp/s2plot">S2PLOT</a> (Version 3.4.0 or higher) and <a href="http://www.imagemagick.org">ImageMagick</a>, along with envrionment variables.

Manifest
- templateSpherical.c   Template C source code for reading and display all sky catalogue data
- us2astro.c         C source code containing general purpose astronomical code, mostly relating to coordinate systems
- us2publish.c       C source code managing the creation of tags.  Contains code for 3D-PDF publishing, which is not required for VR.
- us2menumaker.c     C program file for creating textures and related assets for customising the VR environment menu system
- us2astro.h         C header file for us2astro.c
- us2publish.h       C header file for us2publish.cd
- us2build.csh       Script for compiling and linking required source, builds templateSpherical executable
- us2vrmaker.csh     Required by executable to gather relevant assets into the export directory
- exoplanets.csv     Contains an example comma separated variable (CSV) input file
- format.txt         Contains an example colour mapping table input file
- us2BaseDirectory   Directory containing HTML, Javascript, and image assets for simplifying A-Frame resource building 

Usage:

us2build.csh templateSpherical

templateSpherical -i exoplanet.csv -f format.txt [-o outputdir]

Creates:

Directory VR-us2output (or user-defined VR-outputdir) containing all relevant assets.
