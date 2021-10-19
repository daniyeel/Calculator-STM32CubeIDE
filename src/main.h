//////////////////////////////////////////////////////////////////////////////
// prog: main.h
// comm: external definitions and prototypes for main
// auth: MSC
//////////////////////////////////////////////////////////////////////////////

// debug output stuff
#define OUTPUT_DEBUG

extern int Uart_debug_out; // bitmask-toggle key for task-debug-output
#define MUTEX_DEBUG_OUT   	0x01 // bit 1 used to toggle mutex-task output
#define MAILBOX_DEBUG_OUT 	0x02 // bit 2 used to toggle mbox-task output
#define FLAG_DEBUG_OUT    	0x04 // bit 3 used to toggle flag-task output
#define QUEUE_DEBUG_OUT   	0x08 // bit 4 used to toggle queue-task output
#define INTERRUPT_DEBUG_OUT 0x10 // bit 5 used to toggle interrupt-task output


#define WAIT_FOREVER        0   // to force all OSPends (flag, mutex, mbox etc.) to wait for incoming signal
#define LOOP_DELAY          60  // standard delay time for tasks
#define LED_DELAY           50  // to slow leds down
#define QSIZE               4   // 4 q-members


// priorities for all tasks and prty of mutex-semaphore itself
enum {
    INITTASK_PRTY = 5,

	MBOXPOSTTASK_PRTY,
    MBOXPENDTASK_PRTY,

	INPUTFLAGPOST_PRTY,
    INPUTFIRST_PRTY,
    INPUTFLAGPEND_PRTY,
    INPUTSECOND_PRTY,

    FLAGPOSTTASK_PRTY = 22,
    FLAGPENDTASK_PRTY,

    QUEUEPOSTTASK_PRTY,
    QUEUEPENDTASK_PRTY,

    DUMMY
};


// handles, used for semaphore, mailbox, flag, queue
// defined as a pointer; the os will allocate an OS_EVENT on it
extern OS_EVENT    *MutexHandle;
extern OS_EVENT    *MboxHandle;
extern OS_FLAG_GRP *FlagHandle;
extern OS_FLAG_GRP *FlagIntHandle;
extern OS_EVENT    *QueueHandle;

// q structure
typedef struct queue
{
     int   firstNumber;   // first input calculator
     int   value;         // math operator
     int   secondNumber;  // second input calculator
     int   result;        // result
     char  text[17];      // text to display
} Q, *PQ;

extern Q     data_queue[]; // data queue, in this case array of Q-structs
extern void* os_queue[];   // pointer queue for OS


// function prototypes for various tasks to prevent compiler warnings
extern void InitTask      (void *pdata);
extern void InitMutex     (void);
extern void DisplayOSData (void);
extern void CreateHandles (void);
extern void displayAllStackData(void);
extern void hold_resume   (INT8U);

// function prototypes of threads/tasks/processes to prevent compiler warnings
extern void MboxPostTask  (void *pdata);
extern void MboxPendTask  (void *pdata);
extern void QueuePostTask (void *pdata);
extern void InputQueuePendTask (void *pdata);
extern void InterruptTask (void *pdata);


// functions used for the calculator
extern void InputFlagPost  (void *pdata);
extern void InputFirst	   (void *pdata);
extern void InputSecond	   (INT16U firstNumber, OS_FLAGS value);
extern void InputFlagPend  (INT16U firstNumber, OS_FLAGS value);

// variables for queue (see queue.c)
extern int firNum;
extern int oper;
extern int secNum;
extern int res;
