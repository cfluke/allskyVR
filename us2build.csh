#!/bin/csh -f

# ****************************************************************************/
#  us2build.csh                                             November 2017    */
#                                                                            */
#  Built on cbuild.csh - part of the S2PLOT source distribution              */
#  Please see original cbuild.csh for full copyright notice                  */
#                                                                            */
#  Usage:                                                                    */
#         us2build.csh templateSpherical                                     */
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

if (!(${?S2PATH})) then
  echo "S2PATH environment variable MUST be set ... please fix and retry."
  exit(-1);
endif
if (! -d $S2PATH || ! -e ${S2PATH}/scripts/s2plot.csh) then
  echo "S2PATH is set but invalid: ${S2PATH} ... please fix and retry."
  exit(-1);
endif
source ${S2PATH}/scripts/s2plot.csh
if ($status) then
  exit(-1)
endif

set thisdir=`pwd`

if ($thisdir == $S2PATH) then
  echo "You must NOT be in directory ${S2PATH} to build your own programs."
  exit(-1);
endif

set target=`echo $1 | sed -e 's/\.c$//g'`
set source=${target}.c
set object=${target}.o
if (! -e ${source}) then
  echo "Cannot find source code: ${source}[.c]"
  exit(-1);
endif 

echo "Compiling source code file ${source} ..."
$S2COMPILER ${S2EXTRAINC} -I${S2PATH}/src $source
$S2COMPILER ${S2EXTRAINC} -I${S2PATH}/src us2astro.c 
$S2COMPILER ${S2EXTRAINC} -I${S2PATH}/src us2publish.c 
$S2COMPILER ${S2EXTRAINC} -I${S2PATH}/src us2menumaker.c 

echo "Linking object file ${target}.o ..."
$S2CLINKER -o $target ${object} us2astro.o us2publish.o -L${S2PATH}/${S2KERNEL} ${S2LINKS} ${MLLINKS} ${SWLINKS} ${GLLINKS} -L${S2X11PATH}/lib${S2LBITS} ${S2FORMSLINK}  ${IMATH} -lm ${XLINKPATH} ${S2EXTRALIB}

$S2CLINKER -o us2menumaker us2menumaker.o us2astro.o us2publish.o -L${S2PATH}/${S2KERNEL} ${S2LINKS} ${MLLINKS} ${SWLINKS} ${GLLINKS} -L${S2X11PATH}/lib${S2LBITS} ${S2FORMSLINK}  ${IMATH} -lm ${XLINKPATH} ${S2EXTRALIB}

if ((${?S2INSTALLPATH})) then
  if (! -d $S2INSTALLPATH) then
    echo "S2INSTALLPATH is set but invalid (i.e. doesn't exist) ... please fix and retry."
    exit(-1);
  else
    echo "Installing ${target} in ${S2INSTALLPATH} ..."
    cp -f $target $S2INSTALLPATH
    rm -f $target
  endif
endif

echo "Cleaning up ..."
rm -rf $object us2publish.o us2astro.o us2menumaker.o

echo "Done!"
