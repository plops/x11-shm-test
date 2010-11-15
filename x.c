#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <stdio.h>

int
main()
{
  Display *d=XOpenDisplay(0);
  int s=DefaultScreen(d);
  Window w=XCreateSimpleWindow(d,RootWindow(d,s),0,0,100,100,0,0,0);
  
  /* XVisualInfo h;  */
  /* int num; */
  /* h.visualid=XVisualIDFromVisual(DefaultVisual(d,s)); */
  /* XVisualInfo v=XGetVisualInfo(d,VisualIDMask,&h,&num); */
  
  Colormap cm=DefaultColormap(d,s);
  
  XMapWindow(d,w);

  sleep(3);

  printf("%d\n",XShmQueryExtension(d));
  
  {
    int major,minor;
    Bool pixmaps;
    int ret=XShmQueryVersion(d,&major,&minor,&pixmaps);
    printf("%d %d %d %d\n",ret,major,minor,pixmaps);
  }

  return 0;
}
