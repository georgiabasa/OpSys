import subprocess

def register_device(device_name):
    register_device=f"sudo mknod /dev/{device_name} c 42 0"
    subprocess.run(register_device,shell=True)
    make=f"make"
    subprocess.run(make)
    load_module=f"sudo insmod dev.ko"
    subprocess.run(load_module,shell=True)

def remove_device(device_name):
    remove_device=f"sudo rm /dev/{device_name}"
    subprocess.run(remove_device,shell=True)
    remove_module=f"sudo rmmod dev"
    subprocess.run(remove_module,shell=True)

def write_device(device_name,msg):
    write_device=f"echo {msg} > /dev/{device_name}"
    subprocess.run(write_device,shell=True)


def read_device(device_name):
    read_device=f"cat /dev/{device_name}"
    subprocess.run(read_device,shell=True)

register_device("mydevice")
while 1:
    read_device("mydevice")
    newmsg=str(input("Enter the new msg for the device (0 for exit): "))
    if(newmsg=="0"):
        break
    write_device("mydevice",newmsg)

remove_device("mydevice")
print("Bye Bye")
    



