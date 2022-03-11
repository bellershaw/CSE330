#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/init.h>
#include <linux/timekeeping.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Takeshita Ellershaw Payne");
MODULE_DESCRIPTION("Module for project 2");

//parameters
module_param(userID, int, 0);//user ID of the user running the module
module_param(buff_size, int, 0);//buffer size
module_param(num_prod, int, 0);//number of producers
module_param(num_con, int, 0);//number of consumers

//get the user id if not already initialized
if(userID == 0)
{
    userID = id -u <username>;
}

//define semaphores
struct semaphore mutex;
struct semaphore full;
struct semaphore empty;

//SEMAPHORES: 0-> No resources available
//            > 0 -> Resources available

//initialize semaphores
static incline void sema_init(struct semaphore* mutex, 0);//0 for producer, 1 is consumer
static incline void sema_init(struct semaphore* full, buff_size);//semaphor to lock producer when the buffer is full
static incline void sema_init(struct semaphore* empty, 0);//semaphor to lock consumer when the bufer is empty

//linked list
struct taskList {
    struct task_struct *head, *prev; 
}

struct buffer {
    static int capacity = buff_size;
    static taskList task_list;
}


// FOLLOWING KERNEL THREADS TUTORIAL

static int producer(void *arg)
{
/* Every kthread has a struct task_struct associated with it which is it's identifier.
* Whenever a thread is schedule for execution, the kernel sets "current" pointer to 
* it's struct task_struct.
* current->comm is the name of the command that caused creation of this thread
* current->pid is the process of currently executing thread 
*/
    // Loops through the task list once
    
    //calculate the number of tasks
    struct task_struct* producer;
    size_t process_counter = 0;
    //flag to know if we add to head
    bool flag = false;

    for_each_process(producer) 
    {    
        if (!down_interruptible(*mutex)){
            if(down_interruptible(*full){     
                int pid_val, uid_val;
                //counting the processes
                ++process_counter;
                //add to task list
                struct task_struct;
                uid_val = task->cred->uid.val; //uid of the process
                pid_val task->pid; //pid of the process
                if(userID == uid_val) //if the process uid and user uid are equal add to buffer  
                {
                    //add to shared buffer
                    //this condition checks no task_structs have been inserted
                    if(flag == false)
                    {
                        buffer.task_list->head = task;
                        buffer.task_list->prev = NULL;
                        flag = true;
                    }
                    else
                    {
                        buffer.task_list->prev = buffer.task_list->head;
                        buffer.task_list->head = task;
                    }
                }
                
                printk(KERN_INFO "Full: %d \n", full);
                up(*empty);
                printk(KERN_INFO "Empty: %d \n", empty);
            break;
            }
            else{
            up(*mutex);
            }
        }
    } 
   
    printk(KERN_INFO "I am thread: %s[PID = %d]\n", current->comm, current->pid);
    return 0;
}

static int consumer(void *arg)
{
/* Every kthread has a struct task_struct associated with it which is it's identifier.
* Whenever a thread is schedule for execution, the kernel sets "current" pointer to 
* it's struct task_struct.
* current->comm is the name of the command that caused creation of this thread
* current->pid is the process of currently executing thread 
*/
    while(!kthread_should_stop()){

        // Define 
        if(down_interruptible(*mutex)){
            if(down_interruptible(*empty)){
                //pull from stack
                //time it
                //print time
                up(*full);
            }
        }
    }
    printk(KERN_INFO "I am thread: %s[PID = %d]\n", current->comm, current->pid);
    return 0;
}

static int __init init_func(void)
{
    struct task_struct *producer;
    struct task_struct *consumer;
    int err;

    printk(KERN_INFO "Starting 2 threads\n");

/*struct task_struct *kthread_create(int (*threadfn)(void *data), void *data, \
 *                         const char *namefmt, ...);
 * This function creates a kernel thread and starts the thread.
 */

    // If there are no producers, return a message saying that no tasks can be produced

    if(num_pro != 0){
        pts = kthread_run(producer, NULL, "thread-producer");
            if (IS_ERR(pts)) {
                printk(KERN_INFO "ERROR: Cannot create thread producer\n");
                err = PTR_ERR(pts);
                pts = NULL;
                return err;
            }
    }else{
        printk(KERN_INFO "No producers have been introduced!");
        return 0;
    }
    


    // Create the specified number of consumers
    for(int i = 0; i < num_con; i++){
            cts = kthread_run(consumer, NULL, "thread-consumer");
        if (IS_ERR(cts)) {
            printk(KERN_INFO "ERROR: Cannot create thread consumer\n");
            err = PTR_ERR(cts);
            cts = NULL;
            return err;
            }
    }
  

    printk(KERN_INFO "I am thread: %s[PID = %d]\n", current->comm, current->pid);
    return 0;
}

static void __exit exit_func(void)
{
    printk(KERN_INFO "Exiting the module\n");
}

module_init(init_func);
module_exit(exit_func);