#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

IplImage* image = 0;
IplImage* src = 0;
IplImage* gray = 0;
IplImage* bin = 0;
IplImage* dst = 0;

int radius = 1;
int iterations = 8;

int main(int argc, char* argv[])
{
  // имя картинки задаётся первым параметром
  char* filename = argc == 2 ? argv[1] : "D://ugol.jpg";
  // получаем картинку
  image = cvLoadImage(filename,1);
	
  // клонируем картинку 
  src = cvCloneImage(image);

  printf("[i] image: %s\n", filename);
  assert( src != 0 );

  // Delaem preobrazovania kartinki:
  // sozdaem iadro
  IplConvKernel* Kern = cvCreateStructuringElementEx(radius*2+1, radius*2+1, radius, radius, CV_SHAPE_ELLIPSE);
  // kartinka dlia promezhyto4nogo xranenia rezyl'tatov cvCreateImage
  IplImage* Temp = 0;
  Temp = cvCreateImage(cvSize(image->width, image->height) , IPL_DEPTH_8U, 1);
    
  cvMorphologyEx(image, src, Temp, Kern, CV_MOP_OPEN, iterations);
  cvMorphologyEx(image, src, Temp, Kern, CV_MOP_GRADIENT, iterations);

  // создаём одноканальные картинки
  gray = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
  bin = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );

  // клонируем
  dst = cvCloneImage(src);
    
  // окно для отображения картинки
  cvNamedWindow("original",CV_WINDOW_AUTOSIZE);

  // преобразуем в градации серого
  cvCvtColor(src, gray, CV_RGB2GRAY);

  // преобразуем в двоичное
  cvInRangeS(gray, cvScalar(40), cvScalar(150), bin); // atoi(argv[2])

  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* contours=0;

  // находим контуры
  int contoursCont = cvFindContours( bin, storage,&contours,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));

  //Апроксимация контуров
  contours = cvApproxPoly( contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1 );

  // нарисуем контуры
  for(CvSeq* seq0 = contours;seq0 != 0;seq0 = seq0->h_next)
  {
    cvDrawContours(dst, seq0, CV_RGB(255,216,0), CV_RGB(0,0,250), 0, 1, 8); // рисуем контур
  }

  CvSeq* seq = contours;
	
  CvPoint* M = (CvPoint*)cvGetSeqElem ( seq, 0 );
  CvPoint* M1 = (CvPoint*)cvGetSeqElem ( seq, 0 );
  CvPoint* VP = (CvPoint*)cvGetSeqElem ( seq, 3 );		
  CvPoint* M2 = (CvPoint*)cvGetSeqElem ( seq, 1 );
  CvPoint* NP = (CvPoint*)cvGetSeqElem ( seq, 2 );

  M1->x += (abs((M1->x - VP->x)/2));
  M2->x += (abs((M2->x - NP->x)/2));

  cout << "Verxnia koordinata ygla: " << "(" <<  M1->x << " ; " <<  M1->y << ")" << endl;
  cout << "Nizhnia koordinata ygla: " << "(" <<  M2->x << " ; " <<  M2->y << ")" << endl;

  src = cvCloneImage(image);

  cvLine( src, cvPoint(M1->x, M1->y), cvPoint(M2->x, M2->y), CV_RGB(250,0,0), 3, 8);

  if ((M2->x - M1->x) == 0)
  {
	M->y = M1->y;
	cvLine( src, cvPoint(M->x=100, M->y ), cvPoint(M->x = 500, M->y), CV_RGB(100,200,100), 3, 8);
  }
  else
  {
	// koeff naklona priamoi
	float koeff = (M2->y - M1->y)/(M2->x - M1->x);

	M->x = M1->x+(abs(M1->x - M2->x)/2);
	M->y = M1->y+(abs(M1->y - M2->y)/2);
	int b = M->y;
	cvLine( src, cvPoint(M->x=100, M->y = (1/koeff)*(M->x)+b), cvPoint(M->x = 500, M->y = (1/koeff)*(M->x)+b), CV_RGB(100,200,100), 3, 8);
  } 
  // показываем картинку
  cvShowImage("original",image);
  
  // ждём нажатия клавиши
  cvWaitKey(0);

  // освобождаем ресурсы
  cvReleaseImage(&image);
  cvReleaseImage(&src);
  cvReleaseImage(&gray);
  cvReleaseImage(&bin);
  cvReleaseImage(&dst);
  // удаляем окна
  cvDestroyAllWindows();
  return 0;
}
