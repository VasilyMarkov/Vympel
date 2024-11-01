#include <numeric>
#include "cv.hpp"

using namespace cv;

app::CVision::CVision(const std::string& filename): 
    // capture_(0), 
    filter_(app::constants::filter::cutoff_frequency, app::constants::filter::sample_rate)
{

    // if(!capture_.isOpened()) throw std::runtime_error("file open error");
    // cv::namedWindow( "w", 1);

    // if (width > window_width)
    // {
    //     cv::namedWindow("libcamera-demo", cv::WINDOW_NORMAL);
    //     cv::resizeWindow("libcamera-demo", window_width, window_height);
    // } 

    int ret = cam.initCamera();
    cam.configureStill(width, height, formats::RGB888, 1, 0);
    
    int64_t frame_time = 1000000 / 60;
    controls_.set(controls::FrameDurationLimits, libcamera::Span<const int64_t, 2>({ frame_time, frame_time }));

    cam.set(controls_);
    
    cam.startCamera();
    cam.VideoStream(&width, &height, &stride);
}

bool app::CVision::process()
{
    if (cam.readFrame(&frameData)) {
        Mat im(height, width, CV_8UC3, frameData.imageData, stride);

        // imshow("libcamera-demo", im);
        // key = waitKey(1);

        std::vector<uint8_t> v(im.begin<uint8_t>(), im.end<uint8_t>());
        process_params_.brightness = std::accumulate(std::begin(v), std::end(v), 0);

        process_params_.filtered = filter_.Process(process_params_.brightness);
        cam.returnFrameBuffer(frameData);

        // std::cout << process_params_.filtered << std::endl;
    }
    ++global_tick_;
    return true;
        
}

app::CVision::~CVision() {
    destroyAllWindows();
    cam.stopCamera();
    cam.closeCamera();
}