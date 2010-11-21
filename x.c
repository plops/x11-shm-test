#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <stdio.h>
// martin@hop:~/v1114$ w3m mit-shm.html 

int
main()
{
  int width=1366;
  int height=768;
  
  Display *d=XOpenDisplay(0);
  int s=DefaultScreen(d);
  /*
  printf("%d\n",XShmQueryExtension(d));
  
  {
    int major,minor;
    Bool pixmaps;
    int ret=XShmQueryVersion(d,&major,&minor,&pixmaps);
    printf("r %d maj %d min %d pix %d\n",ret,major,minor,pixmaps);
    }
  */
  
  XShmSegmentInfo shminfo;
  XImage *image=XShmCreateImage(d,DefaultVisual(d,s),
				DefaultDepth(d,s),
				ZPixmap,
				0,&shminfo,
				width,height);
  shminfo.shmid=shmget(IPC_PRIVATE,image->bytes_per_line*image->height,
		       IPC_CREAT|0777);

  printf("id %d line %d\n",shminfo.shmid, image->bytes_per_line);


  shminfo.shmaddr=image->data=shmat(shminfo.shmid,0,0);
  shminfo.readOnly=False;

  printf("addr 0x%x\n",image->data);

  printf("at %d\n",XShmAttach(d,&shminfo));
  int i;

  FILE*f=fopen("/dev/shm/o.bgra","w");
  // 1000 640x480 frames in 1.5s
  for(i=0;i<1000;i++){
  XShmGetImage(d,RootWindow(d,s),image,0,0,0xffffff);
    fwrite(image->data,width*height*4,1,f);
    usleep(32000);
  }
  fclose(f);

  // encode using
  // x264 --input-res 640x480 --input-csp bgra -o o.264 o.bgra

  XShmDetach(d,&shminfo);
  XDestroyImage(image);
  shmdt(shminfo.shmaddr);
  shmctl(shminfo.shmid,IPC_RMID,0);


  return 0;
}
