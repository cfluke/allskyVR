#! /bin/csh -f

# ****************************************************************************/
#  us2vrmenumaker.csh                                       November 2017    */
#                                                                            */
#  Usage:                                                                    */
#                                                                            */
#  https://github.com/cfluke/allskyVR                                        */
#                                                                            */
#  Questions or Comments?  cfluke@swin.edu.au                                */
#                                                                            */
# ****************************************************************************/
#  GNU General Public License v3.0                                           */
# ****************************************************************************/
#  Permissions of this strong copyleft license are conditioned on making     */
#  available complete source code of licensed works and modifications,       */
#  which include larger works using a licensed work, under the same license. */
#  Copyright and license notices must be preserved. Contributors provide an  */
#  express grant of patent rights.                                           */
# ****************************************************************************/

# A-Frame base name
set af="us2aframe"

# Base directory
set bd="us2BaseDirectory"

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

convert posx.tga ${oname}/posx.png; rm posx.tga
convert posy.tga ${oname}/posy.png; rm posy.tga
convert posz.tga ${oname}/posz.png; rm posz.tga
convert negx.tga ${oname}/negx.png; rm negx.tga
convert negy.tga ${oname}/negy.png; rm negy.tga
convert negz.tga ${oname}/negz.png; rm negz.tga

cp ${af}.html ${oname} 
mv ${af}_index.html ${oname}/index.html

chmod 755 ${menuexport}
${menuexport} ${oname}

cp ${bd}/toggle-visible.js ${oname} 


tar -cvf ${oname}.tar ${oname} 
mv ${oname}.tar ${oname}

# Clean up!!
rm -rf ${af}.html
rm -rf ${assets}
rm -rf ${links}
rm -rf labelme.tga
rm -rf taglist.txt
rm -rf ${menuexport}

cd ${oname}

if (`uname` == "Darwin") then
   open -a firefox index.html
else
   echo "Navigate to ${bd} and open index.html"
endif
