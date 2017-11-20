#! /bin/csh -f

# ************************************************************************
#  allskyVRmaker.csh                                        November 2017
#
#  https://github.com/cfluke/allskyVR
# 
#  Questions or Comments?  cfluke@swin.edu.au
#
# ***************************************************************************
#  GNU General Public License v3.0
# ***************************************************************************
#  Permissions of this strong copyleft license are conditioned on making 
#  available complete source code of licensed works and modifications, 
#  which include larger works using a licensed work, under the same license. 
#  Copyright and license notices must be preserved. Contributors provide 
#  an express grant of patent rights.         
# ***************************************************************************


# A-Frame base name
set af="us2aframe"

# Base directory
set bd="allskyVRBase"

if ($1 == "") then
   set oname="VR-us2output"
else 
   set oname = "$1"
endif

echo "Building VR assets and web source in directory ${oname}"

set links="us2links.html"
set assets="us2assets.html"
set skycube="skycube"
set menuexport="us2menuexport.csh"

if ( -d ${oname}) then
   echo "${oname} already exists: removing it"
   rm -rf ${oname}
endif

mkdir ${oname} 

cat ${bd}/${af}_head.html ${assets} ${bd}/${af}_camera.html ${links} ${bd}/${af}_skycube.html ${af}.html ${bd}/${af}_foot.html > ${af}_index.html

cp ${bd}/AssetLibrary/posx.png ${oname}
cp ${bd}/AssetLibrary/posy.png ${oname}
cp ${bd}/AssetLibrary/posz.png ${oname}
cp ${bd}/AssetLibrary/negx.png ${oname}
cp ${bd}/AssetLibrary/negy.png ${oname}
cp ${bd}/AssetLibrary/negz.png ${oname}
cp ${bd}/AssetLibrary/Texture*.jpg ${oname} 
cp ${bd}/AssetLibrary/Menu.jpg ${oname} 
cp ${bd}/AssetLibrary/SkyCube.jpg ${oname} 

cp ${af}.html ${oname} 
mv ${af}_index.html ${oname}/index.html

# Clean up
mv ${assets} ${oname}
mv ${links} ${oname}
mv ${af}.html ${oname}

cp ${bd}/toggle-visible.js ${oname} 

tar -cvf ${oname}.tar ${oname} >& /dev/null
mv ${oname}.tar ${oname}
cd ${oname}


# Edit this out on Linux system
open index.html
