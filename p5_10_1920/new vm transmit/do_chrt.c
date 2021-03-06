/* The kernel call implemented in this file:
 *   m_type:	SYS_CHRT
 *
 * The parameters for this kernel call are:
 *    m2_i1:	RA_PROC_NR   the process number
 *    m2_l1:	RA_DEADLINE  deadline
 */

#include "kernel/system.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <lib.h>
#include <minix/endpoint.h>

#if USE_CHRT

int do_chrt(struct proc *caller, message *m_ptr)
{

  struct proc *rp;
  long exp_time;

  exp_time = m_ptr->m2_l1;
  //if (exp_time < 0)
  //{
     //printf("error!exp_time<0 \n");
	//    return -1;
  //}
  rp = proc_addr(m_ptr->m2_i1);
  rp->p_priority = 5;
  rp->p_deadline = exp_time;//pdeadline

  return (OK);
}

#endif /* USE_CHRT */