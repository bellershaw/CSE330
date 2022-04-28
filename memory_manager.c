#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/module.h>


MODULE_LICENSE("GPL");              
MODULE_AUTHOR("Takeshita Ellershaw Payne");      
MODULE_DESCRIPTION("CSE330 Project 2");  

int contains_page(const struct mm_struct *const mm);
struct page *page_by_address(const struct mm_struct *const mm, const unsigned long address, struct vm_area_struct *vma);

//enum hrtimer_restart timer_callback( struct hrtimer *timer_for_restart );
//static int __init timer_init(void);
//static void __exit timer_exit(void);

  

static int pid = 0;// Parameter which stores the the inputted PID

static int rss = 0;
static int wss = 0;
static int swap = 0;

unsigned long timer_interval_ns = 10e9; // 10-second timer
static struct hrtimer hr_timer;

// TODO: Check param validity
module_param(pid, int, 0);// Get the inputted PID from the command line and pass it in as a parameter


// Create the task struct from the given pid
struct task_struct *processTask;
struct mm_struct *processMM; 
struct vm_area_struct *processMMap;

enum hrtimer_restart timer_callback( struct hrtimer *timer_for_restart)
{

processTask = pid_task(find_vpid(pid), PIDTYPE_PID);
processMM = processTask->mm;

// Resetting the timer, which also means… ?
ktime_t currtime , interval;
currtime = ktime_get();
interval = ktime_set(0,timer_interval_ns);
hrtimer_forward(timer_for_restart, currtime , interval);
// Do the measurement, like looking into VMA and walking through memory pages
// And also do the Kernel log printing aka printk per requirements
contains_page(processMM);
return HRTIMER_RESTART;
}

/*
static int __init timer_init(void) 
{
ktime_t ktime ;
ktime = ktime_set( 0, timer_interval_ns );
hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
hr_timer.function = &timer_callback;
hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );
return 0;
}
static void __exit timer_exit(void) {
int ret;
ret = hrtimer_cancel( &hr_timer );
if (ret) printk("The timer was still in use...\n");
printk("HR Timer module uninstalling\n");
}
*/


 /** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */

static int __init helloBBB_init(void){

printk(KERN_INFO "Opening thread\n");
printk(KERN_INFO "Pid: %d\n", pid);

ktime_t ktime ;
ktime = ktime_set( 0, timer_interval_ns );
hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
hr_timer.function = &timer_callback;
hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );

/* 
Test function that prints out the pid of all processes

struct task_struct* p;
int index = 1;
for_each_process(p)
{
printk(KERN_INFO "Process pid: %d\n", p->pid);
} 
*/

// Get the task struct from a give pid
// Testing with pid 28991



int counter = 0;
while(counter < 6)
{
timer_callback(&hr_timer);
counter++;
}


printk(KERN_INFO "RSS: %d\n", rss);
printk(KERN_INFO "WSS: %d\n", wss);
printk(KERN_INFO "SWAP: %d\n", swap);


printk(KERN_INFO "Found process for pid %d\n", processTask->pid);
printk(KERN_INFO "Process Size %d\n", processMM->task_size);

 
   return 0;
}

/*
int ptep_test_and_young(struct vm_area_struct *vma, unsigned long addr, pte_t *ptep)
{
int ret = 0;
if(pte_young(*ptep))
{
ret = test_and_clear_bit(_PAGE_BIT_ACCESSED, (unsigned long *) &ptep->pte);
}

return ret;

}
*/

struct page *page_by_address(const struct mm_struct *const mm,
                             const unsigned long address, struct vm_area_struct *vma )
{
pgd_t *pgd;
p4d_t *p4d;
pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
struct page *page = NULL;

    pgd = pgd_offset(mm, address);
    if (!pgd_present(*pgd))
        goto do_return;
        
       p4d = p4d_offset(pgd, address);
    if (!pgd_present(*pgd))
        goto do_return;

    pud = pud_offset(p4d, address);
    if (!pud_present(*pud))
        goto do_return;

    pmd = pmd_offset(pud, address);
    if (!pmd_present(*pmd))
        goto do_return;

    pte = pte_offset_kernel(pmd, address);
    if (!pte_present(*pte))
        goto do_return;

    page = pte_page(*pte);
    
    if(pte_present(*pte))
    {
    rss++;
    int ret = 0;
if(pte_young(*pte))
{
ret = test_and_clear_bit(_PAGE_BIT_ACCESSED, (unsigned long *) &pte->pte);
}
    if(ret == 1)
    {
    wss++;
    }
    
    }
    else
    {
    swap++;
    }
do_return:
    return page;
}

int contains_page(const struct mm_struct *const mm)
{
    int contains = 0;
    struct page *myPage;

    if (mm != NULL) {
        const struct vm_area_struct *vma = mm->mmap;
        while (vma != NULL) {
            unsigned long address;
            for (address = vma->vm_start; address < vma->vm_end; address += PAGE_SIZE) {
                myPage = page_by_address(mm, address, vma);
                //printk(KERN_INFO "Found page at address %d\n", address);
            }

            vma = vma->vm_next;
        }
    }

    return contains;
}


 

 

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit helloBBB_exit(void){

int ret;
ret = hrtimer_cancel( &hr_timer );
if (ret) printk("The timer was still in use...\n");
printk("HR Timer module uninstalling\n");

printk(KERN_INFO "Closing thread\n");
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(helloBBB_init);
module_exit(helloBBB_exit);
