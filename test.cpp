//=============================================================================
// Open FLIR Camera
// Take Picture
// Analyze the focus value
// 2020.04.14
// by Shang-Wen, Wong
//=============================================================================


#include <iostream>
#include <FlyCapture2.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

using namespace std;

void PrintBuildInfo()
{
    FlyCapture2::FC2Version fc2Version;
    FlyCapture2::Utilities::GetLibraryVersion(&fc2Version);

    ostringstream version;
    version << "FlyCapture2 library version: " << fc2Version.major << "."
            << fc2Version.minor << "." << fc2Version.type << "."
            << fc2Version.build;
    cout << version.str() << endl;

    ostringstream timeStamp;
    timeStamp << "Application build date: " << __DATE__ << " " << __TIME__;
    cout << timeStamp.str() << endl << endl;
}

void PrintCameraInfo(FlyCapture2::CameraInfo *pCamInfo)
{
    cout << endl;
    char ipAddress[32];

    cout << "*** CAMERA INFORMATION ***" << endl;
    cout << "Serial number - " << pCamInfo->serialNumber << endl;
    cout << "Camera model - " << pCamInfo->modelName << endl;
    cout << "Camera vendor - " << pCamInfo->vendorName << endl;
    cout << "Sensor - " << pCamInfo->sensorInfo << endl;
    cout << "Resolution - " << pCamInfo->sensorResolution << endl;
    cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
    cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl;
    
       sprintf( 
        ipAddress, 
        "%u.%u.%u.%u", 
        pCamInfo->ipAddress.octets[0],
        pCamInfo->ipAddress.octets[1],
        pCamInfo->ipAddress.octets[2],
        pCamInfo->ipAddress.octets[3]);
    cout << "Camera IP - " << ipAddress<<endl;
        // pCamInfo->ipAddress.octets[0]<<","<<
        // pCamInfo->ipAddress.octets[1]<<","<<
        // pCamInfo->ipAddress.octets[2]<<","<<
        // pCamInfo->ipAddress.octets[3] << endl
         //<< endl;
}

void PrintError(FlyCapture2::Error error) { error.PrintErrorTrace(); }

int RunSingleCamera(FlyCapture2::PGRGuid guid)
{
    const int k_numImages = 10;

    FlyCapture2::Error error;    

    // Connect to a camera
	FlyCapture2::Camera cam;
    error = cam.Connect(&guid);
    if (error != FlyCapture2::PGRERROR_OK)
    {
        PrintError(error);
        return -1;
    }

    // Get the camera information
    FlyCapture2::CameraInfo camInfo;
    error = cam.GetCameraInfo(&camInfo);
    if (error != FlyCapture2::PGRERROR_OK)
    {
        PrintError(error);
        return -1;
    }

    PrintCameraInfo(&camInfo);

    // Get the camera configuration
    FlyCapture2::FC2Config config;
    error = cam.GetConfiguration(&config);
    if (error != FlyCapture2::PGRERROR_OK)
    {
        PrintError(error);
        return -1;
    }

    // Set the number of driver buffers used to 10.
    config.numBuffers = 10;

    // Set the camera configuration
    error = cam.SetConfiguration(&config);
    if (error != FlyCapture2::PGRERROR_OK)
    {
        PrintError(error);
        return -1;
    }

    // Start capturing images
    error = cam.StartCapture();
    if (error != FlyCapture2::PGRERROR_OK)
    {
        PrintError(error);
        return -1;
    }
 int imageCnt=0;
    FlyCapture2::Image rawImage;
    while(true){

    //}
    // for (int imageCnt = 0; imageCnt < k_numImages; imageCnt++)
    // {
        // Retrieve an image
        error = cam.RetrieveBuffer(&rawImage);
        if (error != FlyCapture2::PGRERROR_OK)
        {
            PrintError(error);
            continue;
        }

       
        // cout << "Grabbed image " << imageCnt << endl;
        // imageCnt++; 
        // // Create a converted image
        // FlyCapture2::Image convertedImage;

        // // Convert the raw image
        // error = rawImage.Convert(FlyCapture2::PIXEL_FORMAT_MONO8, &convertedImage);
        // if (error != FlyCapture2::PGRERROR_OK)
        // {
        //     PrintError(error);
        //     return -1;
        // }

        // convert to rgb
        FlyCapture2::Image rgbImage;
        rawImage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage );

        // convert to OpenCV Mat
        unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();   
        cv::Mat imageSource = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(),rowBytes);

        cv::Mat imageGrey;
 
        cv::cvtColor(imageSource, imageGrey, CV_RGB2GRAY);
        cv::Mat imageSobel;
        //Sobel(imageGrey, imageSobel, CV_16U, 1, 1);
        cv::Laplacian(imageGrey, imageSobel, CV_16U);

        //图像的平均灰度
        double meanValue = 0.0;
        meanValue = mean(imageSobel)[0];
    
        //double to string
        stringstream meanValueStream;
        string meanValueString;
        meanValueStream << meanValue;
        meanValueStream >> meanValueString;
        meanValueString = "Articulation(Laplacian Method): " + meanValueString;
        cv::Mat dst;
        //cv::hconcat(imageSource, imageSobel, dst);

        
        dst=imageSource.clone();
        cv::putText(dst, meanValueString, cv::Point(20, 50), CV_FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(255, 255, 25), 2);
        
        cv::imshow("Articulation", dst);
        cv::waitKey(1);
        //  char key = 0;
        // key = cv::waitKey(30);     

        // // Create a unique filename

        // ostringstream filename;
        // filename << "FlyCapture2Test-" << camInfo.serialNumber << "-"
        //          << imageCnt << ".pgm";

        // // Save the image. If a file format is not passed in, then the file
        // // extension is parsed to attempt to determine the file format.
        // error = convertedImage.Save(filename.str().c_str());
        // if (error != FlyCapture2::PGRERROR_OK)
        // {
        //     PrintError(error);
        //     return -1;
        // }
    }

    // Stop capturing images
    error = cam.StopCapture();
    if (error != FlyCapture2::PGRERROR_OK)
    {
        PrintError(error);
        return -1;
    }

    // Disconnect the camera
    error = cam.Disconnect();
    if (error != FlyCapture2::PGRERROR_OK)
    {
        PrintError(error);
        return -1;
    }

    return 0;
}


int main()
{
    cv::Mat img_ori;
    cv::Mat img_trans;

    cout<<"Try this!\n";

    PrintBuildInfo();

    FlyCapture2::Error error;

    FlyCapture2::BusManager busMgr;
    unsigned int numCameras;
    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != FlyCapture2::PGRERROR_OK)
    {
        PrintError(error);
        return -1;
    }

    cout << "Number of cameras detected: " << numCameras << endl;


    for (unsigned int i = 0; i < numCameras; i++)
    {
        FlyCapture2::PGRGuid guid;
        error = busMgr.GetCameraFromIndex(i, &guid);
        if (error != FlyCapture2::PGRERROR_OK)
        {
            PrintError(error);
            return -1;
        }

        RunSingleCamera(guid);
    }
    //Find Camera

    //Capture Image
    //Analyze Image

    // cv::Mat frame;
    // cv::VideoCapture cap;

    // int deviceID =0;
    // int apiID =cv::CAP_ANY;
    // cap.open(deviceID+apiID);
    // if(!cap.isOpened())
    // {
    //     cerr<<"Error! Unable to open camer\n";
    //     return -1;
    // }
    // else{
    //     cout<<"Opened\n";
    // }
    // cout<<"Start grabbing:\n";
    
    // //--- GRAB AND WRITE LOOP
    // cout << "Start grabbing" << endl
    //     << "Press any key to terminate" << endl;
    // for (;;)
    // {
    //     // wait for a new frame from camera and store it into 'frame'
    //     cap.read(frame);
    //     // check if we succeeded
    //     if (frame.empty()) {
    //         cerr << "ERROR! blank frame grabbed\n";
    //         break;
    //     }
    //     // show live and wait for a key with timeout long enough to show images
    //     imshow("Live", frame);
    //     if (cv::waitKey(5) >= 0)
    //         break;
    // }
    // // the camera will be deinitialized automatically in VideoCapture destructor


    return 0;
    system("pause");
}
