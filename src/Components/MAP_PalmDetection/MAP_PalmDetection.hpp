/*!
 * \file KW_MAP2.hpp
 * \brief Identyfikacja największego bloba przekraczajacego minimalny rozmiar jako dłon
 * \author kwasak
 * \date 2011-05-12
 */

#ifndef MAP_PALM_DETECTION_HPP_
#define MAP_PALM_DETECTION_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"
#include "Property.hpp"

#include <iostream>
#include <fstream>
#include <cv.h>
#include <highgui.h>

#include <vector>
#include "Types/BlobResult.hpp"
#include "Types/DrawableContainer.hpp"

namespace Processors {
namespace MAP_PalmDetection {

using namespace cv;
using namespace std;

/*!
 * \brief MAP_PalmDetectionn properties
 */
struct Props: public Base::Props
{
	/*!
	 * \copydoc Base::Props::load
	 */
	void load(const ptree & pt)
	{		
	}

	/*!
	 * \copydoc Base::Props::save
	 */
	void save(ptree & pt)
	{
	}
};

/*!
 * \class MAP_PalmDetection
 * \brief Example processor class.
 */



class MAP_PalmDetection: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	MAP_PalmDetection(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~MAP_PalmDetection();

	/*!
	 * Return window properties
	 */
	Base::Props * getProperties()
	{
		return &props;
	}

protected:

	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Retrieves data from device.
	 */
	bool onStep();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();	
	
	/*!
	 * Event handler function.
	 */
	void onNewImage();

	/// New image is waiting
	Base::EventHandler <MAP_PalmDetection> h_onNewImage;


	/*!
	 * Event handler function.
	 */
	void onNewBlobs();

	/// New set of blobs is waiting
	Base::EventHandler <MAP_PalmDetection> h_onNewBlobs;


	/// Input blobs
	Base::DataStreamIn <Types::Blobs::BlobResult> in_blobs;

	/// Input tsl image
	Base::DataStreamIn <cv::Mat> in_img;

	/// Event raised, when data is processed
	Base::Event * newImage;

	/// Output data stream - list of ellipses around found signs
	Base::DataStreamOut < Types::Blobs::BlobResult > out_blobs1;

	/// Output data stream - list of ellipses around found signs
	Base::DataStreamOut < Types::Blobs::BlobResult > out_blobs2;


	/*!
	 * Otrzymanie obserwacji dłoni
	 */

	void getObservation();


private:

	double MaxArea;
	bool PALM;

	/// MAP properties
	Props props;

	cv::Mat tsl_img;

	bool blobs_ready;
	bool img_ready;

	// największy blob czyli dłoń
	Types::Blobs::BlobResult blobs;


};

}//: namespace MAP_PalmDetection
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("MAP_PalmDetection", Processors::MAP_PalmDetection::MAP_PalmDetection, Common::Panel_Empty)

#endif /* KW_MAP_PalmDetection_HPP_ */

