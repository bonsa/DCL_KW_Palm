/*!
 * \file MAP_Estimation.hpp
 * \brief Estymacja MAP, uproszczona dłoń
 * \author kwasak
 * \date 2011-05-12
 */

#ifndef KW_MAP_ESTIMATION_HPP_
#define KW_MAP_ESTIMATION_HPP_

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
namespace MAP_Estimation {

using namespace cv;
using namespace std;

/*!
 * \brief MAP_Estimation properties
 */
struct Props: public Base::Props
{
	bool triggered;
	/*!
	 * \copydoc Base::Props::load
	 */
	void load(const ptree & pt)
	{
		triggered = pt.get("triggered", false);
	}

	/*!
	 * \copydoc Base::Props::save
	 */
	void save(ptree & pt)
	{
		pt.put("triggered", triggered);
	}
};

/*!
 * \class MAP_Estimation
 * \brief Example processor class.
 */

struct finger {

    // wektor obserwacji palca
    vector<double> z;

    // wektor stanu palca
    vector<double> s;

    // poczatkowa hipoteza palca
    vector<double> s0;

    // wektor stanu dłoni obliczona na podstawie aktualnej obserwacji
    vector<double> sTest;

    // wektor obserwacji palca
    vector<double> h_z;

    //macierz H
    double H[5][6];

    //macierz P
    double P[5][5];

    //macierz R
    double R[6][6];

    //macierz odwrotna kowariancji P
    double invP[5][5];

    //macierz odwrotna kowariancji R
    double invR[6][6];

    // różnica punktów charakterystycznych estymacji i punktów aktualnego obrazka
    vector<double> diff;

};


class MAP_Estimation: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	MAP_Estimation(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~MAP_Estimation();

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
	Base::EventHandler <MAP_Estimation> h_onNewImage;


	/*!
	 * Event handler function.
	 */
	void onNewBlobs();

	/// New set of blobs is waiting
	Base::EventHandler <MAP_Estimation> h_onNewBlobs;

	/*!
	 * Event handler function.
	 */
	void map();

	/// Event handler.
	/// New image is waiting
	Base::EventHandler <MAP_Estimation> h_map;

	/*!
	 * Event handler function.
	 */
	void onTrigger();

	/// Event handler.
	Base::EventHandler<MAP_Estimation> h_onTrigger;


	/// Input blobs
	Base::DataStreamIn <Types::Blobs::BlobResult> in_blobs;

	/// Input tsl image
	Base::DataStreamIn <cv::Mat> in_img;

	/// Event raised, when data is processed
	Base::Event * newImage;

	Base::DataStreamOut < Types::DrawableContainer > out_draw;

	//inicjalizacja stanów poczatkowych - hipotezy
	void statesInit();

	//inizjalizacja wartosci poczatkowych macierz P, R, invR, invP
	void matrixInit();

	/*!
	 * Otrzymanie obserwacji dłoni
	 */
	void getObservation();

	void projectionFingertips();

	/*!
	 * Funkcja obracająca punkt p o kąt angle względem układu współrzędnych znajdującego się w punkcie pO
	 */
	cv::Point rot(cv::Point p, double angle, cv::Point p0);

	// Funckja odpowiedzialna za projekcie obserwacji
	void projectionObservation(vector<double> z, int R, int G, int B);

	// Funkcja wyliczajaca wartosci parametrów stanu na podstawie wartosci obserwacji
	void observationToState();

	// Funckja odpowiedzialna za projekcie stanu
	void projectionState(vector<double> s,int R, int G, int B);

	// Funkcja wyliczajaca wartosci parametrów obserwacji na podstawie wartosci obserwacji
	void stateToObservation();

	// Funkcja obliczająca o jaki wektor nalezy zaktualizowac wektor stan
	void calculateDiff();

	// Funckja aktualizująca wektor stanu i macierz kowariancji P
	void updateState();

	// Funkcja sprawdzająca czy warunek końcowy jest sprawdzony
	void stopCondition();

	// obliczanie średnich wartosci obserwacji dłoni na podstawie parametrow stanow całej dloni(wew dlon i place)
	void updateObservation();


	//*****************************************************************//
	//*FUNKCJE wspolne dla PALCÓW****************************************//
	//*****************************************************************//

	// Otrzymanie obserwacji środkowego palca
	vector <double> getFingerObservation(int i);

	// Funckja odpowiedzialna za projekcie obserwacji palca
	void projectionFingerObservation(vector<double> z, int R, int G, int B);

	vector <double> observationFingerToState(vector <double> z_Finger,float a, float b);

	void projectionFingerState(vector<double> s, int R, int G, int B);

	// Funkcja wyliczajaca wartosci parametrów obserwacji na podstawie wartosci obserwacji
	vector <double> stateFingerToObservation(vector <double> s_Finger, float a);

	void calculateFingerH(vector<double> s_Finger, double H_Finger[5][6], float a);

	vector <double> calculateFingerDiff(vector <double> h_z, vector <double> z, double invR[6][6], double H[5][6], double P[5][5], double factor2);

	vector <double> updateFingerState(vector <double> diff_Finger, vector <double> s_Finger, double P_Finger[5][5]);

	bool stopCondition(vector <double> s, vector <double> s0, double invP[5][5], vector <double> diff, float limit);


private:

	bool MAP;

	double MaxArea;

	/// MAP properties
	Props props;

	double error;

	double error2;

	cv::Mat tsl_img;
	cv::Mat segments;

	bool blobs_ready;
	bool img_ready;

	// wektor stanu dłoni
	vector<int> idFingertips;

	vector<cv::Point> fingertips;

	bool newFingertip;

	// największy blob czyli dłoń
	Types::Blobs::BlobResult blobs;

	// kontener przechowujący elementy do rysowania
	Types::DrawableContainer drawcont;

	//wspołczynnik zapominania
	double factor;

	//wspołczynnik skalujacy wielkosc zmiany parametrów stanu
	double factorFinger;

	double factorPalm;

	// funkcja warunek stopu, jesli STOP = true estymacja MAP jest zakończona
	bool STOP;

	/***************************************************************/
	/*Dłon WEW*/

	// wektor obserwacji dłoni
	vector<double> z;

	double MaxY, MinY,  MaxX, MinX;

	//czubek srodkowego palca
	cv::Point topPoint;

	// wektor stanu dłoni
	vector<double> s;

	// poczatkowa hipoteza
	vector<double> s0;

	// wektor stanu dłoni obliczona na podstawie aktualnej obserwacji
	vector<double> sTest;

	// wektor obserwacji dłoni
	vector<double> h_z;

	//macierz H
	double H[5][5];

	//macierz P
	double P[5][5];

	//macierz R
	double R[5][5];

	//macierz odwrotna kowariancji P
	double invP[5][5];

	//macierz odwrotna kowariancji R
	double invR[5][5];

	// różnica punktów charakterystycznych estymacji i punktów aktualnego obrazka
	vector<double> diff;


	//*****************************************************************//
	//*ZMIENNE SRODKOWEGO PALCA****************************************//
	//*****************************************************************//

	finger MFinger;

	//*****************************************************************//
	//*ZMIENNE PALCA WSKAZUJĄCEG0**************************************//
	//*****************************************************************//

	finger FFinger;

	//*****************************************************************//
	//*ZMIENNE KCIUKA**************************************************//
	//*****************************************************************//

	finger TFinger;

	//*****************************************************************//
	//*ZMIENNE MAŁEGO PALCA********************************************//
	//*****************************************************************//

	finger SFinger;

	//*****************************************************************//
	//*ZMIENNE PALEC SERDECZNY*****************************************//
	//*****************************************************************//

	finger RFinger;

	//***********
	double z0, z1, z4;



};

}//: namespace MAP_Estimation
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("MAP_Estimation", Processors::MAP_Estimation::MAP_Estimation, Common::Panel_Empty)

#endif /* KW_MAP_ESTIMATION_HPP_ */

