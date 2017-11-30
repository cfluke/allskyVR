Getting started
===============

*Immersive virtual reality experiences for all-sky astronomical data.*

To see an example in action:

* Using a WebGL compatible browswer, navigate to https://astronomy.swin.edu.au/~cfluke/vr/webundle/ on your smartphone
* Tap the Google Cardboard icon in the lower right of the screen
* Place your smartphone into a Google Cardboard viewer
* Navigation: turn your head to see the celestial sphere around you.
* Menu: turn your head to look for the Menu icon.  Line up the green circular target, and hold your gaze steady.  The menu will expand.
* Selection: to toggle visibility of a data category or the celestial coordinate system, line-up the reticule on the menu item and hold your gaze steady.


Quickstart
^^^^^^^^^^

The Quickstart versions works on Linux and MacOSX operating systems.  It requires a C compiler (gcc or cc).

1. Navigate to the Github repository: https://github.com/cfluke/allskyVR
2. Download and expand the zip file
3. Change into the *allskyVR* directory and execute the deploy script from the command line: <tt>deploy.csh</tt>
4. Create the sample VR environment: *allskyVR -i exoplanets.csv -f format.txt*
5. Transfer the export directory, *VR-allsky*, to a web server
6. Open your web browser, navigate to the export directory, and view *index.html*

You are now ready to explore your all-sky data with your personal head-mounted display.

Fully Customisable
^^^^^^^^^^^^^^^^^^

The Fully Customisable version requires a working installation of `S2PLOT <https://github.com/mivp/s2plot>`_ (Version 3.4.0 or higher).  Please see the *S2PLOT* repository for installation notes, including the required environment variables.  

The *allskyVR* source code is written in C.  The preferred starting point is to modify and use the *templateSpherical.c* code. 

Before executing, set the following two *S2PLOT* environment variables to the same value (the example uses tcsh *setenv* commands).  Choose a pixel size that is as large as possible for your display:

    setenv S2PLOT_WIDTH 800
    
    setenv S2PLOT_HEIGHT 800
    
Build and execute the *templateSpherical.c* example code:

    us2build.csh templateSpherical
    
    templateSpherical -i exoplanets.csv -f format.txt
    
 


Creating your own data file
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Now it is time to explore your own dataset.

1. Create a comma-separated value (csv) data file (yourdata.csv in this example). 
Each celestial object is defined on a single line with the format: 

    RA,Dec,Radius,Category,Scale 

* Celestial coordinates are in decimal degrees
* For an all-sky view set the Radius to 1
* Assign a category index number in the range [1..10]
* Provide the relative scale factor, using value of 1 if you are not sure what to choose!

2. Create your immersive environment: allsky -i yourdata.csv -f format.txt

Creating your own format file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The format file sets the colours that will be assigned to the different category index values.   The text file format, one item per line, is:

    CAT=R,G,B,Label
    
R,G,B are integer red, green, and blue colour index values in the range [0...255], and <tt>Label</tt> is a short text-only label to appear in the A-Frame menu item.   It is necessary to avoid spaces and some symbols that have special meanings in LaTeX (e.g. $ and _ ).  The Label is ignored in Quickstart mode - customisation can be performed by creating relevant textures in appropriate graphics package.
 
For best visual quality, we recommended the use of colour choosing resources such as the `ColorBrewer <http://colorbrewer2.org/#type=sequential&scheme=BuGn&n=3">`_.

Troubleshooting
^^^^^^^^^^^^^^^

* Can't navigate? You may need to active WebGL in your browser.  For information on supported browsers see: https://get.webgl.org/get-a-webgl-implementation/

