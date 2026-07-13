set -ex 

# Ensure the pipes exist
if [ ! -p serial_pipe.in ];  then mkfifo -m 666 serial_pipe.in;  fi
if [ ! -p serial_pipe.out ]; then mkfifo -m 666 serial_pipe.out; fi

# Clear the output listener screen
printf "\033[2J\033[H" > serial_pipe.out &

# Flush any leftover input characters so they don't ghost into the next boot
dd if=serial_pipe.in iflag=nonblock of=/dev/null status=none || true

qemu-system-x86_64 \
    -machine q35 \
    -enable-kvm \
    -serial pipe:serial_pipe \
    -m 512M \
    -kernel arch/x86/boot/bzImage \
    -initrd ~/projects/personal/initramfs/result/initrd.gz \
    -append "console=ttyS0 earlyprintk=serial,ttyS0,115200 panic=1 no_console_suspend loglevel=8" \
    -D qemu-log.txt \
    -d int
