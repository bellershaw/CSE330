#include <linux/init.h>           
#include <linux/kernel.h>    
#include <linux/sched/signal.h>
#include <linux/kthread.h>
#include <linux/init.h>
#include <linux/timekeeping.h>
#include <linux/module.h>       
#include <linux/ktime.h> 
#include <linux/slab.h>
#include <linux/mm_types.h>
#include <linux/hrtimer.h>

MODULE_LICENSE("GPL");              
MODULE_AUTHOR("Takeshita Ellershaw Payne");      
MODULE_DESCRIPTION("CSE330 Project 3");  
MODULE_VERSION("0.1");///The version of the module
    
// Initialize params to make it compile    
static int pid = 0;// Parameter which stores the the inputted PID

// TODO: Check param validity
module_param(pid, int, 0);// Get the inputted PID from the command line and pass it in as a parameter


static int __init helloBBB_init(void)
{

    
    return 0;
}
 
 

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit helloBBB_exit(void)
{

}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(helloBBB_init);
module_exit(helloBBB_exit);
