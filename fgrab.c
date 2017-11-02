#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>

#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

int main (int argc, char *argv[])
{
	struct fb_var_screeninfo screeninfo;
	unsigned char *map_buffer;
	int fd, fbsize, i, ret;

	if (argc <= 1) {
		printf ("Specify device to open\n");
		return 0;
	}

	fd = open (argv[1], O_RDWR);
	if (fd < 0) {
		printf ("failed to open device %s\n", argv[1]);
		return 0;
	}

	/* Get variable display parameters */
	ret = ioctl (fd, FBIOGET_VSCREENINFO, &screeninfo);
	if (ret) {
		printf ("FBIOGET_VSCREENINFO ioctl failed\n");
		goto fail;
	}

	printf ("xres, yres, pixel/bit : %d, %d, %d\n", screeninfo.xres,
		screeninfo.yres, screeninfo.bits_per_pixel);

	/* Size of frame buffer */
	fbsize = screeninfo.xres * screeninfo.yres * (screeninfo.bits_per_pixel / 8);

	printf ("fbsize: %d\n", fbsize);
	/* Map video memory */
	if ((map_buffer =
	     mmap (0, fbsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == (void *) -1) {
		printf ("Failed to mmap framebuffer\n");
		goto fail;
	}

	printf ("\n");
	for (i = 0; i < fbsize; i++)
		printf ("%x %x %x\n", *(map_buffer + i++),
			*(map_buffer + i++), *(map_buffer + i++));
	printf ("\n");

	munmap (map_buffer, fbsize);

fail:
	close (fd);
	return 0;
}
