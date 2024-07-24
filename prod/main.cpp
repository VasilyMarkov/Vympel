#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include "bluetoothDevice.hpp"
#include <qt6/QtNetwork/QUdpSocket>
#include <qt6/QtCore/QByteArray>
#include <qt6/QtCore/QJsonObject>
#include <qt6/QtCore/QJsonDocument>

class LowPassFilter {
 public:
  LowPassFilter(float cutoff_frequency, float sample_rate, float q = 0.707)
      : alpha_(std::sin(2 * M_PI * cutoff_frequency / sample_rate) / (2 * q)),
        y_(0) {}

  float Process(float x) {
    y_ = alpha_ * (x - y_) + y_;
    return y_;
  }

 private:
  float alpha_;
  float y_;
};

float cutoff_frequency = 10.0;
float sample_rate = 1000.0;
LowPassFilter filter(cutoff_frequency, sample_rate);


void sendData(QUdpSocket* socket, const QByteArray& data) {
  socket->writeDatagram(data, QHostAddress::LocalHost, 65000);
}

int main(int argc, char** argv )
{
  // BluetoothDevice device;
  QUdpSocket sock;
  QJsonObject json;
  
  if ( argc != 2 )
  {
    printf("usage: DisplayImage.out <Image_Path>\n");
    
    return -1;
  }
    std::string filename = argv[1];
    cv::VideoCapture capture(filename);
    cv::Mat frame;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";
    cv::namedWindow( "w", 1);

    while(true)
    {
        capture >> frame;
        if(frame.empty())
            break;
        
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY, 0);
        std::vector<uint8_t> v(frame.begin<uint8_t>(), frame.end<uint8_t>());
        int sum = std::accumulate(std::begin(v), std::end(v), 0);

        auto filtered = filter.Process(sum);

        json["brightness"] = sum;
        json["filtered"] = filtered;
        sendData(&sock, QJsonDocument(json).toJson());
        cv::imshow("w", frame);
        cv::waitKey(20); // waits to display frame
        
    }
    cv::waitKey(0); // key press to close window
}