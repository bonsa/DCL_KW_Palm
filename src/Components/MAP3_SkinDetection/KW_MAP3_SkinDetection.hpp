/*!
 * \file KW_MAP3_SkinDetection.hpp
 * \brief
 * \author kwasak
 * \date 2010-04-29
 */

#ifndef KW_MAP3_DETECTION_HPP_
#define KW_MAP3_SKIN_DETECTION_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"
#include "Property.hpp"

#include <Types/stream_OpenCV.hpp>

#include <cv.h>
#include <highgui.h>

#include <sstream>

namespace Processors {
namespace KW_MAP3_Skin {

using namespace cv;

/*!
 * \brief KW_Skin_Detection_TSLn properties
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

protected:

};

/*!
 * \class KW_MAP3_SkinDetection
 * \brief Example processor class.
 */
class KW_MAP3_SkinDetection: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	KW_MAP3_SkinDetection(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~KW_MAP3_SkinDetection();

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

	/// Event handler.
	Base::EventHandler <KW_MAP3_SkinDetection> h_onNewImage;

	/// Input image
	Base::DataStreamIn <Mat> in_img;


	/// Event raised, when image is processed
	Base::Event * newImage;

	/// Output data stream - skin part
	Base::DataStreamOut <Mat> out_img;


	/// Properties
	Props props;

private:
	cv::Mat skin_img;

	Base::Property<int> MIN_T;
	Base::Property<int> MAX_T;

	Base::Property<int> MIN_S;
	Base::Property<int> MAX_S;

	Base::Property<int> MIN_L;
	Base::Property<int> MAX_L;



};

}//: namespace KW_Skin_TSL2
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("KW_MAP3_SkinDetection", Processors::KW_MAP3_Skin::KW_MAP3_SkinDetection, Common::Panel_Empty)

#endif /* KW_MAP3_SKIN_DETECTION_HPP_ */

