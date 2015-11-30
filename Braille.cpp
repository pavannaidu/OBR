#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<fstream>

using namespace std;
using namespace cv;
void BrailleToText(int);


int main()
{
	
	string	filename = "D:\\Pavan\\DIP\\Originals\\Braille_1.jpg";
	Mat image, BinaryImage, LabelledImage, stats, Centroids, FilteredImage;
	int label;
	CvScalar Color = 255;
	Point2f tempPoint, CarrierPoint;
	image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

	if (!image.data)
	{
		cout << "Image not found." << std::endl;
		return -1;
	}
	else
	{
		cout << "Image found." << std::endl;
	}

	threshold(image, BinaryImage, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	medianBlur(BinaryImage, FilteredImage, 5);
	medianBlur(FilteredImage, FilteredImage, 3);
	
	
	label = connectedComponentsWithStats(FilteredImage, LabelledImage, stats, Centroids, 8, CV_32S);
	//////////////////////////////////////////////////////////////////////////////////////////
	cout << "Label:" << label;
	namedWindow("Temp", CV_WINDOW_FREERATIO);
	imshow("Temp", FilteredImage);
	double MinimumDistance = FilteredImage.rows, Row1 =0, Row2=0;
	
	//Extracting the Minimum Distance.
	for (int r = 1; r < Centroids.rows; r++)
	{

			Scalar Intensity = Centroids.at<double>(Point(1, r));
			double t = Intensity.val[0];
			if (Row1 == 0 && Row2 == 0)
			{
				Row1 = t;
			}
			else if (Row1 != 0 && Row2 == 0)
			{
				Row2 = t;
				if (abs(Row1 - Row2) > 5)
				{
					MinimumDistance = abs(Row1 - Row2);
				}
			}
			else if (Row1 != 0 && Row2 != 0)
			{
				Row1 = Row2;
				Row2 = t;
				if (MinimumDistance > abs(Row1 - Row2) && abs(Row1 - Row2) > 5)
				{
					MinimumDistance = abs(Row1 - Row2);
				}
			}
	}
	cout << endl << "MinimumDistance :"<< floor(MinimumDistance/2) << endl;
	
	//////////////////////////////////////////////////////////////////////////////////////////
	/*Circle the KeyPoints and Fill them*/
	int Radius = floor(MinimumDistance / 2);
	for (int r = 1; r < Centroids.rows; r++)
	{
		double row, col;
		for (int c = 0; c <= 1; c++)
		{
			Scalar Intensity = Centroids.at<double>(Point(c, r));
			double t = Intensity.val[0];
			if (c == 0)
			{
				col = t;
			}
			else
			{
				row = t;
			}
		}
		circle(FilteredImage, Point2d(col, row), Radius, Color, -(Radius/2), LINE_8, 0);
	}

	namedWindow("BrailleProcessed", CV_WINDOW_FREERATIO);
	imshow("BrailleProcessed", FilteredImage);
	imwrite("D:\\Pavan\\DIP\\FilteredImages\\FilteredImage_2.jpg", FilteredImage);
	label = connectedComponentsWithStats(FilteredImage, LabelledImage, stats, Centroids, 8, CV_32S);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Convert Centroids into a Points matrix and also, finds:
	//Minimum and Maximum values of the row and column elements.
	vector<Point2d> CO(Centroids.rows - 1);
	Scalar Mintemp = Centroids.at<double>(Point(0, 1));
	double Mintemp1 = Mintemp.val[0];
	Mintemp = Centroids.at<double>(Point(1, 1));
	double Mintemp2 = Mintemp.val[0];
	double RowMin = Mintemp2, RowMax = 0, ColMin = Mintemp1, ColMax = 0;
	
	for (int i = 1; i < Centroids.rows; i++)
	{
		Scalar temp = Centroids.at<double>(Point(0, i));
		double Col0 = temp.val[0];
		if (Col0 >= ColMax)
			ColMax = Col0;

		if (Col0 <= ColMin)
			ColMin = Col0;
	

		temp = Centroids.at<double>(Point(1, i));
		double Col1 = temp.val[0];
		if (Col1 >= RowMax)
			RowMax = Col1;
		if (Col1 <= RowMin)
			RowMin = Col1;

		CO[i - 1] = Point2d(Col0, Col1);
	}

	/*Extracting Parameters*/
	/////////////////////////////////////////////////////////////////////////////////////////
	double HorizontalIntraCharacter;
	double HorizontalInterCharacter;
	double VerticalIntraCharacter;
	double VerticalInterCharacter;
	double HorizontalMinDistance;
	double VerticalMinDistance;
	int MinDistance;
	int SortArrayLength;
	/////////////////////////////////////////////////////////////////////////////////////////
	/*Vertical Parameters Extraction*/
	vector<double> tempEX((CO.size() - 1)/2);
	double temp = 0;
	int AverageCount = 0, Count = 0;
	double VerticalParameters[3] = {0,0,0};
	double VerticalParametersCount[3] = { 0.0, 0.0, 0.0 };
	for (int i = 0; i < CO.size() - 1; i++)
	{
		double temp1, temp2;
		temp1 = CO[i].y;
		temp2 = CO[i + 1].y;
		if (temp2 - temp1 <= 5)
		{
			temp += temp1;
			AverageCount++;
		}
		else
		{
			temp += temp1;
			temp = temp / (AverageCount + 1);
			tempEX[Count] = temp;
			temp = 0;
			AverageCount = 0;
			//cout << tempEX[Count] << endl;
			Count++;
		}
	}
	
	for (int i = 0; i < tempEX.size() - 1; i++)
	{
		double temp1, temp2;
		temp1 = tempEX[i];
		temp2 = tempEX[i + 1];
		tempEX[i] = temp2 - temp1;
		//cout << tempEX[i] << endl;
	}
	cout << "//////////////////////////////" << endl;
	for (int i = 0; i < tempEX.size() - 1; i++)
	{
		double temp, temp1, temp2, temp3;
		if (tempEX[i] > 0)
		{
			temp = tempEX[i];
			if ((VerticalParameters[0] == 0 || abs(temp1 - temp) <= 5))
			{ 
				if (VerticalParameters[0] == 0)
				{
					temp1 = temp;
					VerticalParameters[0] += temp;
					VerticalParametersCount[0] = 1;
					VerticalMinDistance = temp;
				}
				else
				{
					if (VerticalMinDistance > temp)
					{
						VerticalMinDistance = temp;
					}
					temp1 = temp;
					VerticalParameters[0] += temp;
					VerticalParametersCount[0]++;
				}
			}
			else if ((VerticalParameters[1] == 0 || abs(temp2 - temp) <= 5))
			{
				if (VerticalParameters[1] == 0)
				{
					temp2 = temp;
					VerticalParameters[1] += temp;
					VerticalParametersCount[1] = 1;
				}
				else
				{
					temp2 = temp;
					VerticalParameters[1] += temp;
					VerticalParametersCount[1]++;
				}
			}
			else if ((VerticalParameters[2] == 0 || abs(temp3 - temp) <= 5))
			{
				if (VerticalParameters[2] == 0)
				{
					temp3 = temp;
					VerticalParameters[2] += temp;
					VerticalParametersCount[2] = 1;
				}
				else
				{
					temp3 = temp;
					VerticalParameters[2] += temp;
					VerticalParametersCount[2]++;
				}
			}
			
		}	
	}
	
	VerticalParameters[0] = VerticalParameters[0] / VerticalParametersCount[0];
	VerticalParameters[1] = VerticalParameters[1] / VerticalParametersCount[1];
	VerticalParameters[2] = VerticalParameters[2] / VerticalParametersCount[2];
	
	//sort(begin(VerticalParameters), end(VerticalParameters));
	SortArrayLength = sizeof(VerticalParameters) / sizeof(VerticalParameters[0]);
	cout << "**" << SortArrayLength << endl;
	for (int i = 0; i < SortArrayLength - 1;i++)
	{
		int min = i;
		for (int j = i + 1; j < SortArrayLength; j++)
		{
			if (VerticalParameters[j]<VerticalParameters[min])
			{
				min = j;
			}
		}
		if (min != i)
		{
			double temp;
			temp = VerticalParameters[min];
			VerticalParameters[min] = VerticalParameters[i];
			VerticalParameters[i] = temp;		
		}
		
	}
	


	VerticalIntraCharacter = VerticalParameters[0];
	VerticalInterCharacter = VerticalParameters[1];
	
	cout << "VerticalInterCharacter: " << VerticalInterCharacter << endl;
	cout << "VerticalIntraCharacter: " << VerticalIntraCharacter << endl;
	cout << "VerticalMinDistance: " << VerticalMinDistance << endl;
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/*End of Vertical Parameters Extraction.*/

	////////////////////////////////////////////////////////////////////////////////////////////////
	/*Horizontal Parameters Extraction*/
	vector<double> tempEX1(CO.size()); 
	for (int i = 0; i < CO.size(); i++)
	{
		tempEX1[i] = CO[i].x;
	}
	
	SortArrayLength = tempEX1.size();
	for (int i = 0; i < SortArrayLength - 1; i++)
	{
		int min = i;
		for (int j = i + 1; j < SortArrayLength; j++)
		{
			if (tempEX1[j]<tempEX1[min])
			{
				min = j;
			}
		}
		if (min != i)
		{
			double temp;
			temp = tempEX1[min];
			tempEX1[min] = tempEX1[i];
			tempEX1[i] = temp;
		}

	}
	
	//sort(begin(tempEX1), end(tempEX1));
	vector<double> tempEX2((CO.size() - 1) / 2);
	temp = 0;
	AverageCount = 0;
	Count = 0;
	double HorizontalParameters[3] = { 0, 0, 0 };
	double HorizontalParametersCount[3] = { 0.0, 0.0, 0.0 };
	for (int i = 0; i < CO.size() - 1; i++)
	{
		double temp1, temp2;
		temp1 = tempEX1[i];
		temp2 = tempEX1[i+1];
		if (temp2 - temp1 <= 5)
		{
			temp += temp1;
			AverageCount++;
		}
		else
		{
			temp += temp1;
			temp = temp / (AverageCount + 1);
			tempEX2[Count] = temp;
			temp = 0;
			AverageCount = 0;
			//cout << tempEX[Count] << endl;
			Count++;
		}
	}
	for (int i = 0; i < tempEX2.size() - 1; i++)
	{
		double temp1, temp2;
		temp1 = tempEX2[i];
		temp2 = tempEX2[i + 1];
		tempEX2[i] = temp2 - temp1;
		//cout << tempEX[i] << endl;
	}
	for (int i = 0; i < tempEX2.size() - 1; i++)
	{
		double temp, temp1, temp2, temp3;
		if (tempEX2[i] > 0)
		{
			temp = tempEX2[i];
			if ((HorizontalParameters[0] == 0 || abs(temp1 - temp) <= 5))
			{
				if (HorizontalParameters[0] == 0)
				{
					temp1 = temp;
					HorizontalParameters[0] += temp;
					HorizontalParametersCount[0] = 1;
					HorizontalMinDistance = temp;
				}
				else
				{
					if (HorizontalMinDistance > temp)
					{
						HorizontalMinDistance = temp;
					}
					temp1 = temp;
					HorizontalParameters[0] += temp;
					HorizontalParametersCount[0]++;
				}
			}
			else if ((HorizontalParameters[1] == 0 || abs(temp2 - temp) <= 5))
			{
				if (HorizontalParameters[1] == 0)
				{
					temp2 = temp;
					HorizontalParameters[1] += temp;
					HorizontalParametersCount[1] = 1;
				}
				else
				{
					temp2 = temp;
					HorizontalParameters[1] += temp;
					HorizontalParametersCount[1]++;
				}
			}
			else if ((HorizontalParameters[2] == 0 || abs(temp3 - temp) <= 5))
			{
				if (HorizontalParameters[2] == 0)
				{
					temp3 = temp;
					HorizontalParameters[2] += temp;
					HorizontalParametersCount[2] = 1;
				}
				else
				{
					temp3 = temp;
					HorizontalParameters[2] += temp;
					HorizontalParametersCount[2]++;
				}
			}

		}
	}

	HorizontalParameters[0] = HorizontalParameters[0] / HorizontalParametersCount[0];
	HorizontalParameters[1] = HorizontalParameters[1] / HorizontalParametersCount[1];
	HorizontalParameters[2] = HorizontalParameters[2] / HorizontalParametersCount[2];

	//sort(begin(HorizontalParameters), end(HorizontalParameters));
	HorizontalIntraCharacter = HorizontalParameters[0];
	HorizontalInterCharacter = HorizontalParameters[1];
	cout << "HorizontalInterCharacter: " << HorizontalInterCharacter << endl;
	cout << "HorizontalIntraCharacter: " << HorizontalIntraCharacter << endl;
	cout << "HorizontalMinDistance: " << HorizontalMinDistance << endl;

	//Finding the Minimum distance between dots.
	//(least of horizonatal spacing and vertical spaceing)
	if (VerticalMinDistance < HorizontalMinDistance)
	{
		MinDistance = floor(VerticalMinDistance / 2 - 1);
	}
	else
	{
		MinDistance = floor(HorizontalMinDistance / 2 - 1);
	}
	cout << "MinDistance: " << MinDistance << endl;
	///////////////////////////////////////////////////////////////////
	
	//Linear Procedure 
	ofstream file;
	int tempRow;
	int tempCol;
	int tempWidth;
	int tempHeight;
	int tempCount = 0;
	Mat SubImage, SubLabelledImage, SubCentroids,Substats;
	int SubImageLabel;
	int NumberOfCharacters = 0;
	int CharacterCode = 0;
	int FlagCol1 = 0,FlagCol2 = 0,FlagCol3 = 0;
	int PrevRowCount = 0, CurRowCount = 0, RowMinSub;
	int PrevColCount = 0, CurColCount = 0, ColMinSub;
	tempPoint = Point(ColMin, RowMin); //168, SecondLine
	CarrierPoint = tempPoint;
	
NextLine:
	tempRow = tempPoint.y - VerticalInterCharacter/2;
	tempCol = 0;
	tempWidth = FilteredImage.cols;
	tempHeight = VerticalIntraCharacter*2 + VerticalInterCharacter;
	
	SubImage = FilteredImage(Rect(tempCol, tempRow, tempWidth, tempHeight));
	
	SubImageLabel = connectedComponentsWithStats(SubImage, SubLabelledImage, Substats, SubCentroids, 8, CV_32S);
	
	if (SubImageLabel != 1)
	{
		PrevRowCount = 0; CurRowCount = 0; RowMinSub = 0;
		Count = 0;
		for (int i = 0; i < SubImage.rows; i++)
		{
			for (int j = 0; j < SubImage.cols; j++)
			{
				Scalar temp = SubImage.at<uchar>(Point(j, i));
				double temp1 = temp.val[0];
				if (temp1 == 255)
					Count++;
			}

			if (PrevRowCount == 0 && CurRowCount == 0)
			{
				PrevRowCount = Count;
				CurRowCount = Count;
			}

			if (PrevRowCount > CurRowCount)
			{
				RowMinSub = i;
				goto DoneScanningRow;
			}
			else
			{
				PrevRowCount = CurRowCount;
				CurRowCount = Count;
			}
			Count = 0;
		}

	DoneScanningRow:
		//Scan the Columns.
		PrevColCount = 0; CurColCount = 0; ColMinSub = 0;
		Count = 0;
		for (int i = 0; i < SubImage.cols; i++)
		{
			for (int j = 0; j < SubImage.rows; j++)
			{
				Scalar temp = SubImage.at<uchar>(Point(i, j));
				double temp1 = temp.val[0];
				if (temp1 == 255)
					Count++;
			}

			if (PrevColCount == 0 && CurColCount == 0)
			{
				PrevColCount = Count;
				CurColCount = Count;
			}

			if (PrevColCount > CurColCount)
			{
				ColMinSub = i;
				goto DoneScanningCol;
			}
			else
			{
				PrevColCount = CurColCount;
				CurColCount = Count;
			}
			Count = 0;
		}

	DoneScanningCol:
		
		RowMinSub += tempRow;

		if (abs(tempPoint.y - RowMinSub) > 3)
		{
			cout << "Check1" << endl;
			tempPoint.y = RowMinSub;
		}
	}


	while (1)
	{
		//Start
		Scalar temp = LabelledImage.at<int>(tempPoint);
		int tempVal = temp.val[0];

		if (tempVal != 0)
		{
			//cout << "1,";
			tempPoint = CO[tempVal - 1]; //If there is a Point, assign the tempPoint 
			// to the centroid of the label.
			CharacterCode += 32;
		}
		else
		{
			//cout << "0,";
		}	
		
		//Move Down1.
		tempPoint.y = tempPoint.y + VerticalIntraCharacter;
		temp = LabelledImage.at<int>(tempPoint);
		tempVal = temp.val[0];
		if (tempVal != 0)
		{
			//cout << "1,";
			tempPoint = CO[tempVal - 1]; //If there is a Point, assign the tempPoint 
			// to the centroid of the label.
			CharacterCode += 16;
		}
		else
		{
			//cout << "0,";
		}
	
		//Move Down2.
		tempPoint.y = tempPoint.y + VerticalIntraCharacter;
		temp = LabelledImage.at<int>(tempPoint);
		tempVal = temp.val[0];
		if (tempVal != 0)
		{
			//cout << "1,";
			tempPoint = CO[tempVal - 1]; //If there is a Point, assign the tempPoint 
			// to the centroid of the label.
			CharacterCode += 8;
		}
		else
		{
			//cout << "0,";
		}
		//Move Right.
		tempPoint.x = tempPoint.x + HorizontalIntraCharacter;
		temp = LabelledImage.at<int>(tempPoint);
		tempVal = temp.val[0];
		if (tempVal != 0)
		{
			//cout << "1,";
			tempPoint = CO[tempVal - 1]; //If there is a Point, assign the tempPoint 
			// to the centroid of the label.
			CharacterCode += 1;
		}
		else
		{
			//cout << "0,";
		}

		//Move Up1.
		tempPoint.y = tempPoint.y - VerticalIntraCharacter;
		temp = LabelledImage.at<int>(tempPoint);
		tempVal = temp.val[0];
		if (tempVal != 0)
		{
			//cout << "1,";
			tempPoint = CO[tempVal - 1]; //If there is a Point, assign the tempPoint 
			// to the centroid of the label.
			CharacterCode += 2;
		}
		else
		{
			//cout << "0,";
		}

		//Move Up2.
		tempPoint.y = tempPoint.y - VerticalIntraCharacter;
		temp = LabelledImage.at<int>(tempPoint);
		tempVal = temp.val[0];
		if (tempVal != 0)
		{
			//cout << "1" << endl;
			tempPoint = CO[tempVal - 1]; //If there is a Point, assign the tempPoint 
			// to the centroid of the label.
			CharacterCode += 4;
		}
		else
		{	
			//cout << "0" << endl;
		}	
	    //BrailleToText(CharacterCode);
		cout << CharacterCode;
		CharacterCode = 0;
		
		tempPoint.x = tempPoint.x + HorizontalInterCharacter;
		NumberOfCharacters++;
		if (tempPoint.x > ColMax)
		{
			cout << endl << "Number of Character: " << NumberOfCharacters << endl;
			NumberOfCharacters = 0;
			break;
		}

	}

		cout << endl << "/////////////////Next Line//////////////////////" << endl;
		CarrierPoint.y = CarrierPoint.y + (VerticalIntraCharacter * 2) + VerticalInterCharacter;
		tempPoint = CarrierPoint;
		if (CarrierPoint.y < RowMax)
		{
			goto NextLine;
		}

		cout << "Done Scanning" << endl;
		cout << "//////////////////////////////////////////////////////////";
	

	waitKey(0);
	return 0;
}

/*Text Database*/

void BrailleToText(int x)
{
	switch (x)
	{
	case 0:
		cout << " ";
		break;
	case 32:
		cout << "a";
		break;
	case 14:
		cout << "A";
		break;
	case 20:
		cout << "i";
		break;
	case 10:
		cout << "I";
		break;
	case 41:
		cout << "u";
		break;
	case 51:
		cout << "U";
		break;
	case 34:
		cout << "e";
		break;
	case 12:
		cout << "ai";
		break;
	case 42:
		cout << "o";
		break;
	case 21:
		cout << "au";
		break;
	case 3:
		cout << "aM";
		break;
	case 1:
		cout << "aH";
		break;
	case 40:
		cout << "k";
		break;
	case 5:
		cout << "kH";
		break;
	case 54:
		cout << "g";
		break;
	case 49:
		cout << "gh";
		break;
	case 13:
		cout << "G";
		break;
	case 36:
		cout << "ch";
		break;
	case 33:
		cout << "Ch";
		break;
	case 22:
		cout << "j";
		break;
	case 11:
		cout << "jh";
		break;
	case 18:
		cout << "nY";
		break;
	case 31:
		cout << "t";
		break;
	case 23:
		cout << "T";
		break;
	case 53:
		cout << "d";
		break;
	case 63:
		cout << "D";
		break;
	case 15:
		cout << "N";
		break;
	case 30:
		cout << "th";
		break;
	case 39:
		cout << "Th";
		break;
	case 38:
		cout << "d";
		break;
	case 29:
		cout << "Dh";
		break;
	case 46:
		cout << "n";
		break;
	case 60:
		cout << "p";
		break;
	case 26:
		cout << "ph";
		break;
	case 48:
		cout << "b";
		break;
	case 6:
		cout << "B";
		break;
	case 44:
		cout << "m";
		break;
	case 47:
		cout << "y";
		break;
	case 58:
		cout << "r";
		break;
	case 56:
		cout << "l";
		break;
	case 57:
		cout << "v";
		break;
	case 37:
		cout << "sh";
		break;
	case 61:
		cout << "Sh";
		break;
	case 28:
		cout << "s";
		break;
	case 50:
		cout << "h";
		break;
	default:
		cout << "x";
	}
}
