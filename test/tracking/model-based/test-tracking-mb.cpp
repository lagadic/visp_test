/*! \example test-tracking-mb.cpp */
#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>

#include <visp3/gui/vpDisplayGDI.h>
#include <visp3/gui/vpDisplayOpenCV.h>
#include <visp3/gui/vpDisplayX.h>
#include <visp3/io/vpImageIo.h>
#include <visp3/core/vpIoTools.h>
#include <visp3/mbt/vpMbEdgeKltTracker.h>
#include <visp3/io/vpVideoReader.h>
#include <visp3/sensor/vp1394TwoGrabber.h>

template<typename InputIterator1, typename InputIterator2>
bool
rangeEqual(InputIterator1 first1, InputIterator1 last1,
           InputIterator2 first2, InputIterator2 last2)
{
  while(first1 != last1 && first2 != last2) {
    if(*first1 != *first2) return false;
    ++first1;
    ++first2;
  }
  return (first1 == last1) && (first2 == last2);
}

bool compareFiles(const std::string& filename1, const std::string& filename2)
{
  std::ifstream file1(filename1.c_str());
  std::ifstream file2(filename2.c_str());
  
  std::istreambuf_iterator<char> begin1(file1);
  std::istreambuf_iterator<char> begin2(file2);
  
  std::istreambuf_iterator<char> end;
  
  return rangeEqual(begin1, end, begin2, end);
}

std::string getTestFolder(char **argv)
{    
  return std::string(vpIoTools::getNameWE(argv[0]));
}

std::string getTestName(const std::string &opt_video, const std::string &opt_model,
                        int opt_tracker, int opt_visibility, int opt_optimization)
{
  std::stringstream ss;
  ss << std::string("video=") << opt_video;
  ss << std::string("-model=") << opt_model;
  ss << std::string("-tracker=") << opt_tracker;
  ss << std::string("-visibility=") << opt_visibility;
  ss << std::string("-optim=") << opt_optimization;
  ss << std::string(".txt");
  return ss.str();
}

typedef enum {
  MBT_Edge,
  MBT_Keypoint,
  MBT_Hybrid
} TrackerType;

typedef enum {
  Visibility_none,
  Visibility_ogre,
  Visibility_scanline,
  Visibility_ogre_scanline
} VisibilityType;

int main(int argc, char** argv)
{
#if defined(VISP_HAVE_OPENCV) && (VISP_HAVE_OPENCV_VERSION >= 0x020100) && defined(VISP_HAVE_XML2)

  try {
    bool opt_live = false;
    std::string opt_video = "teabox";
    std::string opt_model = "teabox";

    bool opt_turn_off_display = false;
    TrackerType opt_tracker = MBT_Hybrid;
    bool opt_visibility_ogre = false;
    VisibilityType opt_visibility = Visibility_none;
    vpMbTracker::vpMbtOptimizationMethod  opt_optim = vpMbTracker::GAUSS_NEWTON_OPT;
    bool opt_test_pose = false;
    bool opt_init_by_click = false;
    int opt_nb_run = 1;
    int opt_save_images = 0;

    for (int i=0; i<argc; i++) {
      if (std::string(argv[i]) == "--video")
        opt_video = std::string(argv[i+1]);
      else if (std::string(argv[i]) == "--model")
        opt_model = std::string(argv[i+1]);
      else if (std::string(argv[i]) == "--turn-off-display")
        opt_turn_off_display = true;
      else if (std::string(argv[i]) == "--test-pose")
        opt_test_pose = true;
      else if (std::string(argv[i]) == "--init-by-click")
        opt_init_by_click = true;
      else if (std::string(argv[i]) == "--nb-run")
        opt_nb_run = atoi(argv[i+1]);
      else if (std::string(argv[i]) == "--save-images")
        opt_save_images = true;

      else if (std::string(argv[i]) == "--visibility") {
        if (atoi(argv[i+1]) == 0)
          opt_visibility = Visibility_none;
        else if (atoi(argv[i+1]) == 1)
          opt_visibility = Visibility_ogre;
        else if (atoi(argv[i+1]) == 2)
          opt_visibility = Visibility_scanline;
        else
          opt_visibility = Visibility_ogre_scanline;
      }

      else if (std::string(argv[i]) == "--tracker") {
        if (atoi(argv[i+1]) == 0)
          opt_tracker = MBT_Edge;
        else if (atoi(argv[i+1]) == 1)
          opt_tracker = MBT_Keypoint;
        else
          opt_tracker = MBT_Hybrid;
      }

      else if (std::string(argv[i]) == "--optim") {
        if (atoi(argv[i+1]) == 0)
          opt_optim = vpMbTracker::GAUSS_NEWTON_OPT;
        else
          opt_optim = vpMbTracker::LEVENBERG_MARQUARDT_OPT;
      }
      else if (std::string(argv[i]) == "--help") {
        std::cout << "\nUsage: " << argv[0] << " [--video <live|video generic name >] [--model <model generic name>] [--turn-off-display] [--tracker <0 (edge), 1 (keypoint), 2 (hybrid)>] [--visibility <0 (edgenone), 1 (ogre), 2 (scanline), 3 (ogre+scanline)>] [--optim <0 (gauss newton), 1 (levenberg marquart)>] [--test-pose] [--init-by-click] [--nb-run <nb run>] [--save-images] [--help]\n" << std::endl;
        return 0;
      }
    }

    std::string opt_videoname = std::string(DATA_ROOT_DIR) + "/data/video/" + opt_video + "/" + opt_video + "-%04d.pgm";
    std::string opt_objectname = std::string(DATA_ROOT_DIR) + "/data/model/" + opt_model + "/" + opt_model;

    if (opt_video == "live") {
      opt_nb_run = 1;
      opt_init_by_click = true;
    }
    
    std::cout << "Video name: " << opt_videoname << std::endl;
    std::cout << "Tracker requested config files: " << opt_objectname
              << ".[init, xml, cao or wrl]" << std::endl;
    std::cout << "Tracker optional config files: " << opt_objectname << ".[ppm]" << std::endl;
    std::cout << "Visibility: " << opt_visibility << std::endl;
    std::cout << "Tracker: " << opt_tracker << std::endl;
    std::cout << "Optimization: " << opt_optim << std::endl;

    vpImage<unsigned char> I;
    vpCameraParameters cam;
    vpHomogeneousMatrix cMo;

    vpMbTracker *tracker = NULL;
    switch(opt_tracker) {
    case MBT_Edge:     tracker = new vpMbEdgeTracker;   break;
    case MBT_Keypoint: tracker = new vpMbKltTracker;    break;
    case MBT_Hybrid:   tracker = new vpMbEdgeKltTracker; break;
    }
    
    if(vpIoTools::checkFilename(opt_objectname + ".xml")) {
      tracker->loadConfigFile(opt_objectname + ".xml");
    }

    tracker->setOgreVisibilityTest(false);
    tracker->setScanLineVisibilityTest(false);
    if (opt_visibility == Visibility_ogre || opt_visibility == Visibility_ogre_scanline)
      tracker->setOgreVisibilityTest(true);
    if (opt_visibility == Visibility_scanline || opt_visibility == Visibility_ogre_scanline)
      tracker->setScanLineVisibilityTest(true);
    
    tracker->loadModel(opt_objectname + ".cao");
    tracker->setDisplayFeatures(true);
    tracker->setOptimizationMethod(opt_optim);

    vpDisplay *display = NULL;
    vpVideoReader *g = NULL;
    vp1394TwoGrabber *glive = NULL;
    for(unsigned int run=0; run < opt_nb_run; run++) {
      int iter =0;
      bool is_initialized = false;

      if (opt_video != "live") {
        g = new vpVideoReader;
        g->setFileName(opt_videoname);
        g->open(I);
      }
      else {
        glive = new vp1394TwoGrabber(false);
        glive->open(I);
      }

      std::ofstream os;
      std::string logfile = std::string(DATA_ROOT_DIR) + std::string("/log");
      vpIoTools::makeDirectory(logfile);
      std::string test_folder = getTestFolder(argv);
      logfile += std::string("/") + test_folder;
      vpIoTools::makeDirectory(logfile);
      std::string test_name = getTestName(opt_video, opt_model,
                                          (int)opt_tracker, (int)opt_visibility, (int)opt_optim);
      logfile += std::string("/") + test_name;
      os.open(logfile.c_str());

      if (! opt_turn_off_display && display == NULL) {
#if defined(VISP_HAVE_X11)
        display = new vpDisplayX;
#elif defined(VISP_HAVE_GDI)
        display = new vpDisplayGDI;
#elif defined(VISP_HAVE_OPENCV)
        display = new vpDisplayOpenCV;
#endif
        display->init(I,100,100,"Model-based tracker");
      }

      while(1) {
        if (opt_video != "live") {
          if (g->end())
            break;
          g->acquire(I);
        }
        else {
          glive->acquire(I);
        }
//        char name[100];
//        sprintf(name, "tabasco-box-%04d.pgm", iter++);
//        vpImageIo::write(I, name);
        vpDisplay::display(I);
        if(! is_initialized) {
          if (opt_init_by_click)
            tracker->initClick(I, opt_objectname + ".init", true);
          else
            tracker->initFromPose(I, opt_objectname + ".0.pos");
          is_initialized = true;
        }
        tracker->track(I);
        tracker->getPose(cMo);
        tracker->getCameraParameters(cam);
        tracker->display(I, cMo, cam, vpColor::red, 2, true);
        vpDisplay::displayFrame(I, cMo, cam, 0.025, vpColor::none, 3);
        if (! opt_save_images)
          vpDisplay::displayText(I, 10, 10, "A click to exit...", vpColor::red);
        vpDisplay::flush(I);

        {
          std::cout << "process image: " << g->getFrameIndex()-1 << std::endl;
          vpMbHiddenFaces<vpMbtPolygon> &faces = tracker->getFaces();
          std::cout << "Number of faces: " << faces.size() << std::endl;
          faces.computeClippedPolygons(cMo, cam);
          std::vector<vpMbtPolygon*> &poly = faces.getPolygon();
          for (unsigned int i=0; i < poly.size(); i++) {
            if (poly[i]->polyClipped.size() == 0)
                std::cout << "Face is not in the image" << std::endl;
          }
        }

        vpPoseVector pose(tracker->getPose());
        if (opt_video != "live")
          os << "frame " << g->getFrameIndex()-1 << " " << pose.t() << std::endl;

        if (opt_save_images){
          char fout[100];
          sprintf(fout, "%s-%04ld.jpeg", opt_video.c_str(), g->getFrameIndex()-1);
          vpImage<vpRGBa> O;
          vpDisplay::getImage(I, O);
          vpImageIo::write(O, fout);
        }
        if (vpDisplay::getClick(I, false))
          break;
      }

      if (opt_video != "live")
        delete g;
      else
        delete glive;

      os.close();
      std::cout << "Results are saved in: \"" << logfile << "\"" << std::endl;

      if (opt_test_pose) {
        std::string benchfile = std::string(DATA_ROOT_DIR) + std::string("/data/bench/") + test_folder;
        benchfile += std::string("/") + test_name;
        std::cout << "Compare results with: \"" << benchfile << "\"" << std::endl;
        bool success = compareFiles(logfile, benchfile);
        if (success) {
          std::cout << "Test succeed" << std::endl;
        }
        else {
          std::cout << "Test failed during run " << run << std::endl;
          return 1;
        }
      }
    }

    if (! opt_turn_off_display) {
      //vpDisplay::getClick(I);
      delete display;
    }

    vpXmlParser::cleanup();
#if defined(VISP_HAVE_COIN3D) && (COIN_MAJOR_VERSION == 3)
    SoDB::finish();
#endif
    delete tracker;

    return 0;
  }
  catch(vpException e) {
    std::cout << "Catch an exception: " << e << std::endl;
    return 1; // to make ctest fail
  }
#else
  (void)argc;
  (void)argv;
  std::cout << "Install OpenCV + libxml2 and rebuild ViSP to use this example." << std::endl;
#endif
}
