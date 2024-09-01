#include <iostream>
#include <algorithm>
#include <memory>
#include <qt5/QtNetwork/QUdpSocket>
#include "core.hpp"
#include "udp.hpp"
#include "cv.hpp"
#include "utility.hpp"

// using namespace cv;

// static LibCamera cam;
// static uint32_t width = 640;
// static uint32_t height = 480;
// static uint32_t stride = 0;
// static char key;
// static int window_width = 640;
// static int window_height = 480;
// static LibcameraOutData frameData;
// static ControlList controls_;
int main(int argc, char *argv[])
{
  // try {

    // if (width > window_width)
    // {
    //     cv::namedWindow("libcamera-demo", cv::WINDOW_NORMAL);
    //     cv::resizeWindow("libcamera-demo", window_width, window_height);
    // } 

    // int ret = cam.initCamera();
    // cam.configureStill(width, height, formats::RGB888, 1, 0);
    
    // int64_t frame_time = 1000000 / 60;
	  // controls_.set(controls::FrameDurationLimits, libcamera::Span<const int64_t, 2>({ frame_time, frame_time }));

    // cam.set(controls_);
    
    // cam.startCamera();
    // cam.VideoStream(&width, &height, &stride);
    // // if (!ret) {
    //     while (true) {
    //         if (!cam.readFrame(&frameData))
    //             continue;
    //         Mat im(height, width, CV_8UC3, frameData.imageData, stride);

    //         imshow("libcamera-demo", im);
    //         key = waitKey(1);

    //         cam.returnFrameBuffer(frameData);
    //     }
        // destroyAllWindows();
        // cam.stopCamera();
    // }
    // cam.closeCamera();
    // return 0;


    // QCoreApplication app(argc, argv);
    app::CVision cv("foo");
    cv.process();
    // auto [clientIp, clientPort] = app::parseJsonFile("/home/vympel/usr/cv_project/prod/conf/config.json").value();
    // app::UdpSocket socket(QHostAddress(clientIp), clientPort, 
    //                       QHostAddress::LocalHost, app::constants::port::RECEIVER_PORT);

    // std::string filename = "/home/vasily/usr/phystech/vympel/prod/app/video.mp4";
    // app::Core core(std::make_shared<app::CVision>(filename));
    // QThread thread;
    // core.moveToThread(&thread);

    // qRegisterMetaType<app::process_params_t>();
    // qRegisterMetaType<app::core_mode_t>();

    // QObject::connect(&thread, &QThread::started, &core, &app::Core::process, Qt::QueuedConnection);
    // QObject::connect(&core, &app::Core::exit, &thread, &QThread::quit, Qt::QueuedConnection);
    // QObject::connect(&thread, &QThread::finished, &core, &QObject::deleteLater, Qt::QueuedConnection);
    // QObject::connect(&thread, &QThread::finished, &app, &QCoreApplication::quit, Qt::QueuedConnection);
    // QObject::connect(&socket, &app::UdpSocket::sendData, &core, &app::Core::receiveData, Qt::QueuedConnection);
    // QObject::connect(&core, &app::Core::sendData, &socket, &app::UdpSocket::receiveData, Qt::QueuedConnection);

    // thread.start();

    // return app.exec();
  // }

  // catch (const std::exception& ex) {
  //   std::cout << ex.what() << std::endl;
  // }
}