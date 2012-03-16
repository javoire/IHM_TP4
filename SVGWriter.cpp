#include "SVGWriter.h"
#include <assert.h>
#include <iostream>
using namespace std;

void myAssert(bool a)
{
	if (!a)
	{
		printf("problem");
		assert(a);
	}
}

SVGWriter::SVGWriter(const std::pair<float,float> & lowerCorner, const std::pair<float,float> & upperCorner, bool inverseY, float scale)
	: _startSegment(false)
	, _startArea(false)
	, _isClosed(false)
	, _inverseY(inverseY)
	, _scale(scale)
{
	setViewBox(lowerCorner, upperCorner);
}
SVGWriter::~SVGWriter(void)
{
	if (_file.is_open())
		_file.close();
}

bool SVGWriter::openNewSVGDocument(const char * filename)
{
	myAssert(!_file.is_open());

	_file.open(filename);
	if (_file.is_open())
	{
		_file << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
			"<svg xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
			"  xmlns=\"http://www.w3.org/2000/svg\"\n"
			"  xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n"
			"  version=\"1.0\"\n"
			"  viewBox=\""<<_viewBoxStartX<<" "<<_viewBoxStartY<<" "<<_viewBoxWidth<<" "<<_viewBoxHeight<<"\"\n"
			//"  width=\""<<_viewBoxWidth-0.002/_viewBoxWidth
			//<<"\" height=\""<<_viewBoxHeight-0.002/_viewBoxHeight
			"  width=\""<<_viewBoxWidth <<"\" height=\""<<_viewBoxHeight << "\""
			">\n";
			
			// "version=\"1.2\" baseProfile=\"tiny\">"<<endl;
		_file << "<desc>Clipart software</desc>"<<endl;
		_file << "<rect x=\"";
		_file <<_viewBoxStartX+0.001/_viewBoxWidth
			<<"\" y=\""<<_viewBoxStartY+0.001/_viewBoxHeight
			<<"\" width=\""<<_viewBoxWidth-0.002/_viewBoxWidth
			<<"\" height=\""<<_viewBoxHeight-0.002/_viewBoxHeight
			<<"\" fill=\"none\" stroke-width=\""<<0.001*((_viewBoxHeight<_viewBoxWidth)?_viewBoxWidth:_viewBoxHeight)<<"\" stroke=\"blue\" />"<<endl;
	}
	return _file.is_open();
}

void SVGWriter::closeSVGDocument()
{
	myAssert(!_startSegment);
	myAssert(!_startArea);

	myAssert(_file.is_open());
	_file<<"</svg>"<<endl;
	_file.close();
}

void SVGWriter::setViewBox(const std::pair<float,float> lowerCorner, const std::pair<float,float> & upperCorner)
{
	_viewBoxStartX=_scale*lowerCorner.first;
	_viewBoxStartY=_scale*lowerCorner.second;
	_viewBoxWidth=_scale*(upperCorner.first-lowerCorner.first);
	_viewBoxHeight=_scale*(upperCorner.second-lowerCorner.second);
}

void SVGWriter::startSegmentStrip(float r, float g, float b, float thickness, bool isClosed, bool relativeThickness)
{
	myAssert(thickness>0);
	myAssert(_file.is_open());
	myAssert(!_startSegment);
	myAssert(!_startArea);
	_startSegment=true;
	_isClosed=isClosed;
	_firstElement=true;


	if (r<0)
		r=0;
	if (r>=1.0)
		r=1;

	if (g<0)
		g=0;
	if (g>=1.0)
		g=1;

	if (b<0)
		b=0;
	if (b>=1.0)
		b=1;


	_file << "<g fill-rule=\"evenodd\" fill=\"none\" stroke=\"rgb("
		<<(unsigned int)(r*255.0)<<","<<(unsigned int)(g*255.0)<<","<<(unsigned int)(b*255.0)
		<<")\" stroke-width=\""<<(relativeThickness?thickness*max(_viewBoxWidth, _viewBoxHeight):thickness)<<"\" >"<<endl;
	_file<< "<path d=\"";
}
void SVGWriter::stopSegmentStrip()
{
	myAssert(_file.is_open());
	myAssert(_startSegment);
	myAssert(!_startArea);
	_startSegment=false;
	if (_isClosed)
		_file<<" z"<<endl;
	_file<<"\"/>"<<endl;
	_file<<"</g>"<<endl;
}

void SVGWriter::startArea(float r, float g, float b, float thicknessOut, float rOut, float gOut, float bOut, bool relativeThickness)
{
	myAssert(_file.is_open());
	myAssert(!_startSegment);
	myAssert(!_startArea);
	_startArea=true;
	_firstElement=true;

	//clamp input
	if (r<0)
		r=0;
	if (r>=1.0)
		r=1;
	if (g<0)
		g=0;
	if (g>=1.0)
		g=1;
	if (b<0)
		b=0;
	if (b>=1.0)
		b=1;
	if (rOut<0)
		rOut=0;
	if (rOut>=1.0)
		rOut=1;
	if (gOut<0)
		gOut=0;
	if (gOut>=1.0)
		gOut=1;
	if (bOut<0)
		bOut=0;
	if (bOut>=1.0)
		bOut=1;

	_file << "<g fill-rule=\"evenodd\" fill=\"rgb("
		<<(unsigned int)(r*255.0)<<","<<(unsigned int)(g*255.0)<<","<<(unsigned int)(b*255.0)
		<<")\" stroke=\"rgb("<< (unsigned int)(rOut*255.0)<<","<<(unsigned int)(gOut*255.0)<<","<<(unsigned int)(bOut*255.0)
		<<")\" stroke-width=\""<<(relativeThickness?thicknessOut*max(_viewBoxWidth, _viewBoxHeight):thicknessOut)<<"\" >"<<endl;
	_file<< "<path d=\"";
}

void SVGWriter::insertLiteral(const std::string &str)
{
	myAssert(_file.is_open());
//	myAssert(!_startSegment);
//	myAssert(!_startArea);
	_file << str.c_str();
}

void SVGWriter::startGroup(const std::string &style, const std::string &strliteral)
{
	myAssert(_file.is_open());
	// we can nest groups, but we cannot start a group within a path
	myAssert(!_startSegment);
	myAssert(!_startArea);
	_file << "<g ";
	if(style.size()>0)
		_file << "style=\"" << style.c_str() << "\"";
	if(strliteral.size()>0)
		_file << strliteral.c_str() ;
	_file  << ">\n";
}
void SVGWriter::startGroup(int i)
{
	myAssert(_file.is_open());
	// we can nest groups, but we cannot start a group within a path
	myAssert(!_startSegment);
	myAssert(!_startArea);

	_file << "<g ";
	_file << "Layer=\"" << i<< "\"";
	_file  << ">\n";
}

void SVGWriter::stopGroup()
{
	myAssert(_file.is_open());
	_file << "</g>\n";
}

void SVGWriter::startStyledPath(const std::string &style, const std::string &strliteral)
{
	myAssert(_file.is_open());
	myAssert(!_startSegment);
	_startSegment=true;
	_firstElement=true;
	_file << "<path ";
	if(style.size()>0)
		_file << "style=\"" << style.c_str() << "\"";
	if(strliteral.size()>0)
		_file << strliteral.c_str() ;
	_file  << " d=\"";
}

void SVGWriter::stopStyledPath(bool closed)
{
	myAssert(_file.is_open());
	myAssert(_startSegment);
	_startSegment=false;

	//close curve
	if(closed)
		_file << " z";
	_file << "\"/>\n";
}

void SVGWriter::closePath()
{
	_file<<" z"<<endl;
}

void SVGWriter::addNewSegmentComponent()
{
	myAssert(_file.is_open());
	myAssert(_startSegment);
	myAssert(!_startArea);
	//close curve
	if (!_firstElement)
		closePath();
	_firstElement=true;
}

void SVGWriter::addNewAreaComponent()
{
	myAssert(_file.is_open());
	myAssert(!_startSegment);
	myAssert(_startArea);
	//close curve
	if (!_firstElement)
		closePath();
	_firstElement=true;
}



void SVGWriter::stopArea()
{
	myAssert(_file.is_open());
	myAssert(!_startSegment);
	myAssert(_startArea);
	_startArea=false;

	//close curve
	_file<<" z\"/>"<<endl;
	_file<<"</g>"<<endl;
}

void SVGWriter::addPoint(float x, float y)
{
	myAssert(!_bezier||_firstElement);
	myAssert(_file.is_open());
	x *= _scale;
	y *= _scale;
	myAssert((!_checkBBox)||(_viewBoxStartX<=x&&x<_viewBoxStartX+_viewBoxWidth));
	
	if (_inverseY)
	{
		y-=_viewBoxStartY;
		y*=-1;
		y+=_viewBoxHeight+_viewBoxStartY;
	}
	
	myAssert ( (!_checkBBox)||(_viewBoxStartY<=y&&y<_viewBoxStartY+_viewBoxHeight));

	if (_firstElement)
	{
		_file<<"M "<<x<<","<<y<<" ";
		_firstElement=false;
		_bezier=false;
	}
	else
	{
		_file<<"L "<<x<<","<<y<<" ";
	}
}

void SVGWriter::addBezierPoint(float x, float y, float controlx1, float controly1, 
							   float controlx2, float controly2, float x2, float y2)
{
	myAssert(_bezier||_firstElement);
	myAssert(_file.is_open());
	myAssert((!_checkBBox)||(_viewBoxStartX<=x&&x<_viewBoxStartX+_viewBoxWidth));
	x *= _scale;
	y *= _scale;
	x2 *= _scale;
	y2 *= _scale;
	controlx1 *= _scale;
	controly1 *= _scale;
	controlx2 *= _scale;
	controly2 *= _scale;
	

	//we do not inverse for bezier curves, because then we would also have to inverse the derivatives... let us do this later!!!
	if (_inverseY)
	{
		y-=_viewBoxStartY;
		y*=-1;
		y+=_viewBoxHeight+_viewBoxStartY;
	
		y2-=_viewBoxStartY;
		y2*=-1;
		y2+=_viewBoxHeight+_viewBoxStartY;


		controly1-=_viewBoxStartY;
		controly1*=-1;
		controly1+=_viewBoxHeight+_viewBoxStartY;

		controly2-=_viewBoxStartY;
		controly2*=-1;
		controly2+=_viewBoxHeight+_viewBoxStartY;

		//dy1*=-1;
		//dy2*=-1;
	}
	
	myAssert((!_checkBBox)||(_viewBoxStartY<=y&&y<_viewBoxStartY+_viewBoxHeight));

	if (_firstElement)
	{
		_firstElement=false;
		_bezier=true;
		_file<<"M "<<x<<","<<y<<" ";
		// std::cout<<"line FROM:"<<x<<","<<y<<std::endl;
	}

	//svg stores control points for a bezier curve.
	//the nice thing is that the difference between the control point and the start is the derivative.
	//_file<<"C "<<x+dx1<<","<<y+dy1<<" "<<x2-dx2<<","<<y2-dy2<<" "<<x2<<","<<y2<<" ";
	_file<<"C "<<controlx1<<","<<controly1<<" "<<controlx2<<","<<controly2<<" "<<x2<<","<<y2<<" ";
	//_file<<"C "<<0<<","<<0<<" "<<0<<","<<0<<" "<<x2<<","<<y2<<" ";
	// std::cout<<"line To:"<<x2<<","<<y2<<std::endl;

}