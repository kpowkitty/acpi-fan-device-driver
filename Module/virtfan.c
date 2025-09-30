/**************************************************************
* GitHub-Name:: kpowkitty
*
* File:: virtfan.c
*
* Description:: A device driver for a virtualized ACPI 
* device tree -- FAN. Grabs all current FAN handles for a user
* to open specifically under a file pointer and query whether
* it is on or off with read(), turn it on or off with write(),
* and ioctl functions to query the Fan Performance State level,
* it's available Fan Performance State packages, and set
* its current Fan Performance State level.
*
**************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/acpi.h>
#include <acpi/acpi_bus.h>
#include <acpi/acpi.h>

#define DEVICE_NAME "virtfans"
#define NUM_OF_FANS 4
#define NUM_OF_STATES 5
#define SUCCESS 0
#define FAILURE -1

#define FAN_IOC_MAGIC 'f'

#define FAN_IOC_GET_FST _IOR(FAN_IOC_MAGIC, 0, struct fan_fst)
#define FAN_IOC_GET_FPSP _IOR(FAN_IOC_MAGIC, 0, struct fsp_table)
#define FAN_IOC_SET_FSL _IOW(FAN_IOC_MAGIC, 1, __u32)

/* Character device */
static struct cdev virtfan_cdev;

/* This device's unique identifier. */
static dev_t dev_number;

/**
 * For holding the acpi_handles for each of the fans.
 */
static acpi_handle *fan_handles;

/**
 * Structure for holding a fan's flattened _FST package.
 */
struct fan_fst {
	__u32 revision; /* revision number */
	__u32 control; /* fan state level */
	__u32 speed; /* fan speed */
};

/**
 * Structure for holding a fan's flattened _FSP package.
 */
struct fan_fsp {
	__u32 level; /* fan state level */
	__u32 trip_point; /* temperature to turn on the fan */
	__u32 speed; /* fan speed */
	__u32 noise_level; /* noise level of fan - optional */
	__u32 power; /* power draw of fan - optional */
};

/**
 * Table for holding an array of fan's flattened _FSP packages.
 */
struct fsp_table {
	__u32 size; /* num of states */
	struct fan_fsp fsp[NUM_OF_STATES]; /* array of fan's 
					      flattened _FSP packages */
};

/**
 * Destructor for our fan_handle object.
 */
int destroy_fan_handles(void) {
	if (fan_handles == NULL) {
		pr_info("fan_handles was NULL.");
		return SUCCESS;
	}

	vfree(fan_handles);
	fan_handles = NULL;

	return SUCCESS;
}

/** FILE OPERATIONS **/

/**
 * Opens the desired FAN#.
 */
static int virtfan_open(struct inode *inode, struct file *file) 
{
	/* Get the fan number. 0 - 3 are the only possible choices. */
	int fan_index = iminor(inode);
	if (fan_index > 3 || fan_index < 0) {
		pr_err("Out of bounds fan_index: %d.", fan_index);
		return -EINVAL;
	}

	pr_info("Opening fan device %d.\n", fan_index);

	/* Give the valid fan_handle to the user. */
	if (fan_handles) {
		file->private_data = fan_handles[fan_index];
	} else {
		pr_err("fan_handles uninitialized.");
		return -ENOMEM;
	}
	
	return SUCCESS;
}

/**
 * Releases the desired FAN#.
 */
static int virtfan_release(struct inode *inode, struct file *file) 
{
	/* Get the fan number. 0 - 3 are the only possible choices. */
        int fan_index = iminor(inode);
        if (fan_index > 3 || fan_index < 0) {
                pr_err("Out of bounds fan_index: %d.", fan_index);
                return -EINVAL;
        }

        pr_info("Releasing fan device %d.\n", fan_index);

	/* Set it to NULL. */
        file->private_data = NULL;
        
        return SUCCESS;
}

/**
 * For reading the current _PCS of a specified fan.
 */
static ssize_t virtfan_read(struct file *file, char __user *buf, size_t size, loff_t *offset) 
{
	union acpi_object *result;
	struct acpi_buffer ret = { ACPI_ALLOCATE_BUFFER, NULL };
	acpi_integer fan_state;
	
	/**
	 * uses acpi to read it's _PSC method, which tells whether the specific
	 * fan is _ON or _OFF.
	 */
	pr_info("file->private_data: %p.", file->private_data);
	acpi_status status = acpi_evaluate_object(file->private_data, "_PSC", NULL, &ret);
	if (ACPI_FAILURE(status)) {
		pr_err("Cannot evaluate object \"%p\": acpi_status: %s.\n",
				file->private_data, acpi_format_exception(status));
		if (ret.pointer) {
			ACPI_FREE(ret.pointer);
		}
		destroy_fan_handles();
		return -EINVAL;
	}

	/**
	 * ensures the result was valid.
	 */
	pr_info("Checking result...");
	result = (union acpi_object *)ret.pointer;
	if (result && result->type == ACPI_TYPE_INTEGER) {
		pr_info("virtfan: Fan %p status: 0x%llx.\n", file->private_data,
			(unsigned long long)result->integer.value);
		fan_state = result->integer.value;
	} else {
		pr_err("_PSC did not return an integer.\n");
		destroy_fan_handles();
		if (ret.pointer) {
			ACPI_FREE(ret.pointer);
		}
		return FAILURE;
	}

	/* Done using ret, free it. */
	if (ret.pointer) {
		ACPI_FREE(ret.pointer);
	}

	/* Copy the info to user. */
	pr_info("Size: %zu.", size);
	if (size >= 1) {
		pr_info("fan_state: %llu.", fan_state);
		if (copy_to_user(buf, &fan_state, sizeof(fan_state)) != 0) {
			pr_err("Failed to copy fan state to user buffer.\n");
			return -EFAULT;
		}
		pr_info("Successful copy_to_user.");
	}

	pr_info("Successful read.");
	return sizeof(fan_state);
}

/**
 * For changing the current ON/OFF state of the opened FAN#.
 */
static ssize_t virtfan_write(struct file *file, const char __user *buf, size_t size, loff_t *offset) 
{
	acpi_status status;
	acpi_handle fan_handle = file->private_data;
	acpi_integer new_state;

	/* It should ONLY be sizeof(acpi_integer). */
	if (size != sizeof(acpi_integer)) {
		pr_err("virtfan: Input size %zu incorrect.", size);
		return -EINVAL;
	}

	/* Copy desired state from user. */
	if (copy_from_user(&new_state, buf, sizeof(acpi_integer)) != 0) {
		pr_err("virtfan: Failed to copy user input.");
		return -EFAULT;
	}

	pr_info("virtfan: Requested new fan state: %llu.", (unsigned long long)new_state);
	
	/* Prepare method string with dynamic state. */
	char *method = (new_state == 0) ? "_OFF" : "_ON";
	status = acpi_evaluate_object(fan_handle, method, NULL, NULL);
	if (ACPI_FAILURE(status)) {
		pr_err("virtfan: Failed to set fan state via %s.", acpi_format_exception(status));
	}

	pr_info("virtfan: Fan state changed succesfully.");
	return sizeof(acpi_integer);
}

/**
 * For the user to change the Fan Performance State, query what Fan Performance States
 * are available, and get the current Fan Performance State that's set. 
 */
static long virtfan_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	acpi_status status;
	union acpi_object *result;
	acpi_integer fan_level;
	struct acpi_buffer ret = { ACPI_ALLOCATE_BUFFER, NULL };
	struct fsp_table curr_fpsp = {0};
	struct fan_fst curr_fst;
	int fan_index = iminor(file_inode(file));
	if (fan_index < 0 || fan_index > 3) {
		pr_err("Invalid fan index: %d\n", fan_index);
		return -EINVAL;
	}
	acpi_handle curr_handle = fan_handles[fan_index];

	switch (cmd) {
		case FAN_IOC_GET_FST:
			/**
			 * uses acpi to get the current Fan Status. This object returns a package
			 * containing revision, control (fan performance level), and 
			 * the speed of the fan.
			 */
			status = acpi_evaluate_object(curr_handle, "_FST", NULL, &ret);
			if (ACPI_FAILURE(status)) {
				pr_err("Cannot evaluate object \"%p\": acpi_status: %s.\n",
						file->private_data, acpi_format_exception(status));
				if (ret.pointer) {
					ACPI_FREE(ret.pointer);
				}
				destroy_fan_handles();
				return -EINVAL;
			}

			pr_info("Checking result...");

			/* validates the result. */
			result = (union acpi_object *)ret.pointer;
			if (result && result->type == ACPI_TYPE_PACKAGE) {
				pr_info("virtfan: Fan %p successful _FST returned.\n",
						file->private_data);
			} else {
				pr_err("_FST did not return a package.\n");
				destroy_fan_handles();
				if (ret.pointer) {
					ACPI_FREE(ret.pointer);
				}
				return FAILURE;
			}

			/* prepare flattened structured for copying to user. */
			curr_fst.revision = result->package.elements[0].integer.value;
			curr_fst.control = result->package.elements[1].integer.value;
			curr_fst.speed = result->package.elements[2].integer.value;

			/* copy to user. */
			if (copy_to_user((void __user *)arg, &curr_fst, sizeof(curr_fst)) != 0) {
				pr_err("Failed to copy_to_user.");
				return -EINVAL;
			}
			
			/* cleanup. */
			if (ret.pointer) {
				ACPI_FREE(ret.pointer);
			}
			
			return SUCCESS;
		
		case FAN_IOC_SET_FSL:
			/* get the desired fan state level from the user. */
			if (copy_from_user(&fan_level, (void __user *)arg,
				sizeof(acpi_integer)) != 0) {
				pr_err("Failed to copy_from_user.");
				return -EINVAL;
			}

			/* prepare it as an acpi_object_list for acpi_evaluate_object. */
			union acpi_object arg_obj;
			arg_obj.type = ACPI_TYPE_INTEGER;
			arg_obj.integer.value = fan_level;

			struct acpi_object_list param_objs;
			param_objs.count = 1;
			param_objs.pointer = &arg_obj;

			/* call acpi to change the fan state level. */
			status = acpi_evaluate_object(curr_handle, "_FSL", &param_objs, NULL);
			if (ACPI_FAILURE(status)) {
				pr_err("Cannot evaluate object \"%p\": acpi_status: %s.\n",
						file->private_data, acpi_format_exception(status));
				destroy_fan_handles();
				return -EINVAL;
			}

			/* no cleanup needed. */
			return SUCCESS;
		
		case FAN_IOC_GET_FPSP:
			/**
			 * get the current fan_handle's FPSP list. This list contains
			 * every Fan Performance State package available to the current
			 * fan handle. 
			 */
			status = acpi_evaluate_object(curr_handle, "_FPS", NULL, &ret);
			if (ACPI_FAILURE(status)) {
				pr_err("Cannot evaluate object \"%p\": acpi_status: %s.\n",
						file->private_data, acpi_format_exception(status));
				if (ret.pointer) {
					ACPI_FREE(ret.pointer);
				}
				destroy_fan_handles();
				return -EINVAL;
			}

			pr_info("Checking result...");
			
			/**
			 * get the result from acpi_evaluate_object; it should
			 * contain a pointer to our FPSP packages.
			 */
			result = (union acpi_object *)ret.pointer;
			if (result && result->type == ACPI_TYPE_PACKAGE) {
				pr_info("virtfan: Fan %p successful _FPS found.\n",
						file->private_data);
			} else {
				pr_err("_FPS did not return a package.\n");
				destroy_fan_handles();
				if (ret.pointer) {
					ACPI_FREE(ret.pointer);
				}
				return FAILURE;
			}

			curr_fpsp.size = NUM_OF_STATES;

			/* Start from 1, because 0 is the revision number */
			for (__u32 i = 1; i <= curr_fpsp.size; ++i) {
				/* grab the current package */
				union acpi_object *pkg = &result->package.elements[i];
				
				if (pkg->type != ACPI_TYPE_PACKAGE) {
					pr_err("Invalid pkg->type.");
					return -EINVAL;
				}

				/* grab the current package's elements */
				union acpi_object *elems = pkg->package.elements;
				pr_info("elems[4].integer.value: %llu.", elems[4].integer.value);	
				curr_fpsp.fsp[i - 1].level = elems[0].integer.value;
				curr_fpsp.fsp[i - 1].trip_point = elems[1].integer.value;
				curr_fpsp.fsp[i - 1].speed = elems[2].integer.value;
				curr_fpsp.fsp[i - 1].noise_level = elems[3].integer.value;
				curr_fpsp.fsp[i - 1].power = elems[4].integer.value;
			}

			/* pass it over to the user */
			if (copy_to_user((void __user *)arg, &curr_fpsp, 
				sizeof(struct fsp_table)) != 0) {
				pr_err("Failed to copy to user.");
				return -ENOMEM;
			}
			
			/* cleanup */
			if (ret.pointer) {
				ACPI_FREE(ret.pointer);
			}

			return SUCCESS;

		default:
			/* if we've reached this point, the user did not
			 * pass a correct ioctl cmd. */
			pr_err("Incorrect ioctl cmd passed.");
			return -ENOTTY;
	}
}

/**
 * The structure for our defined file_operations.
 */
static const struct file_operations virtfan_fops = {
	.owner = THIS_MODULE,
	.open = virtfan_open,
	.release = virtfan_release,
	.write = virtfan_write,
	.read = virtfan_read,
	.unlocked_ioctl = virtfan_ioctl,
};

/**
 * The class struct for our device driver.
 */
static struct class *virtfan_class;

/**
 * The device struct for our device driver.
 */
static struct device *virtfan_device;

/**
 * Initialization routine for this device driver.
 *
 * Populates our acpi_handles to later be used for reading and writing.
 *
 * @note This operates under the assumption there are
 * four fans (and errors if we cannot load them).
 */
static int __init virtfan_load(void)
{
	pr_info("virtfan: init function entered\n");
	int ret;
	size_t fan_path_size = 11; // 10 + 1 for null terminator
	char current_fan[11];
	acpi_status status;

	fan_handles = vmalloc(sizeof(acpi_handle) * NUM_OF_FANS);
	if (fan_handles == NULL) {
		pr_err("Failed to initialize fan_handles.");
		return ENOMEM;
	}

	/* Grab each FAN# handle. */
	for (int i = 0; i < NUM_OF_FANS; ++i) {
		pr_info("virtfan: load loop idx %d.\n", i);
		snprintf(current_fan, fan_path_size, "\\\\_SB.FAN%d", i);
		pr_info("virtfan: current_fan: \"%s\".", current_fan);
		status = acpi_get_handle(NULL, current_fan, &fan_handles[i]);
		if (ACPI_FAILURE(status)) {
			pr_err("virtfan: Failed to evaluate _STA for FAN%d: %s.\n", 
					i, acpi_format_exception(status));
			destroy_fan_handles();
			return -ENODEV;
		}

		pr_info("Loaded fan_handle[%d]: %p.", i, fan_handles[i]);
	}

	/** 
	 * Allocate a cdev structure for our fans.
	 * We are using the same cdev operations for each fan, 
	 * so we only need one.
	 */
	if ((ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME)),
		ret < 0) {
		destroy_fan_handles();
		return ret;
	}

	/* Initialize our cdev structure with the desired file operations. */
	cdev_init(&virtfan_cdev, &virtfan_fops);
	if ((ret = cdev_add(&virtfan_cdev, dev_number, 1)),
		ret) {
		goto err_unregister;
	}

	virtfan_class = class_create("virtfan");
	if (IS_ERR(virtfan_class)) {
		ret = PTR_ERR(virtfan_class);
		goto err_cdev_del;
	}

	virtfan_device = device_create(virtfan_class, NULL, dev_number, NULL,
		DEVICE_NAME);
	if (IS_ERR(virtfan_device)) {
		ret = PTR_ERR(virtfan_device);
		goto err_cdev_del;
	}

	pr_info("virtfan loaded\n");
	return 0;

err_cdev_del:
	cdev_del(&virtfan_cdev);
err_unregister:
	destroy_fan_handles();
	unregister_chrdev_region(dev_number, 1);
	return 0;
}

/**
 * Exit routine for this module. Cleans up any
 * dangling memory.
 */
static void __exit virtfan_unload(void)
{
	/* Cleanup the device itself. */
	device_destroy(virtfan_class, dev_number);
	class_destroy(virtfan_class);
	cdev_del(&virtfan_cdev);
	unregister_chrdev_region(dev_number, 1);
	
	/* Cleanup our allocated memory. */
	if (destroy_fan_handles() != SUCCESS) {
		pr_err("Failed to destroy_fan_handles.");
	}

	pr_info("virtfan unloaded\n");
}

module_init(virtfan_load);
module_exit(virtfan_unload);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kat Powell");
MODULE_DESCRIPTION("ACPI virtual fan driver");
