#pragma once
#ifndef IMAGE_H_jdflsjfdkjdslfds
#define IMAGE_H_jdflsjfdkjdslfds

#include <fstream>
#include <iostream>
#include <vector>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>

using namespace std;

struct PixelPos: public pair<int, int>
{
	PixelPos(int i=0, int j=0)
		:pair<int,int>(i,j)
	{
		
	}
	int x()
	{
		return first;
	}
	int y()
	{
		return second;
	}

	PixelPos & operator-(const PixelPos& p)
	{
		first-=p.first;
		second-=p.second;
	}

	float norm2()
	{
		return (first*first+second*second);
	}
	float norm()
	{
		return sqrt(norm2());
	}


};

struct Pixel
{
	Pixel(float rI=0,float gI=0,float bI=0)
		:r(rI)
		,g(gI)
		,b(bI)
	{
	
	}

	float operator[](int i) const
	{
		if ((i>2)||(i<0))
		{
			printf("Access beyond color!\n");
			i=0;
		}
		return rgb[i];
	}

	float & operator[](int i)
	{
		if ((i>2)||(i<0))
		{
			printf("Access beyond color!\n");
			i=0;
		}
		return rgb[i];
	}
	
	Pixel operator+(const Pixel & p)
	{
		return Pixel(p.r+r,p.g+g,p.b+b);
	}

	Pixel operator-(const Pixel & p)
	{
		return Pixel(r-p.r,g-p.g,b-p.b);
	}

	Pixel & operator=(const Pixel & p)
	{
		r=p.r;
		g=p.g;
		b=p.b;
		return (*this);
	}

	float norm2()
	{
		return (r*r+g*g+b*b);
	}

	float norm()
	{
		return sqrt(norm2());
	}


	bool operator<(const Pixel & p)
	{
		if (r<p.r)
			return true;
		if ((r==p.r) && (g<p.g))
			return true;
		if ((r==p.r) && (g=p.g) &&(b<p.b))
			return true;
		return false;
	}


	union
	{
		struct
		{
			float r;
			float g;
			float b;
		};
		float rgb[3];
	};	
	//Deux possibilités pour acceder la meme valeur: .r ou bien .rgb[0]
};

struct Image
{
	Image(int wI=1, int hI=1)
		:w(wI)
		,h(hI)
	{
		data.resize(w*h);
	}

	void resize(int wI=1, int hI=1)
	{
		w=wI;
		h=hI;
		data.resize(w*h);
	}

	bool load(const char *filename) {
        ifstream in(filename);
        char temp[256];

        if (!in.is_open()) printf("impossible d'ouvrir le fichier\n");
        
        //find P start
        while (!in.eof() && in.peek()!='P')
            in.getline(temp, 256);
    
        if (in.eof()) printf("fin du fichier inattendue\n");
    
        //delete line with p
        in.getline(temp, 256);
    
        //p3 format
        if (temp[1] != '3') printf("seul le format RGB (3 canaux) est supporté\n");
    
        //jump over comments
        while (!in.eof() && in.peek()=='#')
            in.getline(temp, 256);
    
        in.getline(temp, 256);
    
    
        int nb=sscanf(temp, "%i %i", &w, &h);
        if (nb != 2) printf("problème de format de fichier\n");
    
        while (!in.eof() && in.peek()=='#')
            in.getline(temp, 256);
    
        int value;
        in.getline(temp, 256);
        nb=sscanf(temp, "%i", &value);
    
        if (nb != 1) printf("problème de format de fichier\n");
        if (value != 255) printf("problème de format de fichier\n");
    
        vector<unsigned char> rawData(w*h*3);
    
        nb = 0;
        int res;
        while(!in.eof()) {
            in >> res;
            rawData[nb] = static_cast<unsigned char>(res);
            ++nb;
            if (nb == 3*w*h) break;
    
        }
    
        if (nb != 3*w*h) printf("problème de format de fichier\n");
    
        in.close();
		
		data.resize(w * h);
		for (int i=0; i<w; i++)
			for (int j=0; j<h; j++)
			{
					data[(i + w * (h-1-j))] = Pixel(static_cast<float>(rawData[3 * (i+w*j)]) / 255.0f, static_cast<float>(rawData[3 * (i+w*j)+1]) / 255.0f, static_cast<float>(rawData[3 * (i+w*j)+2]) / 255.0f);
			}
		return true;
    };

	
	Image & operator= (const Image & image)
	{
		w=image.w;
		h=image.h;
		data.resize(image.nbPixels());
		for (int i=0; i<image.nbPixels();++i)
		{
			data[i]=image.data[i];
		}
		return (*this);
	}

	float & getPixel(int i, int j, int col)
	{
		if (i<0)
			i=0;
		if (i>w-1)
			i=w-1;

		if (j<0)
			j=0;
		if (j>h-1)
			j=h-1;

		return data[i+w*j][col];
	}

	float getPixel(int i, int j, int col) const 
	{
		if (i<0)
			i=0;
		if (i>w-1)
			i=w-1;

		if (j<0)
			j=0;
		if (j>h-1)
			j=h-1;

		return data[i+w*j][col];
	}

	Pixel & getPixel(int i, int j)
	{
		if (i<0)
			i=0;
		if (i>w-1)
			i=w-1;

		if (j<0)
			j=0;
		if (j>h-1)
			j=h-1;

		return data[i+w*j];
	}
	const Pixel & getPixel(int i, int j) const
	{
		if (i<0)
			i=0;
		if (i>w-1)
			i=w-1;

		if (j<0)
			j=0;
		if (j>h-1)
			j=h-1;

		return data[i+w*j];
	}

	
	Pixel & operator[](int i)
	{
		return data[i];
	}

	const Pixel & operator[](int i) const
	{
		return data[i];
	}

	int nbPixels() const
	{
		return w*h;
	}

	void draw() const
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glBegin(GL_POINTS);
		for (int y=0; y<h;++y)
			for (int x=0; x<w;++x)
			{
				glColor3f(getPixel(x,y,0), getPixel(x,y,1), getPixel(x,y,2));
				glVertex2f((2*float(x)+1)/float(w) -1, (2*float(y)+1)/float(h) -1);
			}
		glEnd();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	
	int w;
	int h;
	std::vector<Pixel> data;
};
#endif