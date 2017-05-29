/*
 *  hello-2.c - Demonstrating the module_init() and module_exit() macros.
 *  This is preferred over using init_module() and cleanup_module().
 */

#include <linux/kernel.h>   
#include <linux/module.h>
#include <asm/uaccess.h>
MODULE_LICENSE("GPL");
/* Zbiory nag³ówkowe dla urz±dzeñ znakowych */
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <asm/uaccess.h>  /* for put_user */

#define SUCCESS 0
#define DEVICE_NAME "giepeio"
#define BUF_SIZE 50

// Char bedacy posrednikiem miedzy user space a sterownikiem GPIO
char mediate[BUF_SIZE];

// Wartosc pokazywana na pinach GPIO
unsigned char gpio_val;

// Wartosc GPIO przemapowana w pamieci na odpowiedni adres (0x41200000)
unsigned char *fmem;

// Rzeczy potrzebne dla rejestracji urzadzenia w systemie plikow
dev_t my_dev=0;
struct cdev *my_cdev = NULL;
static struct class *my_class = NULL;

// Handler gdy wywolany jest write z user space
ssize_t my_write(struct file *filp,
	const char __user *buf,size_t count, loff_t *off)
{
  // Retcode
  int res, gpio_temp;

  /* Sprawdzamy, czy jest miejsce na urządzeniu */
  if (count == 0) return -ENOSPC;

  // Tak, ale trzeba to zapisac do pamieci teraz
  __copy_from_user(mediate,buf,count);

  // Zamiana na wartosc int
  res = kstrtoint(mediate, 10, &gpio_temp);
  if(res != 0){
  	if(res == -ERANGE){
  		printk(KERN_ALERT "Write overflow!\n");
  		return -EOVERFLOW;
  	}
  	if(res == -EINVAL){
  		printk(KERN_ALERT "Incorrect input!\n");
  		return -EINVAL;
  	}
  	// Nie wiadomo jaki blad
  	return -1;
  }

  // Sprawdzenie przekroczenia maksymalnej wartosci
  if(gpio_temp < 0 || gpio_temp > 255){
	printk(KERN_ALERT "Incorrect value!\n");
	return -EINVAL;
  }

  // Przerzutowanie
  gpio_val = (unsigned char)gpio_temp;
  
  printk(KERN_INFO "Otrzymano liczbe %c", gpio_val);

  // TODO: ZAPIS WARTOSCI DO POZYCJI W PAMIECI
  *fmem = gpio_val;

  printk(KERN_INFO "W pamieci zapisana jest wartosc %c\n", *fmem);

  return count;
}	

// Handler gdy wywolany jest read z user space
ssize_t my_read(struct file *filp,
	char __user *buf,size_t count, loff_t *off)

{
  if (count == 0) return 0;

  sprintf(mediate,"%d\n",(int)gpio_val);

  __copy_to_user(buf,mediate,count);

  return count;
}	

// Handler gdy wywolany jest open z user space
// POWINNO BYC ZABEZPIECZENIE PRZED WIELOKROTNYM OTWIERANIEM!!!
static int my_open(struct inode *inode, struct file *file)
{
  return SUCCESS;
}

// Handler gdy wywolany jest read z user space
static int my_release(struct inode *inode, struct file *file)
{
  return SUCCESS;
}

// Struktura przechowujaca informacje o tym handlerze
struct file_operations fops = {
  .owner = THIS_MODULE,
  .read=my_read,
  .write=my_write,
  .open=my_open,
  .release=my_release,
};

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye, world!\n");
	// Usuwamy urzadzenie z klasy
	if(my_dev && my_class){
		device_destroy(my_class, my_dev);
	}
	if(my_cdev){
		cdev_del(my_cdev);
	}
	my_cdev=NULL;

	// Zwalniamy numer urzadzenia
	unregister_chrdev_region(my_dev, 1);

	if(fmem) {
		iounmap(fmem);
	}

	// Wyrejestrowujemy klase
	if(my_class){
		class_destroy(my_class);
		my_class = NULL;
	}
}

static int __init hello_init(void)
{
	int res;
	printk(KERN_INFO "Hello, world!\n");

	// Tworzymy klase opisujaca nasze urzadzenie - potrzebne dla udev

	my_class = class_create(THIS_MODULE, "my_class");
	if (IS_ERR(my_class)) {
		printk(KERN_ERR "Error creating my_class class.\n");
		res = PTR_ERR(my_class);
		goto err1;
	}

	// Alokujemy numer urzadzenia
	res = alloc_chrdev_region(&my_dev, 0, 1, DEVICE_NAME);
	if(res){
		printk(KERN_INFO "<1>Allocation of the device called %s failed\n", DEVICE_NAME);
		goto err1;
	}
	
	my_cdev = cdev_alloc();
	if(my_cdev==NULL){
		printk(KERN_INFO "<1>Allocation of cdev for %s failed\n", DEVICE_NAME);
		res = -ENODEV;
		goto err1;
	}

	my_cdev->ops = &fops;
	my_cdev->owner = THIS_MODULE;

	// Dodajemy urzadzenie znakowe do systemu
	res = cdev_add(my_cdev, my_dev, 1);
	if(res){
		printk(KERN_ERR "Registration of the device number for %s failed\n", DEVICE_NAME);
		goto err1;
	}

	fmem = ioremap(0x41200000, sizeof(unsigned char));
	
	// Tworzymy nasze urzadzenie
	device_create(my_class, NULL, my_dev, NULL, "my_dev%d", MINOR(my_dev));
	printk(KERN_ALERT "Registration is a success. The major device %s is %d", DEVICE_NAME, MAJOR(my_dev));
	return SUCCESS;

err1:
	hello_exit();
	return res;
}


module_init(hello_init);
module_exit(hello_exit);
