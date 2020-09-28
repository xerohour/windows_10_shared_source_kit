######################################################################
# File:  Makefile
#
# Purpose:
# It's for making stuff.  That's what makefiles DO, silly rabbit.
#
# See also:
# Anvil.mak   For the VCVars environment.
# sources     For the Razzle environment.
#
# History:
# 21 Mar 2000  SEdmison  Created.
# 25 Apr 2000  SEdmison  Added some comments.  Comments!  In a
#                        Makefile!  Who knew?!
######################################################################


!if "$(NTMAKEENV)" != ""
############################################################
#
# NTBuild-specific section
#
############################################################

#
# Standard disclaimer from the NT build deities:
#
# This section is needed for NT builds- please do not replace
# It will NOT hurt your make process.
# IF YOU DELETE THIS SECTION IT WILL MAKE THE NT BUILD PROCESS DIE.
# This means that you will have to start doing your own RISC builds.
# Please do not delete this section. IT WILL NOT HARM YOUR BUILDS
# TO LEAVE IT IN. IT IS BENIGN.
#
!include $(NTMAKEENV)\makefile.def


!else
############################################################
#
# Anvil-specific targets
# N.B. [sedmison]:  These just get passed through to the
# VCVars32 nmake.
#
############################################################
all:
	nmake /nologo /f Anvil.mak all

clean:
	nmake /nologo /f Anvil.mak clean

prep:
	nmake /nologo /f Anvil.mak prep

symbols:
	nmake /nologo /f Anvil.mak symbols


!endif   # !if NTMAKEENV
