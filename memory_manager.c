#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/module.h>


MODULE_LICENSE("GPL");              
MODULE_AUTHOR("Takeshita Ellershaw Payne");      
MODULE_DESCRIPTION("CSE330 Project 3");  
MODULE_VERSION("0.1");///The version of the module
    
// Initialize params to make it compile    
static int pid = 0;// Parameter which stores the the inputted PID

static int rss = 0;
static int wss = 0;
static int swap = 0;

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

unsigned long timer_interval_ns = 10e9; // 10-second timer
static struct hrtimer hr_timer;

enum hrtimer_restart timer_callback( struct hrtimer *timer_for_restart )
{
	// Resetting the timer, which also means… ?
  	ktime_t currtime , interval;
  	currtime  = ktime_get();
  	interval = ktime_set(0,timer_interval_ns); 
  	hrtimer_forward(timer_for_restart, currtime , interval);

	// Do the measurement, like looking into VMA and walking through memory pages
// And also do the Kernel log printing aka printk per requirements

	return HRTIMER_RESTART;
}

static int __init timer_init(void) {
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


struct page *page_by_address(const struct mm_struct *const mm,
                             const unsigned long address)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    struct page *page = NULL;

    pgd = pgd_offset(mm, address);
    if (!pgd_present(*pgd))
        goto do_return;

    pud = pud_offset(pgd, address);
    if (!pud_present(*pud))
        goto do_return;

    pmd = pmd_offset(pud, address);
    if (!pmd_present(*pmd))
        goto do_return;

    pte = pte_offset_kernel(pmd, address);
    if (!pte_present(*pte))
        goto do_return;

    page = pte_page(*pte);
do_return:
    return page;
}

int contains_page(const struct mm_struct *const mm, struct page *const page)
{
    int contains = 0;

    if (mm != NULL) {
        const struct vm_area_struct *vma = mm->mmap;
        while (vma != NULL) {
            unsigned long address;
            for (address = vma->vm_start; !contains && address < vma->vm_end; address += PAGE_SIZE) {
                contains = (page_by_address(mm, address) == page);
            }

            vma = vma->vm_next;
        }
    }

    return contains;
}



/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(helloBBB_init);
module_exit(helloBBB_exit);

module_init(timer_init);
module_exit(timer_exit);
