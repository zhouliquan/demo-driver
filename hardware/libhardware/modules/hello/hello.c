#define LOG_TAG "HelloStub:"

#include <hardware/hardware.h>
#include <hardware/hello.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <stdlib.h>
#include <memory.h>

#define DEVICE_NAME "/dev/hello"
#define MODULE_NAME "Hello"
#define MODULE_AUTHOR "Richie"

/*设备打开和关闭接口*/
static int hello_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device);
static int hello_device_close(struct hw_device_t* device);

/*设备访问接口*/
static int hello_set_val(struct hello_device_t* dev, int val);
static int hello_get_val(struct hello_device_t* dev, int* val);

/*模块方法表*/
static struct hw_module_methods_t hello_module_methods ={
    open: hello_device_open
};

/*模块实例变量*/
/*实例变量名必须为HAL_MODULE_INFO_SYM,
 *tag 必须为HARDWARE_MODULE_TAG
 */
static struct hello_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: HELLO_HARDWARE_MODULE_ID,
        name: MODULE_NAME,
        author: MODULE_AUTHOR,
        methods: &hello_module_methods,
    }
};

static int hello_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device){
    struct hello_device_t* dev;
    dev = (struct hello_device_t*)malloc(sizeof(struct hello_device_t));

    if(!dev){
        ALOGE(LOG_TAG" failed to alloc space");
        return -EFAULT;
    }

    memset(dev, 0, sizeof(struct hello_device_t));

    dev->common.tag = HARDWARE_MODULE_TAG;
    dev->common.version = 0;
    dev->common.module = (hw_module_t*)module;
    dev->common.close = hello_device_close;
    dev->set_val = hello_set_val;
    dev->get_val = hello_get_val;

    if((dev->fd = open(DEVICE_NAME, O_RDWR)) == -1) {
        ALOGE(LOG_TAG" failed to open "DEVICE_NAME" -- %s", strerror(errno));
        free(dev);
        return -EFAULT;
    }

    *device = &(dev->common);
    ALOGI(LOG_TAG" open "DEVICE_NAME" successfully.");

    return 0;
}

static int hello_device_close(struct hw_device_t* device){
    struct hello_device_t* dev = (struct hello_device_t*)device;
    if(dev){
        close(dev->fd);
        free(dev);
    }

    return 0;
}


static int hello_set_val(struct hello_device_t* dev, int val){
    ALOGI(LOG_TAG" set val %d to device.", val);

    write(dev->fd, &val, sizeof(val));
    return 0;
}

static int hello_get_val(struct hello_device_t* dev, int* val){
    if(!val){
        ALOGI(LOG_TAG" error val pointer.");
        return -EFAULT;
    }

    read(dev->fd, val, sizeof(*val));

    ALOGI(LOG_TAG" get value %d from device", *val);

    return 0;
}
