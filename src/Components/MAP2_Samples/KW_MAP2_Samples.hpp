/*!
 * \file KW_MAP2.hpp
 * \brief Estymacja MAP2, uproszczona dłoń, zbieranie próbek
 * \author kwasak
 * \date 2011-04-28
 */

#ifndef KW_MAP2_SAMPLES_HPP_
#define KW_MAP2_SAMPLES_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"

#include <iostream>
#include <fstream>
#include <cv.h>
#include <highgui.h>

#include <vector>
#include "Types/BlobResult.hpp"
#include "Types/DrawableContainer.hpp"

namespace Processors {
namespace KW_MAP2_Samples {

using namespace cv;
using namespace std;

/*!
 * \brief KW_PalmDetection properties
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
 * \class KW_MAP2_Samples
 * \brief Example processor class.
 */



class KW_MAP2_Samples: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	KW_MAP2_Samples(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~KW_MAP2_Samples();

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
	Base::EventHandler <KW_MAP2_Samples> h_onNewImage;


	/*!
	 * Event handler function.
	 */
	void onNewBlobs();

	/// New set of blobs is waiting
	Base::EventHandler <KW_MAP2_Samples> h_onNewBlobs;

	/*!
	 * Event handler function.
	 */
	void calculate();

	/// Event handler.
	/// New image is waiting
	Base::EventHandler <KW_MAP2_Samples> h_calculate;

	/// Input blobs
	Base::DataStreamIn <Types::Blobs::BlobResult> in_blobs;

	/// Input tsl image
	Base::DataStreamIn <cv::Mat> in_img;

	/// Event raised, when data is processed
	Base::Event * newImage;

	/// Output data stream - list of ellipses around found signs
	Base::DataStreamOut < Types::Blobs::BlobResult > out_signs;

	Base::DataStreamOut < Types::DrawableContainer > out_draw;

	/// Properties
	Props props;


	/*!
	 * Otrzymanie obserwacji dłoni
	 */
	void getObservation();

	/*!
	 * Funkcja obracająca punkt p o kąt angle względem układu współrzędnych znajdującego się w punkcie pO
	 */
	cv::Point rot(cv::Point p, double angle, cv::Point p0);

	// Funkcja wyliczajaca wartosci parametrów stanu na podstawie wartosci obserwacji
	void observationToState();

	//*****************************************************************//
	//*FUNKCJE SRODKOWEGO PALCA****************************************//
	//*****************************************************************//

	// Otrzymanie obserwacji środkowego palca
	void getMiddleFingerObservation();

	// Funkcja wyliczajaca wartosci parametrów stanu środkowego palca na podstawie wartosci obserwacji
	void observationMiddleFingerToState();



	// Otrzymanie obserwacji środkowego palca
	vector <double> getFingerObservation(int i);

	vector <double> observationFingerToState(vector <double> z_Finger,float a, float b);


private:

	cv::Mat tsl_img;
	cv::Mat segments;

	bool blobs_ready;
	bool img_ready;

	// wektor stanu dłoni
	vector<int> idFingertips;

	vector<cv::Point> fingertips;

	// czy funkcja jest pierwszy raz uruchomiana
	bool first;

	// największy blob czyli dłoń
	Types::Blobs::BlobResult blobs;

	// kontener przechowujący elementy do rysowania
	Types::DrawableContainer drawcont;

	// wektor obserwacji dłoni
	vector<double> z;

	//czubek srodkowego palca
	cv::Point topPoint;

	// wektor stanu dłoni
	vector<double> s;

	// wektor obserwacji dłoni
	vector<double> h_z;

	//macierz H
	double H[5][5];

	//macierz przechowująca parametry stanu dla kilku obrazków
	double nStates[5][20];

	//macierz przechowująca punkty char dla kilku obrazków
	double nObservation[5][20];

	// ile obrazków w sekwencji juz task podbrał
	int ileObrazkow;

	//macierz przechowująca parametry stanu dla kilku obrazków
	double meanStates[5];

	//*****************************************************************//
	//*ZMIENNE SRODKOWEGO PALCA****************************************//
	//*****************************************************************//

	// wektor obserwacji dłoni
	vector<double> z_MFinger;

	// wektor stanu dłoni
	vector<double> s_MFinger;

	//macierz przechowująca parametry stanu środkowego palca dla kilku obrazków
	double nStates_MFinger[5][20];

	//macierz przechowująca parametry obserwacji środkowego palca dla kilku obrazków
	double nObservation_MFinger[6][20];

	//macierz przechowująca parametry stanu dla kilku obrazków
	double meanStates_MFinger[5];

	//*****************************************************************//
	//*ZMIENNE PALCA WSKAZUJATEGO**************************************//
	//*****************************************************************//

	// wektor obserwacji dłoni
	vector<double> z_FFinger;

	// wektor stanu dłoni
	vector<double> s_FFinger;

	//macierz przechowująca parametry stanu środkowego palca dla kilku obrazków
	double nStates_FFinger[5][20];

	//macierz przechowująca parametry obserwacji środkowego palca dla kilku obrazków
	double nObservation_FFinger[6][20];

	//macierz przechowująca parametry stanu dla kilku obrazków
	double meanStates_FFinger[5];

	//*****************************************************************//
	//*ZMIENNE KCIUKA**************************************************//
	//*****************************************************************//

	// wektor obserwacji dłoni
	vector<double> z_TFinger;

	// wektor stanu dłoni
	vector<double> s_TFinger;

	//macierz przechowująca parametry stanu środkowego palca dla kilku obrazków
	double nStates_TFinger[5][20];

	//macierz przechowująca parametry obserwacji środkowego palca dla kilku obrazków
	double nObservation_TFinger[6][20];

	//macierz przechowująca parametry stanu dla kilku obrazków
	double meanStates_TFinger[5];

	//*****************************************************************//
	//*ZMIENNE MAŁEGO PALCA********************************************//
	//*****************************************************************//

	// wektor obserwacji dłoni
	vector<double> z_SFinger;

	// wektor stanu dłoni
	vector<double> s_SFinger;

	//macierz przechowująca parametry stanu środkowego palca dla kilku obrazków
	double nStates_SFinger[5][20];

	//macierz przechowująca parametry obserwacji środkowego palca dla kilku obrazków
	double nObservation_SFinger[6][20];

	//macierz przechowująca parametry stanu dla kilku obrazków
	double meanStates_SFinger[5];


	//*****************************************************************//
	//*ZMIENNE PALCA SERDECZNEGO***************************************//
	//*****************************************************************//

	// wektor obserwacji dłoni
	vector<double> z_RFinger;

	// wektor stanu dłoni
	vector<double> s_RFinger;

	//macierz przechowująca parametry stanu środkowego palca dla kilku obrazków
	double nStates_RFinger[5][20];

	//macierz przechowująca parametry obserwacji środkowego palca dla kilku obrazków
	double nObservation_RFinger[6][20];

	//macierz przechowująca parametry stanu dla kilku obrazków
	double meanStates_RFinger[5];
};

}//: namespace KW_MAP2_Samples
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("KW_MAP2_Samples", Processors::KW_MAP2_Samples::KW_MAP2_Samples, Common::Panel_Empty)

#endif /* KW_MAP2_SAMPLES_HPP_ */

