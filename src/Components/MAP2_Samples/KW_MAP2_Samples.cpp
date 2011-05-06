/*!
 * \file KW_MAP2_Samples.cpp
 * \brief Estymacja MAP2 - zbieranie probek  
 * \date 2011-04-26
 */

#include <memory>
#include <string>
#include <math.h> 

#include "KW_MAP2_Samples.hpp"
#include "Logger.hpp"
#include "Types/Ellipse.hpp"
#include "Types/Line.hpp"
#include "Types/Rectangle.hpp"
#include <vector>
#include <iomanip>

namespace Processors {
namespace KW_MAP2_Samples {

using namespace cv;

KW_MAP2_Samples::~KW_MAP2_Samples() {
	LOG(LTRACE) << "Good bye KW_MAP2\n";

	ileObrazkow = 0;
}

bool KW_MAP2_Samples::onInit() {
	LOG(LTRACE) << "KW_MAP2::initialize\n";

	h_onNewImage.setup(this, &KW_MAP2_Samples::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	h_onNewBlobs.setup(this, &KW_MAP2_Samples::onNewBlobs);
	registerHandler("onNewBlobs", &h_onNewBlobs);

	h_calculate.setup(this, &KW_MAP2_Samples::calculate);
	registerHandler("calculate", &h_calculate);


	registerStream("in_blobs", &in_blobs);
	registerStream("in_img", &in_img);

	newImage = registerEvent("newImage");

	registerStream("out_signs", &out_signs);
	registerStream("out_draw", &out_draw);


	//pierwsze uruchomienie komponentu
	first = true;

	return true;
}

bool KW_MAP2_Samples::onFinish() {
	LOG(LTRACE) << "KW_MAP2_Samples::finish\n";

	return true;
}

bool KW_MAP2_Samples::onStep() {
	LOG(LTRACE) << "KW_MAP2_Samples::step\n";

	blobs_ready = img_ready = false;

	try {

		ileObrazkow = ileObrazkow + 1;
		cout<<"ilosc obrazkow"<<ileObrazkow<<"\n" ;

		drawcont.clear();

		//czubki palców
		fingertips.clear();
		idFingertips.clear();

		z.clear();
		s.clear();
		h_z.clear();
		z_MFinger.clear();
		s_MFinger.clear();

		getObservation();
		observationToState();

		//środkowy palec
		z_MFinger = getFingerObservation(2);

		nObservation_MFinger[0][ileObrazkow -1] = z_MFinger[0];
		nObservation_MFinger[1][ileObrazkow -1] = z_MFinger[1];
		nObservation_MFinger[2][ileObrazkow -1] = z_MFinger[2];
		nObservation_MFinger[3][ileObrazkow -1] = z_MFinger[3];
		nObservation_MFinger[4][ileObrazkow -1] = z_MFinger[4];
		nObservation_MFinger[5][ileObrazkow -1] = z_MFinger[5];

		s_MFinger = observationFingerToState(z_MFinger, 0.7, 0.6);

		nStates_MFinger[0][ileObrazkow -1] = s_MFinger[0];
		nStates_MFinger[1][ileObrazkow -1] = s_MFinger[1];
		nStates_MFinger[2][ileObrazkow -1] = s_MFinger[2];
		nStates_MFinger[3][ileObrazkow -1] = s_MFinger[3];
		nStates_MFinger[4][ileObrazkow -1] = s_MFinger[4];

		//palec wskazujacy
		z_FFinger = getFingerObservation(3);

		nObservation_FFinger[0][ileObrazkow -1] = z_FFinger[0];
		nObservation_FFinger[1][ileObrazkow -1] = z_FFinger[1];
		nObservation_FFinger[2][ileObrazkow -1] = z_FFinger[2];
		nObservation_FFinger[3][ileObrazkow -1] = z_FFinger[3];
		nObservation_FFinger[4][ileObrazkow -1] = z_FFinger[4];
		nObservation_FFinger[5][ileObrazkow -1] = z_FFinger[5];

		s_FFinger = observationFingerToState(z_FFinger, 0.72, 0.56);

		nStates_FFinger[0][ileObrazkow -1] = s_FFinger[0];
		nStates_FFinger[1][ileObrazkow -1] = s_FFinger[1];
		nStates_FFinger[2][ileObrazkow -1] = s_FFinger[2];
		nStates_FFinger[3][ileObrazkow -1] = s_FFinger[3];
		nStates_FFinger[4][ileObrazkow -1] = s_FFinger[4];

		//kciuk
		z_TFinger = getFingerObservation(4);

		nObservation_TFinger[0][ileObrazkow -1] = z_TFinger[0];
		nObservation_TFinger[1][ileObrazkow -1] = z_TFinger[1];
		nObservation_TFinger[2][ileObrazkow -1] = z_TFinger[2];
		nObservation_TFinger[3][ileObrazkow -1] = z_TFinger[3];
		nObservation_TFinger[4][ileObrazkow -1] = z_TFinger[4];
		nObservation_TFinger[5][ileObrazkow -1] = z_TFinger[5];

		s_TFinger = observationFingerToState(z_TFinger, 0.72, 0.56);

		nStates_TFinger[0][ileObrazkow -1] = s_TFinger[0];
		nStates_TFinger[1][ileObrazkow -1] = s_TFinger[1];
		nStates_TFinger[2][ileObrazkow -1] = s_TFinger[2];
		nStates_TFinger[3][ileObrazkow -1] = s_TFinger[3];
		nStates_TFinger[4][ileObrazkow -1] = s_TFinger[4];

		//mały palec
		z_SFinger = getFingerObservation(0);

		nObservation_SFinger[0][ileObrazkow -1] = z_SFinger[0];
		nObservation_SFinger[1][ileObrazkow -1] = z_SFinger[1];
		nObservation_SFinger[2][ileObrazkow -1] = z_SFinger[2];
		nObservation_SFinger[3][ileObrazkow -1] = z_SFinger[3];
		nObservation_SFinger[4][ileObrazkow -1] = z_SFinger[4];
		nObservation_SFinger[5][ileObrazkow -1] = z_SFinger[5];

		s_SFinger = observationFingerToState(z_SFinger, 0.82, 0.36);

		nStates_SFinger[0][ileObrazkow -1] = s_SFinger[0];
		nStates_SFinger[1][ileObrazkow -1] = s_SFinger[1];
		nStates_SFinger[2][ileObrazkow -1] = s_SFinger[2];
		nStates_SFinger[3][ileObrazkow -1] = s_SFinger[3];
		nStates_SFinger[4][ileObrazkow -1] = s_SFinger[4];

		//palec serdeczny
		z_RFinger = getFingerObservation(1);

		nObservation_RFinger[0][ileObrazkow -1] = z_RFinger[0];
		nObservation_RFinger[1][ileObrazkow -1] = z_RFinger[1];
		nObservation_RFinger[2][ileObrazkow -1] = z_RFinger[2];
		nObservation_RFinger[3][ileObrazkow -1] = z_RFinger[3];
		nObservation_RFinger[4][ileObrazkow -1] = z_RFinger[4];
		nObservation_RFinger[5][ileObrazkow -1] = z_RFinger[5];

		s_RFinger = observationFingerToState(z_RFinger, 0.73, 0.54);

		nStates_RFinger[0][ileObrazkow -1] = s_RFinger[0];
		nStates_RFinger[1][ileObrazkow -1] = s_RFinger[1];
		nStates_RFinger[2][ileObrazkow -1] = s_RFinger[2];
		nStates_RFinger[3][ileObrazkow -1] = s_RFinger[3];
		nStates_RFinger[4][ileObrazkow -1] = s_RFinger[4];


		out_draw.write(drawcont);
		newImage->raise();

		return true;
	} catch (...) {
		LOG(LERROR) << "KW_MAP2_Samples::getCharPoints failed\n";
		return false;
	}
}


bool KW_MAP2_Samples::onStop() {
	return true;
}

bool KW_MAP2_Samples::onStart() {
	return true;
}

void KW_MAP2_Samples::onNewImage() {
	LOG(LTRACE) << "KW_MAP2_Samples::onNewImage\n";

	img_ready = true;
	tsl_img = in_img.read();
	//co robi tak linijka?
	tsl_img = tsl_img.clone();
	if (blobs_ready && img_ready)
		onStep();
}

void KW_MAP2_Samples::onNewBlobs() {
	LOG(LTRACE) << "KW_MAP2_Samples::onNewBlobs\n";

	blobs_ready = true;
	blobs = in_blobs.read();
	if (blobs_ready && img_ready)
		onStep();
}

void KW_MAP2_Samples::getObservation(){

	LOG(LTRACE) << "KW_MAP2_Samples::getCharPoints\n";

	try {

		// id największego bloba, czyli dłoni
		int id = 0;
		// aktualnie pobrany blob
		Types::Blobs::Blob *currentBlob;
		// wynikowy blob
		Types::Blobs::BlobResult result;
		// punkty znajdujace sie na konturze
		CvSeq * contour;
		// czyta elementy na konturu
		CvSeqReader reader;
		// punkt, na którym aktualnie jest wykonywana operacja
		cv::Point actualPoint;
		// obrocony actualPoint o kat nachylania dłoni wzgledem układu wspolrzędnych w punkcie środa masy
		cv::Point tempPoint;
		// wektor zawierający punkty konturu
		vector<cv::Point> contourPoints;
		//zmienna pomocnicza
		float TempDist;
		//zapamietuje poprzedni znak różnicy miedzy punktami,
		//1- funkcja jest rosnoca, -1 - funkcja malejąca
		int lastSign;
		int lastMinDist;
		//idenksy punktów charakterystycznych;
		//powyzej tej odległości od środa cieżkosci moga znajdować sie ekstrema
		int MINDIST;
		//id ostatenio wyznaczonego ekstremum
		int idLastExtreme;

		Types::DrawableContainer signs;

		//momenty goemetryczne potrzebne do obliczenia środka ciężkości
		double m00, m10, m01;
		//powierzchnia bloba, powiedzchnia największego bloba, współrzędne środka ciężkości, maksymalna wartośc współrzędnej Y
		double Area, MaxArea, CenterOfGravity_x, CenterOfGravity_y, MaxY, MinY,  MaxX, MinX;

		double height, width;
		MaxArea = 0;

		//największy blob to dłoń
		for (int i = 0; i < blobs.GetNumBlobs(); i++) {
			currentBlob = blobs.GetBlob(i);
			cout<<"i_"<<i<<"\n";

			Area = currentBlob->Area();
			//szukanie bloba o największej powierzchni
			if (Area > MaxArea) {
				MaxArea = Area;
				// id największego bloba, czyli dłoni
				id = i;
				cout<<"i_"<<i<<"\n";
			}
		}
		//current Blob przychowuje największego bloba, czyli dłoni
		currentBlob = blobs.GetBlob(id);
		cout<<"id"<<id<<"\n";

		// calculate moments
		m00 = currentBlob->Moment(0, 0);
		m01 = currentBlob->Moment(0, 1);
		m10 = currentBlob->Moment(1, 0);

		MinX = currentBlob->MinX();
		MaxX = currentBlob->MaxX();
		MinY = currentBlob->MinY();
		MaxY = currentBlob->MaxY();

		//obliczenie środka cięzkości
		CenterOfGravity_x = m10 / m00;
		CenterOfGravity_y = m01 / m00;

		z.push_back(CenterOfGravity_x);
		z.push_back(CenterOfGravity_y);

		Types::Ellipse * elE;
		elE = new Types::Ellipse(Point2f(CenterOfGravity_x, CenterOfGravity_y), Size2f(10, 10));
		elE->setCol(CV_RGB(0,255,0));
		drawcont.add(elE);


		//kontur największego bloba
		contour = currentBlob->GetExternalContour()->GetContourPoints();
		cvStartReadSeq(contour, &reader);
		CV_READ_SEQ_ELEM( actualPoint, reader);
		topPoint = actualPoint;
		idFingertips.push_back(0);

		for (int j = 0; j < contour->total; j = j + 1) {

			CV_READ_SEQ_ELEM( actualPoint, reader);

			if (j % 20 == 1)
			{
				//wpisanie punktów z konturu do wektora
				contourPoints.push_back(cv::Point(actualPoint.x, actualPoint.y));
			}
		}

		MINDIST = (MaxY - CenterOfGravity_y) * (MaxY - CenterOfGravity_y) * 4/ 9;

		//środek cieżkości przesuwał troche w dół ekranu, aby ułatwić wyznaczanie punktów charakterystycznych
		CenterOfGravity_y += (MaxY - CenterOfGravity_y) * 2 / 3;


		//******************************************************************
		//obliczenie roznicy miedzy punktami konturu a przesuniętym środkiem ciężkosci
		vector<double> dist;
		//1 -oznacza, że ostatni element z konturu należał do dolnej czesci dłoni
		lastMinDist = 0;
		idLastExtreme = 0;
		lastSign = 0; //ta linijka nic nie znaczy, nie chce miec warninga
		for (unsigned int i = 0; i < contourPoints.size(); i++)
		{
			TempDist = (contourPoints[i].x - CenterOfGravity_x)	* (contourPoints[i].x - CenterOfGravity_x) + (contourPoints[i].y - CenterOfGravity_y) * (contourPoints[i].y - CenterOfGravity_y);

			if (TempDist > MINDIST)
				dist.push_back(TempDist);
			else
				//jeśli odległość jest mniejsza niż MINDIST oznacza to, że jest to dolna cześć dłoni i nie znajdują się tam żadnego punkty charakterystyczne poza przesuniętym środkiem ciężkości, dlatego te punkty można ominąć
				dist.push_back(MINDIST);

			if (i == 1)
			{
				if ((dist[1] - dist[0]) > 0)
					lastSign = 1;
				else
					lastSign = -1;
			}
			else if (i > 1)
			{
				if (dist[i] > MINDIST && dist[i - 1] > MINDIST)
				{

					//jeżeli ostatnio była wykryta dolna cześci dłoni, następnym charakterystycznych punktem powinien być czubek palca, dlatego lastSign = 1;
					if (lastMinDist == 1)
					{
						lastSign = 1;
						lastMinDist = 0;
					}
					//maksiumum - czubek palca, funkcja rosła i zaczeła maleć
					if ((dist[i] - dist[i-1]) < 0 && lastSign == 1)
					{
						if (((contourPoints[i-1].x - contourPoints[idLastExtreme].x) * (contourPoints[i-1].x - contourPoints[idLastExtreme].x) + (contourPoints[i-1].y - contourPoints[idLastExtreme].y) * (contourPoints[i-1].y - contourPoints[idLastExtreme].y)) > 5000)
						{
							if((dist[i-1]>20000) && (idFingertips.size()<5))
							{
								idFingertips.push_back(i-1);
								lastSign = -1;
								idLastExtreme = i-1;
							}
						}
					}
					//minimum - punkt między palcami
					else if ((dist[i] - dist[i-1]) > 0 && lastSign == -1)
					{
						if (((contourPoints[i-1].x - contourPoints[idLastExtreme].x) * (contourPoints[i-1].x - contourPoints[idLastExtreme].x) + (contourPoints[i-1].y - contourPoints[idLastExtreme].y) * (contourPoints[i-1].y - contourPoints[idLastExtreme].y)) > 5000)
						{
							lastSign = 1;
							idLastExtreme = i-1;
						}
					}
				}
				else
				{
					// element należący do dołu dłoni
					lastMinDist = 1;
				}
			}

		}

		int idLeftPoint = 0;
		int xLeftPoint = 1000000;
		for (unsigned int i = 0; i < idFingertips.size(); i++) {
			//znajdujemy punkt najbardziej wysynięty na lewo, czyli wierzchołek małego palca
			if (xLeftPoint > contourPoints[idFingertips[i]].x)
			{
				xLeftPoint = contourPoints[idFingertips[i]].x;
				idLeftPoint = i;
			}
		}

		for (int i = idLeftPoint; i >= 0; i--) {
			//wpisanie do tablicy punktów charakterystycznych punktów opisujących trzy lewe palce
			fingertips.push_back(cv::Point( contourPoints[idFingertips[i]].x, contourPoints[idFingertips[i]].y));
		}

		for (int i = idFingertips.size() - 1; i > idLeftPoint; i--) {
			//wpisanie do tablicy punktów charakterystycznych punktów opisujących dwa prawe palce
			fingertips.push_back(cv::Point( contourPoints[idFingertips[i]].x,  contourPoints[idFingertips[i]].y));
		}



		double dx = - z[0] + topPoint.x;
		double dy = - z[1] + topPoint.y;

		Types::Ellipse * el;
		el = new Types::Ellipse(Point2f(topPoint.x, topPoint.y), Size2f(10, 10));
		el->setCol(CV_RGB(0,0,0));
		drawcont.add(el);
		//argument kąta nachylenia
		double angle = abs(atan2(dy, dx));

		z.push_back(angle);

		height = MaxY - MinY;
		width = MaxX - MinX;

		z.push_back(height);
		z.push_back(width);

		nObservation[0][ileObrazkow -1] = CenterOfGravity_x;
		nObservation[1][ileObrazkow -1] = CenterOfGravity_y;
		nObservation[2][ileObrazkow -1] = angle;
		nObservation[3][ileObrazkow -1] = height;
		nObservation[4][ileObrazkow -1] = width;

		result.AddBlob(blobs.GetBlob(id));
		out_signs.write(result);

	} catch (...) {
		LOG(LERROR) << "KW_MAP::getCharPoints failed\n";

	}
}

//punkcja obracająca punkt p o kąt angle według układu współrzędnych znajdującym się w punkcie p0
cv::Point KW_MAP2_Samples::rot(cv::Point p, double angle, cv::Point p0) {
	cv::Point t;
	t.x = p0.x + (int) ((double) (p.x - p0.x) * cos(angle) - (double) (p.y - p0.y) * sin(angle));
	t.y = p0.y + (int) ((double) (p.x - p0.x) * sin(angle) + (double) (p.y - p0.y) * cos(angle));
	return t;
}


// Funkcja wyliczajaca wartosci parametru stanu na podstawie wartosci obserwacji
void KW_MAP2_Samples::observationToState()
{
	float s_mx, s_my, s_angle, s_heigth, s_width;

	s_mx = z[0] - 0.025 * z[4];
	s_my = z[1] + 1.0/7.0 * z[3];

	Types::Ellipse * el;

	el = new Types::Ellipse(cv::Point(s_mx, s_my), Size2f(10, 10));
	el->setCol(CV_RGB(255,0,0));
	drawcont.add(el);

	s_angle = z[2];
	s_heigth = 0.4 * z[3];
	s_width = 0.5 * z[4];

	s.push_back(s_mx);
	s.push_back(s_my);
	s.push_back(s_angle);
	s.push_back(s_heigth);
	s.push_back(s_width);

	nStates[0][ileObrazkow -1] = s_mx;
	nStates[1][ileObrazkow -1] = s_my;
	nStates[2][ileObrazkow -1] = s_angle;
	nStates[3][ileObrazkow -1] = s_heigth;
	nStates[4][ileObrazkow -1] = s_width;


}


//*****************************************************************//
//*SRODKOWY PALEC**************************************************//
//*****************************************************************//



// Otrzymanie obserwacji środkowego palca
vector <double> KW_MAP2_Samples::getFingerObservation(int i)
{

	double downX, downY, topX, topY, alfa, w;

	downX = z[0] - 3.0/7.0 * (topPoint.x - z[0]);
	downY = z[1] - 3.0/7.0 * (topPoint.y - z[1]);

	Types::Ellipse * el;

	el = new Types::Ellipse(cv::Point(downX, downY), Size2f(10, 10));
	el->setCol(CV_RGB(255,0,255));
	drawcont.add(el);

	topX = fingertips[i].x;
	topY = fingertips[i].y;

	el = new Types::Ellipse(cv::Point(topX, topY), Size2f(10, 10));
	el->setCol(CV_RGB(255,0,255));
	drawcont.add(el);

	//z jest w stopniach, a alfa ma byc w radianach

	double dx, dy;
	dx = topX - downX;
	dy = topY - downY;

	double angle = abs(atan2(dy, dx));
	alfa = angle;  //kat w radianach
	w = z[4];

	vector <double> z_Finger;

	z_Finger.push_back(downX);
	z_Finger.push_back(downY);
	z_Finger.push_back(topX);
	z_Finger.push_back(topY);
	z_Finger.push_back(alfa);
	z_Finger.push_back(w);

	return z_Finger;
}



vector <double> KW_MAP2_Samples:: observationFingerToState(vector <double> z_Finger, float a, float b)
{

	float s_mx, s_my, s_angle, s_heigth, s_width;

	s_mx = z_Finger[0] + a * (z_Finger[2] - z_Finger[0]);
	s_my = z_Finger[1] + a * (z_Finger[3] - z_Finger[1]);

	s_angle = z_Finger[4];
	s_heigth = b * (sqrt((z_Finger[0]-z_Finger[2])*(z_Finger[0]-z_Finger[2])+(z_Finger[1]-z_Finger[3])*(z_Finger[1]-z_Finger[3])));
	s_width = 0.12 * z_Finger[5];

	vector <double> sTest;
	sTest.push_back(s_mx);
	sTest.push_back(s_my);
	sTest.push_back(s_angle);
	sTest.push_back(s_heigth);
	sTest.push_back(s_width);

	return sTest;

}



// Otrzymanie obserwacji środkowego palca
void KW_MAP2_Samples::getMiddleFingerObservation()
{
	double downX, downY, topX, topY, alfa, w;

	downX = z[0] - 3.0/7.0 * (topPoint.x - z[0]);
	downY = z[1] - 3.0/7.0 * (topPoint.y - z[1]);

	Types::Ellipse * el;

	el = new Types::Ellipse(cv::Point(z[0], z[1]), Size2f(10, 10));
	el->setCol(CV_RGB(255,255,255));
	drawcont.add(el);

	topX = topPoint.x;
	topY = topPoint.y;

	//katy sa w radianach
	alfa = z[2];
	w = z[4];

	z_MFinger.push_back(downX);
	z_MFinger.push_back(downY);
	z_MFinger.push_back(topX);
	z_MFinger.push_back(topY);
	z_MFinger.push_back(alfa);
	z_MFinger.push_back(w);

	nObservation_MFinger[0][ileObrazkow -1] = downX;
	nObservation_MFinger[1][ileObrazkow -1] = downY;
	nObservation_MFinger[2][ileObrazkow -1] = topX;
	nObservation_MFinger[3][ileObrazkow -1] = topY;
	nObservation_MFinger[4][ileObrazkow -1] = alfa;
	nObservation_MFinger[5][ileObrazkow -1] = w;

}


void KW_MAP2_Samples:: observationMiddleFingerToState()
{

	float s_mx, s_my, s_angle, s_heigth, s_width;

	s_mx = z_MFinger[0] + 0.7 * (z_MFinger[2] - z_MFinger[0]);
	s_my = z_MFinger[1] + 0.7 * (z_MFinger[3] - z_MFinger[1]);

	Types::Ellipse * el;

	el = new Types::Ellipse(cv::Point(s_mx, s_my ), Size2f(10, 10));
	el->setCol(CV_RGB(0,255,255));
	drawcont.add(el);

	s_angle = z_MFinger[4];
	s_heigth = 0.6 * (sqrt((z_MFinger[0]-z_MFinger[2])*(z_MFinger[0]-z_MFinger[2])+(z_MFinger[1]-z_MFinger[3])*(z_MFinger[1]-z_MFinger[3])));
	s_width = 0.12 * z_MFinger[5];

	nStates_MFinger[0][ileObrazkow -1] = s_mx;
	nStates_MFinger[1][ileObrazkow -1] = s_my;
	nStates_MFinger[2][ileObrazkow -1] = s_angle;
	nStates_MFinger[3][ileObrazkow -1] = s_heigth;
	nStates_MFinger[4][ileObrazkow -1] = s_width;

}


void KW_MAP2_Samples::calculate()
{
	//zapisywanie macierzy P, invP, R, invR do pliku, tworzenie pliku
	std::ofstream plik("/home/kasia/Test.txt");

	plik<<"\n ";
	plik<<"RSamples\n ";

	plik<<"R = [\n ";
	for (int i = 0; i< 5; i++)
	{
		for(int j = 0; j< ileObrazkow; j++)
		{
			plik<<setprecision(5)<<nObservation[i][j]<<" \t";
		}
		plik<<";";
	}
	plik<<"]";


	//dla Kasprzaka

	plik<<"\n\nR = \n";
	for (int i = 0; i< 5; i++)
	{
		for(int j = 0; j< ileObrazkow; j++)
		{
			plik<<setprecision(4)<<nObservation[i][j]<<" \t";
		}
		plik<<"\n";
	}
	plik<<"\n";

	plik<<"\n ";
	plik<<"PSamples\n ";

	plik<<"P = [\n ";
	for (int i = 0; i< 5; i++)
	{
		for(int j = 0; j< ileObrazkow; j++)
		{
			plik<<setprecision(5)<<nStates[i][j]<<" \t";
		}
		plik<<";";
	}
	plik<<"]\n\n";

	// dla KAsprzaka
	plik<<"P = \n";
	for (int i = 0; i< 5; i++)
	{
		for(int j = 0; j< ileObrazkow; j++)
		{
			plik<<setprecision(4)<<nStates[i][j]<<" \t";
		}
		plik<<"\n";
	}
	plik<<"\n\n";

	for (unsigned int i = 0 ; i < 5; i++)
	{
		for (int j = 0; j < ileObrazkow; j++)
		{
			meanStates[i] += nStates[i][j];
		}
		meanStates[i] /= ileObrazkow;
		plik<<"meanStates["<<i<<"] = "<<meanStates[i]<<";\n";
	}

	//*****************************************************************//
	//*ZMIENNE SRODKOWEGO PALCA****************************************//
	//*****************************************************************//


		plik<<"\n ";
		plik<<"RSamples_MFinger\n ";

		plik<<"R_MFinger = [\n ";
		for (int i = 0; i< 6; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nObservation_MFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]";

		plik<<"\n ";
		plik<<"PSamples_MFinger\n ";

		plik<<"P_MFinger = [\n ";
		for (int i = 0; i< 5; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nStates_MFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]\n\n";

		for (unsigned int i = 0 ; i < 5; i++)
		{
			for (int j = 0; j < ileObrazkow; j++)
			{
				meanStates_MFinger[i] += nStates_MFinger[i][j];
			}
			meanStates_MFinger[i] /= ileObrazkow;
			plik<<"meanStates_MFinger["<<i<<"] = "<<meanStates_MFinger[i]<<";\n";
		}

	//*****************************************************************//
	//*ZMIENNE PALCA WSKAZUJACEGO**************************************//
	//*****************************************************************//


		plik<<"\n ";
		plik<<"RSamples_FFinger\n ";

		plik<<"R_FFinger = [\n ";
		for (int i = 0; i< 6; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nObservation_FFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]";

		plik<<"\n ";
		plik<<"PSamples_FFinger\n ";

		plik<<"P_FFinger = [\n ";
		for (int i = 0; i< 5; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nStates_FFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]\n\n";

		for (unsigned int i = 0 ; i < 5; i++)
		{
			for (int j = 0; j < ileObrazkow; j++)
			{
				meanStates_FFinger[i] += nStates_FFinger[i][j];
			}
			meanStates_FFinger[i] /= ileObrazkow;
			plik<<"meanStates_FFinger["<<i<<"] = "<<meanStates_FFinger[i]<<";\n";
		}

	//*****************************************************************//
	//*ZMIENNE KCIUKA**************************************************//
	//*****************************************************************//


		plik<<"\n ";
		plik<<"RSamples_TFinger\n ";

		plik<<"R_TFinger = [\n ";
		for (int i = 0; i< 6; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nObservation_TFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]";

		plik<<"\n ";
		plik<<"PSamples_TFinger\n ";

		plik<<"P_TFinger = [\n ";
		for (int i = 0; i< 5; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nStates_FFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]\n\n";

		for (unsigned int i = 0 ; i < 5; i++)
		{
			for (int j = 0; j < ileObrazkow; j++)
			{
				meanStates_TFinger[i] += nStates_TFinger[i][j];
			}
			meanStates_TFinger[i] /= ileObrazkow;
			plik<<"meanStates_TFinger["<<i<<"] = "<<meanStates_TFinger[i]<<";\n";
		}

	//*****************************************************************//
	//*ZMIENNE MAŁEGO PALCA*********************************************//
	//*****************************************************************//


		plik<<"\n ";
		plik<<"RSamples_SFinger\n ";

		plik<<"R_SFinger = [\n ";
		for (int i = 0; i< 6; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nObservation_SFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]";

		plik<<"\n ";
		plik<<"PSamples_SFinger\n ";

		plik<<"P_SFinger = [\n ";
		for (int i = 0; i< 5; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nStates_SFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]\n\n";

		for (unsigned int i = 0 ; i < 5; i++)
		{
			for (int j = 0; j < ileObrazkow; j++)
			{
				meanStates_SFinger[i] += nStates_SFinger[i][j];
			}
			meanStates_SFinger[i] /= ileObrazkow;
			plik<<"meanStates_SFinger["<<i<<"] = "<<meanStates_SFinger[i]<<";\n";
		}

	//*****************************************************************//
	//*ZMIENNE PALCA SERDECZNEGO***************************************//
	//*****************************************************************//


		plik<<"\n ";
		plik<<"RSamples_RFinger\n ";

		plik<<"R_RFinger = [\n ";
		for (int i = 0; i< 6; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nObservation_RFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]";
		// dla prof. Kasprzaka
		plik<<"RSamples_RFinger\n ";
		plik<<"R_RFinger = \n";
		for (int i = 0; i< 6; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(4)<<nObservation_RFinger[i][j]<<" \t";
			}
			plik<<"\n";
		}
		plik<<"]";

		plik<<"\n ";
		plik<<"PSamples_RFinger\n ";

		plik<<"P_RFinger = \n ";
		for (int i = 0; i< 5; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(5)<<nStates_RFinger[i][j]<<" \t";
			}
			plik<<";";
		}
		plik<<"]\n\n";

		// dla prof. Kasprzaka
		plik<<"P_RFinger = \n";
		for (int i = 0; i< 5; i++)
		{
			for(int j = 0; j< ileObrazkow; j++)
			{
				plik<<setprecision(4)<<nStates_RFinger[i][j]<<" \t";
			}
			plik<<"\n";
		}
		plik<<"]\n\n";

		for (unsigned int i = 0 ; i < 5; i++)
		{
			for (int j = 0; j < ileObrazkow; j++)
			{
				meanStates_RFinger[i] += nStates_RFinger[i][j];
			}
			meanStates_RFinger[i] /= ileObrazkow;
			plik<<"meanStates_RFinger["<<i<<"] = "<<meanStates_RFinger[i]<<";\n";
		}


	plik.close();
}


//konstruktor
KW_MAP2_Samples::KW_MAP2_Samples(const std::string & name) :
	Base::Component(name) {
	LOG(LTRACE) << "Hello KW_MAP2_Samples\n";


}

}//: namespace KW_MAP2_Samples
}//: namespace Processors
