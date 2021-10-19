//////////////////////////////////////////////////////////////////////////////
// prog: taskcreate.c
// comm: task creation and stuff: OSTaskCreateExt, OSTaskStkChk
// auth: MSC
//////////////////////////////////////////////////////////////////////////////

#include "includes.h"
#include "taskcreate.h"
#include "main.h"


// Create different stacks for the tasks:
// give each thread/task/process its own stack with size
// stacks are allocated here statically, because malloc() is not supported
OS_STK MboxPostTask_Stk  [STACK_SIZE];
OS_STK MboxPendTask_Stk  [STACK_SIZE];
OS_STK InputFlagPost_Stk [STACK_SIZE];
OS_STK InputFirst_Stk	 [STACK_SIZE];
OS_STK InputSecond_Stk	 [STACK_SIZE];
OS_STK QueuePostTask_Stk [STACK_SIZE];
OS_STK QueuePendTask_Stk [STACK_SIZE];


// allocation of array of structures
// notes: the last structure is set to zero's, not necessary, but it simplifies
//        to disable a task: insert // in front of the line
// 1. looping thru the array (until null) and 2. copy-pasting with the last comma
TASKDATA tasks[] =
{
//  name           stack              priority            stacksize   option			   name
// -------------------------------------------------------------------------------------------------

    // in mailbox.c
{   MboxPostTask,  MboxPostTask_Stk,  MBOXPOSTTASK_PRTY,  STACK_SIZE, OS_TASK_OPT_STK_CHK, "mboxpost" },
{   MboxPendTask,  MboxPendTask_Stk,  MBOXPENDTASK_PRTY,  STACK_SIZE, OS_TASK_OPT_STK_CHK, "mboxpend" },

    // in flag.c
{   InputFlagPost, InputFlagPost_Stk, INPUTFLAGPOST_PRTY,  STACK_SIZE, OS_TASK_OPT_STK_CHK, "flagpost"   },
{   InputFirst,    InputFirst_Stk,    INPUTFIRST_PRTY,     STACK_SIZE, OS_TASK_OPT_STK_CHK, "inputfirst" },

    // in queue.c
{   QueuePostTask, QueuePostTask_Stk, QUEUEPOSTTASK_PRTY, STACK_SIZE, OS_TASK_OPT_STK_CHK, "qpost"  },
{   InputQueuePendTask, QueuePendTask_Stk, QUEUEPENDTASK_PRTY, STACK_SIZE, OS_TASK_OPT_STK_CHK, "qpend"  },

    // laatste, niet wissen: nodig om for-loop af te sluiten
{   NULL,          NULL,              0,                  0,          0,                    "" }
};


//////////////////////////////////////////////////////////////////////////////
// func: createTasks
// args: none
// comm: all tasks are created in a loop, using the TASKDATA array
// note: the stacks are allocated statically; dynamic allocation would be an
//       improvement, but malloc() is not supported.
//////////////////////////////////////////////////////////////////////////////
void CreateTasks()
{
	// using a pointer to a structure: pointer is incremented
	PTASKDATA ptd = tasks; // tasks == &tasks[0]: both are addresses of first struct
	for (; ptd->taskname != NULL; ptd++)
	{
		OSTaskCreateExt(
			ptd->taskname,	 					// taskname
			ptd, 								// ms: why not use it to give taskinfo to task as pdata
			ptd->stack + ptd->stacksize-1, 		// top of stack
			ptd->priority,						// priority
            ptd->priority, 						// not used
			ptd->stack, 						// bottom of stack
			ptd->stacksize, 					// size of stack
			NULL, 								// not used
			ptd->option);						// enable stackchecking

		UART_puts("OSTaskCreate: "); UART_puts(ptd->name); UART_puts("\n\r");
	}
	// or, by using an integer to get the right structure
	// this version is simpler to understand, but less elegant
	/*
	//int       nr_of_tasks = tasks/sizeof(TASKDATA) -1; // nice, but not used
	int i;
	for (i=0; tasks[i].taskname != NULL; i++)
	{
		OSTaskCreateExt(
			tasks[i].taskname, 					// taskname
			NULL, 								// not used
			&tasks[i].stack[tasks[i].stacksize-1], 	// top of stack
			tasks[i].priority,					// priority
            tasks[i].priority, 					// not used
			tasks[i].stack, 					// bottom of stack
			tasks[i].stacksize, 				// size of stack
			NULL, 								// not used
			tasks[i].option);					// enable stackchecking
	}
	*/
}

//////////////////////////////////////////////////////////////////////////////
// func: displayTaskStatus
// args: address of task control block, char flag for displaying purpose
// ret:  none
// comm: displays the actual running status for a task
//////////////////////////////////////////////////////////////////////////////
void displayTaskStatus(OS_TCB *ptaskdata, char title)
{
	//#define  OS_STAT_RDY                 0x00u  /* Ready to run                                            */
	//#define  OS_STAT_SEM                 0x01u  /* Pending on semaphore                                    */
	//#define  OS_STAT_MBOX                0x02u  /* Pending on mailbox                                      */
	//#define  OS_STAT_Q                   0x04u  /* Pending on queue                                        */
	//#define  OS_STAT_SUSPEND             0x08u  /* Task is suspended                                       */
	//#define  OS_STAT_MUTEX               0x10u  /* Pending on mutual exclusion semaphore                   */
	//#define  OS_STAT_FLAG                0x20u  /* Pending on event flag group                             */
	//#define  OS_STAT_MULTI               0x80u  /* Pending on multiple events                              */
	//#define  OS_STAT_PEND_ANY         (OS_STAT_SEM | OS_STAT_MBOX | OS_STAT_Q | OS_STAT_MUTEX | OS_STAT_FLAG)

	UART_puts(title ? " Taskstatus=" : ", New status=");

	switch(ptaskdata->OSTCBStat)
	{
	case OS_STAT_RDY:     UART_puts("ready to run"); break;
	case OS_STAT_SUSPEND: UART_puts("suspended");   break;
	case OS_STAT_FLAG:    UART_puts("pending on event flag"); break;
	case OS_STAT_SUSPEND | OS_STAT_FLAG:
		     	 	 	  UART_puts("suspended & pending on event flag"); break;
	default: 			  UART_putint(ptaskdata->OSTCBStat); break; // unexpected status
	}
}

//////////////////////////////////////////////////////////////////////////////
// func: displayStackData
// args: priority (== process-id)
// comm: functions retrieves and displays StackData-information
//       the OS-call is an optional setting in the STACKDATA array
//	     (next to that, task running status is displayed)
// note: function is important, to check if task has enough memory, or
//       if task has too much memory;
//////////////////////////////////////////////////////////////////////////////
void displayStackData(char *name, int priority)
{
    OS_STK_DATA stackdata; // allocate structure
	INT8U       error;
    OS_TCB      taskdata; // is a task running or paused?

	if ((error = OSTaskStkChk(priority, &stackdata))) // get data onto this struct
	    return;

    UART_puts("\n\rStackData for "); UART_puts(name); UART_puts(":"); UART_putint(priority);
    UART_puts(", \tUsage=");   UART_putint((int)stackdata.OSUsed/4);
    UART_puts(" - Free=");	 UART_putint((int)stackdata.OSFree/4);

    // extend with some extra data, why not...
	OSTaskQuery(priority, &taskdata);
	UART_puts(", \t"); displayTaskStatus(&taskdata, TRUE);
}


//////////////////////////////////////////////////////////////////////////////
// func: displayAllStackData
// args: none
// ret:  none
// comm: displays for every task the stackallocation and more
//////////////////////////////////////////////////////////////////////////////
void displayAllStackData()
{
	PTASKDATA ptd = tasks; // tasks == &tasks[0]: both are addresses of first struct
	for (; ptd->taskname != NULL; ptd++)
		displayStackData(ptd->name, ptd->priority);
}



//////////////////////////////////////////////////////////////////////////////
// func: getTaskName
// args: int taskid (same as task priority)
// ret:  pointer to name of task
// comm: if you only have a taskid, and want to know the taskname
//////////////////////////////////////////////////////////////////////////////
char *getTaskName(int taskid)
{
	PTASKDATA ptd = tasks;
	for (; ptd->taskname != NULL; ptd++)
		if (taskid == ptd->priority)
			return (ptd->name);

	return (NULL);
}


//////////////////////////////////////////////////////////////////////////////
// func: hold_resume
// args: INT8U taskgroup, as: all mutextasks, or flagtasks, of mboxtasks etc
//       groups are identified with debug-out-flag
// comm: hold and resume tasks, which depends on the current task status
//       initiated by keys from mailboxtask
//       functionality of suspend/resume depends on actual task status, f.i. if
//		 a task is pending (flagpend), the OS cannot suspend the task immediately.
//////////////////////////////////////////////////////////////////////////////
void hold_resume(INT8U);
void hold_resume(INT8U taskgroup)
{
	INT8U          error;
	INT8U          taskid; // same as priority
	INT8U		   nr_tasks;
    OS_TCB         taskdata;
    int			   i;

    // i use the debux_out flags as 'taskgroups'
    switch(taskgroup)
	{
	case MAILBOX_DEBUG_OUT: 	taskid = MBOXPOSTTASK_PRTY;  nr_tasks = 2; break;
	case FLAG_DEBUG_OUT:  		taskid = FLAGPOSTTASK_PRTY;  nr_tasks = 2; break;
	case QUEUE_DEBUG_OUT:  		taskid = QUEUEPOSTTASK_PRTY; nr_tasks = 2; break;
	default: return;
	}

	// the mutextasks use a mutex, so if i get him first, i avoid possible deadlocks
	if (taskgroup == MUTEX_DEBUG_OUT) // avoid mutex deadlock
		OSMutexPend(MutexHandle, WAIT_FOREVER, &error);

	// scan thru taskgroup and suspend or resume them
	for (i=0; i< nr_tasks; i++, taskid++)
	{
		// get task status first, which is copied for me into struct taskdata
		OSTaskQuery(taskid, &taskdata);
		UART_puts("\n\r"); UART_puts(getTaskName(taskid)); UART_puts(" (task:"); UART_putint(taskid); UART_puts(") ");
		displayTaskStatus(&taskdata, TRUE);

		// if task was suspended, resume again, else suspend it
		switch(taskdata.OSTCBStat)
		{
		case OS_STAT_FLAG: 						// "pending on event flag"
		case OS_STAT_RDY:   					// "running"
			OSTaskSuspend(taskid);
			break;

		case OS_STAT_SUSPEND:  					// "suspended"
		case OS_STAT_SUSPEND | OS_STAT_FLAG:    // "suspended & pending on event flag"
			OSTaskResume(taskid);
			break;

		default: UART_puts("no hold/resume action"); break;
		}
		// get task status again, to check if it worked
		OSTaskQuery(taskid, &taskdata);
		displayTaskStatus(&taskdata, FALSE);
	}

	if (taskgroup == MUTEX_DEBUG_OUT) // free the mutex
		OSMutexPost(MutexHandle);
}


