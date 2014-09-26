#include "MotionDetector.h"
#include <direct.h>

namespace Etoile
{
	MotionDetector::MotionDetector(void)
	{
		_dir = "/motion_src/pics"; // directory where the images will be stored
		_ext = ".jpg"; // extension of the images
		_logfile = "/motion_src/log";
		_delay = 50; // in mseconds, take a picture every 1/2 second
		_enable = true;
	}

	void MotionDetector::setDirectory(const std::string& path)
	{
		_dir = path + _dir;
		_logfile = path + _logfile;
	}

	MotionDetector::~MotionDetector(void)
	{
	}

	/**
	* Checks if a folder exists
	* @param foldername path to the folder to check.
	* @return true if the folder exists, false otherwise.
	*/
	bool folder_exists(std::string foldername)
	{
		struct stat st;
		stat(foldername.c_str(), &st);
		return st.st_mode & S_IFDIR;
	}

	/**
	* Portable wrapper for mkdir. Internally used by mkdir()
	* @param[in] path the full path of the directory to create.
	* @return zero on success, otherwise -1.
	*/
	int we_mkdir(const char *path)
	{
#ifdef _WIN32
		return ::_mkdir(path);
#else
#if _POSIX_C_SOURCE
		return ::mkdir(path);
#else
		return ::mkdir(path, 0755); // not sure if this works on mac
#endif
#endif
	}


	// Check if the directory exists, if not create it
	// This function will create a new directory if the image is the first
	// image taken for a specific day
	inline void directoryExistsOrCreate(const char* pzPath)
	{
		DIR *pDir;
		// directory doesn't exists -> create it
		if ( pzPath == NULL || (pDir = opendir (pzPath)) == NULL)
			we_mkdir(pzPath);
		// if directory exists we opened it and we
		// have to close the directory again.
		else if(pDir != NULL)
			(void) closedir (pDir);
	}

	// When motion is detected we write the image to disk
	//    - Check if the directory exists where the image will be stored.
	//    - Build the directory and image names.
	int incr = 0;
	inline bool saveImg(Mat image, const string DIRECTORY, const string EXTENSION, const char * DIR_FORMAT, const char * FILE_FORMAT)
	{
		stringstream ss;
		time_t seconds;
		struct tm * timeinfo;
		char TIME[80];
		time (&seconds);
		// Get the current time
		timeinfo = localtime (&seconds);

		// Create name for the date directory
		strftime (TIME,80,DIR_FORMAT,timeinfo);
		ss.str("");
		ss << DIRECTORY<<"/" << TIME;
		directoryExistsOrCreate(ss.str().c_str());
		ss << "/cropped";
		directoryExistsOrCreate(ss.str().c_str());

		// Create name for the image
		strftime (TIME,80,FILE_FORMAT,timeinfo);
		ss.str("");
		if(incr < 100) incr++; // quick fix for when delay < 1s && > 10ms, (when delay <= 10ms, images are overwritten)
		else incr = 0;
		ss << DIRECTORY<<"/" << TIME << static_cast<int>(incr) << EXTENSION;
		std::cout<<"save:"<<ss.str()<<std::endl;
		return imwrite(ss.str().c_str(), image);
	}

	// Check if there is motion in the result matrix
	// count the number of changes and return.
	inline int detectMotion(const Mat & motion, Mat & result, Mat & result_cropped,
		int x_start, int x_stop, int y_start, int y_stop,
		int max_deviation,
		Scalar & color)
	{
		// calculate the standard deviation
		Scalar mean, stddev;
		meanStdDev(motion, mean, stddev);
		// if not to much changes then the motion is real (neglect agressive snow, temporary sunlight)
		if(stddev[0] < max_deviation)
		{
			int number_of_changes = 0;
			int min_x = motion.cols, max_x = 0;
			int min_y = motion.rows, max_y = 0;
			// loop over image and detect changes
			for(int j = y_start; j < y_stop; j+=2){ // height
				for(int i = x_start; i < x_stop; i+=2){ // width
					// check if at pixel (j,i) intensity is equal to 255
					// this means that the pixel is different in the sequence
					// of images (prev_frame, current_frame, next_frame)
					if(static_cast<int>(motion.at<uchar>(j,i)) == 255)
					{
						number_of_changes++;
						if(min_x>i) min_x = i;
						if(max_x<i) max_x = i;
						if(min_y>j) min_y = j;
						if(max_y<j) max_y = j;
					}
				}
			}
			if(number_of_changes){
				//check if not out of bounds
				if(min_x-10 > 0) min_x -= 10;
				if(min_y-10 > 0) min_y -= 10;
				if(max_x+10 < result.cols-1) max_x += 10;
				if(max_y+10 < result.rows-1) max_y += 10;
				// draw rectangle round the changed pixel
				Point x(min_x,min_y);
				Point y(max_x,max_y);
				Rect rect(x,y);
				Mat cropped = result(rect);
				cropped.copyTo(result_cropped);
				rectangle(result,rect,color,1);
			}
			return number_of_changes;
		}
		return 0;
	}

	void MotionDetector::apply()
	{
		//cvNamedWindow("GeckoGeek Window", CV_WINDOW_AUTOSIZE);
		directoryExistsOrCreate(_dir.c_str());
		// Format of directory
		string DIR_FORMAT = "%Y_%m_%d"; // 1Jan1970
		string FILE_FORMAT = DIR_FORMAT + "/" +"%Y_%m_%d_%H_%M_%S"; // 1Jan1970/1Jan1970_12153
		string CROPPED_FILE_FORMAT = DIR_FORMAT + "/cropped/" + "%Y_%m_%d_%H_%M_%S"; // 1Jan1970/cropped/1Jan1970_121539

		// Set up camera
		CvCapture * camera = cvCaptureFromCAM(CV_CAP_ANY);
		cvSetCaptureProperty(camera, CV_CAP_PROP_FRAME_WIDTH, 1280); // width of viewport of camera
		cvSetCaptureProperty(camera, CV_CAP_PROP_FRAME_HEIGHT, 720); // height of ...

		// Take images and convert them to gray
		Mat result, result_cropped;
		Mat prev_frame = result = cvQueryFrame(camera);
		Mat current_frame = cvQueryFrame(camera);
		Mat next_frame = cvQueryFrame(camera);
		cvtColor(current_frame, current_frame, CV_RGB2GRAY);
		cvtColor(prev_frame, prev_frame, CV_RGB2GRAY);
		cvtColor(next_frame, next_frame, CV_RGB2GRAY);

		// d1 and d2 for calculating the differences
		// result, the result of and operation, calculated on d1 and d2
		// number_of_changes, the amount of changes in the result matrix.
		// color, the color for drawing the rectangle when something has changed.
		Mat d1, d2, motion;
		int number_of_changes, number_of_sequence = 0;
		Scalar mean_, color(0,255,255); // yellow

		// Detect motion in window
		int x_start = 10, x_stop = current_frame.cols-11;
		int y_start = 350, y_stop = 530;

		// If more than 'there_is_motion' pixels are changed, we say there is motion
		// and store an image on disk
		int there_is_motion = 5;

		// Maximum deviation of the image, the higher the value, the more motion is allowed
		int max_deviation = 20;

		// Erode kernel
		Mat kernel_ero = getStructuringElement(MORPH_RECT, Size(2,2));

		// All settings have been set, now go in endless loop and
		// take as many pictures you want..
		while (_enable){
			// Take a new image
			prev_frame = current_frame;
			current_frame = next_frame;
			next_frame = cvQueryFrame(camera);
			
			//cvShowImage("GeckoGeek Window", &next_frame);

			result = next_frame;
			cvtColor(next_frame, next_frame, CV_RGB2GRAY);

			// Calc differences between the images and do AND-operation
			// threshold image, low differences are ignored (ex. contrast change due to sunlight)
			absdiff(prev_frame, next_frame, d1);
			absdiff(next_frame, current_frame, d2);
			bitwise_and(d1, d2, motion);
			threshold(motion, motion, 35, 255, CV_THRESH_BINARY);
			erode(motion, motion, kernel_ero);

			number_of_changes = detectMotion(motion, result, result_cropped,  x_start, x_stop, y_start, y_stop, max_deviation, color);
			if(_show) imshow("VIDEO", result);
			// If a lot of changes happened, we assume something changed.
			if(number_of_changes>=there_is_motion)
			{
				if(number_of_sequence>0){ 
					saveImg(result,_dir,_ext,DIR_FORMAT.c_str(),FILE_FORMAT.c_str());
					saveImg(result_cropped,_dir,_ext,DIR_FORMAT.c_str(),CROPPED_FILE_FORMAT.c_str());
				}
				number_of_sequence++;
			}
			else
			{
				number_of_sequence = 0;
				// Delay, wait a 1/2 second.
				cvWaitKey (_delay);
			}
			if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		   {
					cout << "esc key is pressed by user" << endl; 
					return exit(0);
					break; 
		   }
		}
	}

	void MotionDetector::cleanDirectory()
	{
		std::remove(_dir.c_str());
	}
}