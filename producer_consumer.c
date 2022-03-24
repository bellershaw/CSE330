#include <linux/init.h>           
#include <linux/kernel.h>    
#include <linux/sched/signal.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/init.h>
#include <linux/timekeeping.h>
#include <linux/module.h>       
#include <linux/ktime.h> 
#include <linux/slab.h>
 
MODULE_LICENSE("GPL");              
MODULE_AUTHOR("Takeshita Ellershaw Payne");      
MODULE_DESCRIPTION("CSE330 Project 2");  
MODULE_VERSION("0.1");              ///< The version of the module
    
// Initialize params to make it compile    
static int UID = 0;
static int buff_size = 0;
static int num_producers = 0;
static int num_consumers = 0;

// TODO: Check param validity
module_param(UID, int, 0);  // Parameter which stores the the user ID of a given user
module_param(buff_size, int, 0); // Parameter which stores the size of the buffer, must be a positive int
module_param(num_producers, int, 0); // Parameter which stores the number of producer threads, must be 1 or 0
module_param(num_consumers, int, 0); // Parameter which stores the number of consumer threads, a non-negative int)

// Declare producer and consumer functions
static int consumer_func(void *arg);
static int producer_func(void *arg);

// Declare global pointer of consumer functions so all consumer threads can be stopped
struct task_struct *consumer;//was consumer_ptr

// Semaphore declaration
struct semaphore empty;
struct semaphore full;
struct semaphore mutex;

// Linked List declaration 
struct task_node
{
	struct task_struct *task;
	struct task_node *next;

} empty_task_list;

// Buffer struct declaration
struct buff_data
{
	int capacity;
	struct task_node *head;
} buff_data;



// Define a global int to track total task time
long total_task_time;

 /** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */

static int __init helloBBB_init(void)
{

    //printk(KERN_INFO "Opening thread\n");

    // Initialize semaphores
    // If Semaphore > 0: Runs
    // If Semaphore = 0: Does not run
    sema_init(&empty, buff_size);
    sema_init(&full, 0); 
    sema_init(&mutex, 1);

    //printk(KERN_INFO "Buffsize: %d\n", buff_size);


    // Set initial values for global buffer


    empty_task_list.task = NULL;
    empty_task_list.next = NULL;

    buff_data.capacity = buff_size;
    buff_data.head = &empty_task_list;

    // Make producer and consumer threads 
    struct task_struct *producer;
    //struct task_struct *consumer;
    int err;



    // Start consumer threads
    //printk(KERN_INFO "Starting consumers\n");
    consumer = kthread_run(consumer_func, NULL, "consumer");
    if (IS_ERR(consumer))
    {
        printk(KERN_INFO "ERROR: Cannot create thread consumer\n");
        err = PTR_ERR(consumer);
        consumer = NULL;
        return err;
    }




    // Have the get the value of the consumer threads so the producer can stop them later
    //consumer_ptr = consumer;
    //printk(KERN_INFO "Consumer pointer set to %p\n", consumer_ptr);
    //printk(KERN_INFO "Consumer set to %p\n", consumer);


    // Start the producer thread
    //printk(KERN_INFO "Starting producer. \n");
    producer = kthread_run(producer_func, NULL, "producer");
    if (IS_ERR(producer))
    {
        printk(KERN_INFO "ERROR: Cannot create thread producer\n");
        err = PTR_ERR(producer);
        producer = NULL;
        return err;
    }



    
    return 0;
}
 
static int producer_func(void *arg)
{

    // Show the producer thread is up and running 
    //printk(KERN_INFO "I am thread: %s[PID = %d]\n", current->comm, current->pid);

    //printk("1\n");

    struct task_struct* p;
    size_t process_counter=0;
    int index = 1;
    for_each_process(p)
    {
        if(p->cred->uid.val == UID && process_counter < buff_size)
        {
            // Loop until empty nodes exist in the buffer
            if(down_interruptible(&empty)) 
            {
                break;
            }

            if(down_interruptible(&mutex)) 
            {
                break;
            }

            // Add new process to the tail of the linked list
            //printk(KERN_INFO "Entered Producer. Empty: %d\n", empty.count);

            struct task_node* newNode; 
            newNode = kmalloc(sizeof (struct task_node), GFP_KERNEL);

            if(!newNode)
            {
                printk(KERN_INFO "allocated memory wrong");
            }

            newNode->task = p;
            newNode->next = NULL;


            //add to empty list
            if(buff_data.head == &empty_task_list)
            {
                buff_data.head = newNode;
                //printk(KERN_INFO "Added New Task node to head\n");
            } 
            else //add to list containing nodes
            {
                struct task_node *temp = buff_data.head;

                while(temp->next != NULL)
                {
                    temp = temp->next;
                }

                temp->next = newNode;

            }



            ++process_counter;

            printk(KERN_INFO "[%s] Produced Item#-%d at buffer index:%d for PID:%d", current->comm, p->cred->uid.val, index,  p->pid);
	    index++;



            // Increment the mutex
            up(&mutex);

            // Increment the full semaphore
            up(&full); 

            //printk(KERN_INFO "Exited producer.  Full: %d\n", full.count);
        }



        // Exit the consumer pointer
        /*
        if(consumer_ptr == NULL){
        printk(KERN_INFO "Error: Consumer pointer is null.");
        }else{
        kthread_stop(consumer_ptr);
        }
        */

        //printk("Producer Finished\n"); 

    }

    return 0;

}

static int consumer_func(void *arg)
{

    //printk(KERN_INFO "I am thread: %s[PID = %d]\n", current->comm, current->pid);

    int counter = 0, index = 1;
    //breaks if kthread_stop() is called
    while(counter < buff_size)//while(1)
    {
	/*if(kthread_should_stop())
	{
		return 0;
	}*/
        // Loop until full nodes exist in the buffer
        if(down_interruptible(&full)) 
        {
            break;
        }

        // Loop until mutex is available
        if(down_interruptible(&mutex)) 
        {
            break;
        }

        //printk(KERN_INFO "Entered Consumer.  Full: %d", full.count);

        // Remove the head of the linked list
        if(buff_data.head == NULL)
        {
            // This only happens if the semaphores aren't working right
            printk(KERN_INFO "Consumer error: attempting to consume from empty buffer");
            //printk(KERN_INFO "Full: %d", full.count);
           // kthread_stop(consumer);
        }
        else
        {

            // Remove task node from the buffer 
            // TODO: Fix sloppy memory management 
            //printk(KERN_INFO "Debug: -1\n");
            struct task_node *temp;
            //printk(KERN_INFO "Debug: 0\n");

            temp = buff_data.head;
            if(temp->next != NULL)
            {
                //temp = buff_data.head;
                //printk(KERN_INFO "Debug 2 \n");
                buff_data.head = buff_data.head->next;
            }
            else
            {
                printk(KERN_INFO "Debug2 : Next is Null\n");
                //kthread_stop(consumer);
            }

            //printk(KERN_INFO "Debug: 3\n");

            //printk(KERN_INFO "1\n");
            long start_time = temp->task->start_time;
            //printk(KERN_INFO "2\n");
            //printk("Start Time: %ld\n", start_time);
            //printk(KERN_INFO "3\n");
            long current_time = ktime_get_ns();
            //printk(KERN_INFO "4\n");
            //printk(KERN_INFO "Current Time: %ld\n", current_time);
            //printk(KERN_INFO "5\n");
            long task_time = current_time - start_time;
	    int seconds = task_time/1000000000;
	    //printk(KERN_INFO "sec: %d\n",seconds);
	    int minutes = seconds/60;
	    //printk(KERN_INFO "min: %d\n",minutes);
	    int hours = minutes/60;
	    //printk(KERN_INFO "hours: %d\n",hours);
            //printk(KERN_INFO "6\n");
            //printk(KERN_INFO "Task Time: %ld\n", task_time);
            //printk(KERN_INFO "7\n");
            total_task_time += task_time;
            //printk(KERN_INFO "8\n");

            printk(KERN_INFO "[%s] consumed Item#-%d on buffer index: %d PID:%d Elapsed Time- %d:%d:%d\n", current->comm, temp->task->cred->uid.val, 0, temp->task->pid, hours, minutes, seconds);


        

        }

        counter++;




        // Increment the mutex
        up(&mutex);

        up(&empty);

        //printk(KERN_INFO "Exited Consumer.  Empty: %d\n", empty.count);


        // Increment the empty semaphore

        // Find the task time of the process and increment total task time 
        // QUESTION: DOES THE PROCESS BEGIN ONCE I RETREIVE THE START TIME?
        // TODO: Find right data format for timekeeping

        /*int start_time = temp->task->start_time;
        int current_time = ktime_get_ns();
        int task_time = current_time-start_time;
        total_task_time += task_time;
        printk("%s consumed Item#-%d on buffer index: %d PID:%d Elapsed Time- %d", current->comm, 0, temp->task->pid, task_time);*/




    }
    return 0;


}
 

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit helloBBB_exit(void)
{
	//kthread_stop(consumer_ptr);
	int seconds = total_task_time/1000000000;
	int minutes = seconds/60;
	int hours = minutes/60;
    printk(KERN_INFO "The total elapsed time of all processes for UID %d is %d:%d:%d\n", UID, hours, seconds, minutes);
    //printk(KERN_INFO "Closing thread\n");
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(helloBBB_init);
module_exit(helloBBB_exit);
