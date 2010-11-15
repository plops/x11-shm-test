#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <stdio.h>
// martin@hop:~/v1114$ w3m mit-shm.html 

int
bgra2rgb(int c)
{
  switch(c){
  case 0: return 2;
  case 1: return 1;
  case 2: return 0;
  }
}

int
main()
{
  int width=1366;
  int height=768;
  
  Display *d=XOpenDisplay(0);
  int s=DefaultScreen(d);
  Window w=XCreateSimpleWindow(d,RootWindow(d,s),0,0,width/10,height/10,0,0,0);
  
  /* XVisualInfo h;  */
  /* int num; */
  /* h.visualid=XVisualIDFromVisual(DefaultVisual(d,s)); */
  /* XVisualInfo v=XGetVisualInfo(d,VisualIDMask,&h,&num); */
  
  Colormap cm=DefaultColormap(d,s);
  
  XMapWindow(d,w);


  printf("%d\n",XShmQueryExtension(d));
  
  {
    int major,minor;
    Bool pixmaps;
    int ret=XShmQueryVersion(d,&major,&minor,&pixmaps);
    printf("r %d maj %d min %d pix %d\n",ret,major,minor,pixmaps);
  }

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
  
  XShmGetImage(d,RootWindow(d,s),image,0,0,0xffffff);

  //sleep(3);

  unsigned char buf[width*height*3];
  int i,j,c;
  for(j=0;j<height;j++)
    for(i=0;i<width;i++)
      for(c=0;c<3;c++)
	buf[3*(i+j*width)+c]=image->data[4*(i+j*width)+bgra2rgb(c)];
  
  FILE*f=fopen("o.pgm","w");
  fprintf(f,"P6 %d %d 255\n",width,height);
  fwrite(buf,sizeof(buf),1,f);
  fclose(f);

  XShmDetach(d,&shminfo);
  XDestroyImage(image);
  shmdt(shminfo.shmaddr);
  shmctl(shminfo.shmid,IPC_RMID,0);


  return 0;
}
