
/* dummy vpiuser.c when not compiled into cverobj.o form */
#include "vpi_user.h"
#include "cv_vpi_user.h"



/* Template functin table for added user systf tasks and functions.
   See file vpi_user.h for structure definition
   Note only vpi_register_systf and vpi_ or tf_ utility routines that 
   do not access the simulation data base may be called from these routines
*/ 


void (*vlog_startup_routines[]) () =
{
 0
};
