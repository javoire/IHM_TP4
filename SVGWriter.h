#pragma once
//#include <FStream>
#include <fstream>
#include <utility>

class SVGWriter
{
public:
	typedef std::pair<float,float> Point2D;
	SVGWriter(const std::pair<float,float> & lowerCorner, const std::pair<float,float> & upperCorner, bool inverseY=true, float scale=100.0f);
	~SVGWriter(void);
	void setViewBox(const std::pair<float,float> lowerCorner, const std::pair<float,float> & upperCorner);
	void setInverseY(bool inverse)
	{
		_inverseY=inverse;
	}

	bool openNewSVGDocument(const char * filename);
	void closeSVGDocument();
	void startSegmentStrip(float r, float g, float b, float thickness, bool closed=false, bool relativeThickness=true);
	void stopSegmentStrip();

	void startGroup(int i);
	void startGroup(const std::string &style, const std::string &strliteral="");
	void stopGroup();
	void startStyledPath(const std::string &style, const std::string &strliteral="");
	void stopStyledPath(bool closed);
	void SetFirstElement(bool value = true){_firstElement = value;}
	void closePath();
	void insertLiteral(const std::string &str);

	void startArea(float r, float g, float b, float thickness=0, float rOut=0, float gOut=0, float bOut=0, bool relativeThickness=true);
	//for example holes in the area definition
	void addNewAreaComponent();
	void addNewSegmentComponent();
	void stopArea();

	void addPoint(float x, float y);
	void addBezierPoint(float x, float y, float dx1, float dy1, float x2, float y2, float dx2, float dy2);

	void setBBoxCheck(bool check)
	{
		_checkBBox=check;
	}


protected:
	bool _firstElement;
	bool _isClosed;
	bool _startSegment;
	bool _startArea;
	float _viewBoxStartX;
	float _viewBoxStartY;
	float _viewBoxWidth;
	float _viewBoxHeight;
	bool _inverseY;
	bool _bezier;
	bool _checkBBox;
	float _scale;
	std::ofstream _file;

};
