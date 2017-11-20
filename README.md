# allskyVR
Immersive virtual reality experiences for all-sky astronomical data

Made with s2plot and A-Frame WebVR, allskyVR allows astronomers to immerse themselves in all-sky data.   

Requires a compatible smart phone and Google cardboard viewer or high-end consumer VR head-mounted display.

To see an example in action: <a href="http://astronomy.swin.edu.au/~cfluke/vr/webundle/" target=_NEW>link</a>

<b>Navigation:</b> turn your head to see the celestial sphere around you
<b>Menu:</b> look for the Menu icon, line up the green circluar target, hold steady and the menu will expand.
<b>Selection:</b> to toggle visibility of a data category or the celestial coordinate system, line-up the reticule and hold steady.

# Quickstart

The Quickstart versions works on Linux and MacOSX operating systems.  It requires a C compiler (gcc or cc).

1. Download the contents of the allskyVR repository
2. Expand the zip file and move to the directory of your choice
3. Change into the allskyVR directory and execute the deploy script from the command line: <tt>deploy.csh</tt>
4. Create the sample VR environment: <tt>allskyVR -i exoplanets.csv -f format.txt</tt>
5. Transfer the export directory, <tt>VR-allsky</tt>, to a web server
6. Open your web browser, navigate to the export directory, and view <tt>index.html</tt>

You are now ready to explore your all-sky data with your personal head-mounted display.

# Creating your own data file

Now it is time to explore your own dataset.

1. Create a comma-separated value (csv) data file (yourdata.csv in this example). 
Each celestial object is defined on a single line with the format: 

    RA,Dec,Radius,Category,Scale 

  - Celestial coordinates are in decimal degrees
  - For an all-sky view set the Radius to 1
  - Assign a category index number in the range [1..10]
  - Provide the relative scale factor, using value of 1 if you are not sure what to choose!
2. Create your immersive environment: allsky -i yourdata.csv -f format.txt

# Troubleshooting

- Can't navigate? You may need to active WebGL in your browser.
