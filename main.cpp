#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <vector>
#include "image.h"
#include "SVGWriter.h"
using namespace std;


//______________________________________________________________________________
//______________________________________________________________________________
//Commencez ci-dessous !
//______________________________________________________________________________
//______________________________________________________________________________

//Variables
Image image;
Image imageResult;

vector<PixelPos> selectedPixels; 
//const char * filename="data/TelecomLogo.ppm";
//const char * filename="data/TelecomLogo.ppm";
const char * filename="data/MJBlackAndWhite.ppm";

enum Mode { INVERT=0, FILTERING=1, SEGMENTATION=2, YOUR_OWN=3};
const char * modeToString(Mode d)
{

	if (d==SEGMENTATION)
		return "Segmentation";
	else if (d==FILTERING)
		return "Filtering";
	else
		return "YOUR_OWN";
}

enum DisplayMode { ORIGINAL=0, RESULT=1, YOUR_OWN_2=3};
DisplayMode displayMode = ORIGINAL;

const char * displayModeToString(DisplayMode d)
{
	if (d==ORIGINAL)
		return "Display Original Image";
	else if (d==RESULT)
		return "Display Result";
	else
		return "Display YOUR_OWN";
}

void computeResult(Mode mode)
{
	if (mode==INVERT)
	{
		//a corriger!
		cout<<"Inverting..."<<endl;
		Pixel pixelWhite = Pixel(1,1,1);
		Pixel pixelBlack = Pixel(0,0,0);
	
		for (int i=0;i<image.nbPixels();++i) {
			
			Pixel pixel = imageResult[i];
	
			if (pixel.r == pixelBlack.r) {
				imageResult[i]=Pixel(1,1,1);
			} else if (pixel.r == pixelWhite.r) {
				imageResult[i]=Pixel(0,0,0);
			}
		}
	}
	else if (mode==FILTERING)
	{
		//filtrage...	
		cout<<"Filtering..."<<endl;
	}
	else if (mode==SEGMENTATION)
	{
		//segmentation...
		cout<<"Segmentation..."<<endl;
	}
}

// prise en compte du clavier
void keyboard(unsigned char key, int x, int y)
{
	cout<<"key"<<key<<"pressed at"<<x<<","<<y<<endl;
    fflush(stdout);
    if (key>'0'&&key<='9')
    {
		computeResult(Mode(key-'1'));
	}
    if (key=='o')
    {
		displayMode=ORIGINAL;
	}
	else if (key=='r')
	{
		displayMode=RESULT;
	}
	else if (key==27)     // touche ESC
        exit(0);

	cout<<"-----"<<endl;
	cout<<"DISPLAY_MODE:"<< displayModeToString(displayMode) << endl;

	glutPostRedisplay();
}


void drawPoints(vector<PixelPos> & pixelPos)
{
	//pas besoin de comprendre cette fonction!
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPointSize(5);
		glBegin(GL_POINTS);
		for (unsigned int i=0; i<pixelPos.size();++i)
		{
			int x=pixelPos[i].first;
			int y=pixelPos[i].second;
			glColor3f(1.0-image.getPixel(x,y,0), 1.0-image.getPixel(x,y,1), 1.0-image.getPixel(x,y,2));
			glVertex2f((2*float(x)+1)/float(image.w) -1, (2*float(y)+1)/float(image.h) -1);
		}
		glEnd();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
}


void dessiner( )
{
	if (displayMode==ORIGINAL)
	{
		image.draw();
		cout << "dispModeorig" << endl;
		//drawPoints(selectedPixels);
	}
	else
	{
		imageResult.draw();
	}
}


void testSVG()
{
    SVGWriter writer(SVGWriter::Point2D(-3,-3), SVGWriter::Point2D(3,3));
	writer.openNewSVGDocument("SVGTestFile.svg");
	{
		writer.startArea(1,0,0);
		{
			writer.addPoint(0,0);
			writer.addPoint(2,0);
			writer.addPoint(2,2);
			writer.addPoint(0,2);

			writer.addNewAreaComponent();
			{
				//inverse orientation to create a hole.
				writer.addPoint(0.5,1.5);
				writer.addPoint(1.5,1.5);
				writer.addPoint(1.5,0.5);
				writer.addPoint(0.5,0.5);
			}
		}
		writer.stopArea();

		writer.startArea(0,0,1);
		writer.addBezierPoint(-1,0,0,1,0,1,1,0);
		writer.stopArea();
	}
	writer.closeSVGDocument();
}

void mouse(int button, int state, int x, int y) {
    if (state!=0) return;
	const int i = x;
    const int j = image.h-1 - y;
    
	//utilisateur a cliqué sur pixel (i,j) dans l'image
	cout<<"PIXEL SELECTED:"<< i<<" "<<j << endl;
	//selectedPixels.push_back(PixelPos(i,j));
    glutPostRedisplay();
}

//______________________________________________________________________________
//______________________________________________________________________________
//Arretez vous ici!!!
//______________________________________________________________________________
//______________________________________________________________________________




/**
 * Appel des différentes fonctions de dessin
*/



void display(void);
void reshape(int w, int h);

/**
 * Programme principal
 */


int main(int argc, char** argv)
{
    glutInit(&argc, argv);


	if (argc>1)
	{
		if (!image.load(argv[1]))
			exit(1);
	}
	else
	{
		if (!image.load(filename))
			exit(1);
	}
	//copier pour initialiser...
	imageResult=image;

    // couches du framebuffer utilisees par l'application
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

    // position et taille de la fenetre
    glutInitWindowPosition(200, 100);
    glutInitWindowSize(image.w,image.h);
    glutCreateWindow(argv[0]);	

    // Initialisation du point de vue
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	//
    // Active la lumière
    // Pour la partie
    // ECLAIRAGE
         
    glDisable( GL_LIGHTING );
   	glEnable(GL_NORMALIZE);
    glClearColor (0.0, 0.0, 0.0, 0.0);

	// Details sur le mode de tracé
    glDisable( GL_DEPTH_TEST );            // effectuer le test de profondeur
    glShadeModel(GL_SMOOTH);


	// cablage des callback
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutDisplayFunc(display);

    // lancement de la boucle principale
    glutMainLoop();
        
	glDisable(GL_LIGHTING);

	return 0;  // instruction jamais exécutée
}

/**
 * Fonctions de gestion opengl à ne pas toucher
 */
// Actions d'affichage
// Ne pas changer
void display(void)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
    // Effacer tout
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT); // la couleur et le z
   
    dessiner( );    

    glutSwapBuffers();
	glPopAttrib();
}
// pour changement de taille ou desiconification
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

