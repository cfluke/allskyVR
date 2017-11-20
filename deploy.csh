#! /bin/csh -f

# ************************************************************************
#  deploy.csh                                               November 2017   
#                                                          
#  Check file execute permission are set (chmod 755 deploy.csh)
# 
#  Usage:                                                 
#  	deploy.csh
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

# Compile the source code - tested with cc 

cc -o allskyVR allskyVR.c -lm
