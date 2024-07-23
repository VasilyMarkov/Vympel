#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include "bluetoothDevice.hpp"


int main(int argc, char** argv )
{
  BluetoothDevice device;
  // if ( argc != 2 )
  // {
  //   printf("usage: DisplayImage.out <Image_Path>\n");
    
  //   return -1;
  // }
  //   std::string filename = argv[1];
  //   cv::VideoCapture capture(filename);
  //   cv::Mat frame;

  //   std::cout << v[0] << std::endl;
  //   if( !capture.isOpened() )
  //       throw "Error when reading steam_avi";
  //   cv::namedWindow( "w", 1);

  //   while(true)
  //   {
  //       capture >> frame;
  //       if(frame.empty())
  //           break;
        
  //       cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY, 0);

  //       auto mean = std::accumulate(frame.begin<int>(), frame.end<int>(), 0);
  //       // std::cout << mean << std::endl;
  //       cv::imshow("w", frame);
  //       cv::waitKey(20); // waits to display frame
        
  //   }
  //   cv::waitKey(0); // key press to close window
}