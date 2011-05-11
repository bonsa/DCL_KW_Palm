/*!
 * \file KW_Skin_Detection_TSL2.hpp
 * \brief
 * \author kwasak
 * \date 2010-03-01
 */

#ifndef KW_SKIN_DETECTION_TSL2_HPP_
#define KW_SKIN_DETECTION_TSL2_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"
#include "Property.hpp"
#include "Common/Timer.hpp"

#include <Types/stream_OpenCV.hpp>

#include <cv.h>
#include <highgui.h>

#include <sstream>

namespace Processors {
namespace KW_Skin_TSL2 {

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
 * \class KW_Skin_Detection_TSL2
 * \brief Example processor class.
 */
class KW_Skin_Detection_TSL2: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	KW_Skin_Detection_TSL2(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~KW_Skin_Detection_TSL2();

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
	Base::EventHandler <KW_Skin_Detection_TSL2> h_onNewImage;

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

	Common::Timer timer;

	float total;
	int loops;


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
REGISTER_PROCESSOR_COMPONENT("KW_Skin_Detection_TSL2", Processors::KW_Skin_TSL2::KW_Skin_Detection_TSL2, Common::Panel_Empty)

#endif /* KW_SKIN_DETECTION_TSL2_HPP_ */

