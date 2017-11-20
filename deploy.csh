# ************************************************************************************
#  allskyVR.C                                                         November 2017   
#                                                                                     
#  Usage:                                                                             
#          allsky -i exoplanets.csv -f format.txt [-o output-directory]               
#                                                                                     
#          -i = input file name                                                       
#          -f = geometry format file name                                             
#          -i = output directory file name                                            
#                                                                                     
#  https://github.com/cfluke/allskyVR                                                 
#                                                                                     
#  Questions or Comments?  cfluke@swin.edu.au                                         
#                                                                                     
# ************************************************************************************
#  GNU General Public License v3.0                                                    
# ************************************************************************************
#  Permissions of this strong copyleft license are conditioned on making available    
#  complete source code of licensed works and modifications, which include larger     
#  works using a licensed work, under the same license. Copyright and license notices 
#  must be preserved. Contributors provide an express grant of patent rights.         
# ************************************************************************************



#! /bin/csh -f

# Compile the source code - tested with cc 

cc -o allskyVR allskyVR.c -lm
