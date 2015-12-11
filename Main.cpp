// ----------------------------------------------
// Informatique Graphique 3D & R�alit� Virtuelle.
// Travaux Pratiques
// Algorithmes de Rendu
// Copyright (C) 2015 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <GL/glut.h>

#include "Vec3.h"
#include "Camera.h"
#include "Mesh.h"
//#include "Ray.h"
#include "copie de Ray.h"

using namespace std;

static const unsigned int DEFAULT_SCREENWIDTH = 1024;
static const unsigned int DEFAULT_SCREENHEIGHT = 768;
static const string DEFAULT_MESH_FILE ("models/man.off");

static string appTitle ("Informatique Graphique & Realite Virtuelle - Travaux Pratiques - Algorithmes de Rendu");
static GLint window;
static unsigned int FPS = 0;
static bool fullScreen = false;

static Camera camera;
static Mesh mesh;
static bool* is_eclaired;
static Vec3d light_source;
double geometrique_ggx(double alpha,Vec3d normal,Vec3d w);
void printUsage () {
	std::cerr << std::endl
	<< appTitle << std::endl
	<< "Author: Tamy Boubekeur" << std::endl << std::endl
	<< "Usage: ./main [<file.off>]" << std::endl
	<< "Commands:" << std::endl
	<< "------------------" << std::endl
	<< " ?: Print help" << std::endl
	<< " w: Toggle wireframe mode" << std::endl
	<< " <drag>+<left button>: rotate model" << std::endl
	<< " <drag>+<right button>: move model" << std::endl
	<< " <drag>+<middle button>: zoom" << std::endl
	<< " q, <esc>: Quit" << std::endl << std::endl;
}

void init (const char * modelFilename) {
	glCullFace (GL_BACK);     // Specifies the faces to cull (here the ones pointing away from the camera)
	glEnable (GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
	glDepthFunc (GL_LESS); // Specify the depth test for the z-buffer
	glEnable (GL_DEPTH_TEST); // Enable the z-buffer in the rasterization
	glLineWidth (2.0); // Set the width of edges in GL_LINE polygon mode
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f); // Background color
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);

	mesh.loadOFF (modelFilename);
	camera.resize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);

	light_source = Vec3d(10, 10, 10);
	is_eclaired = new bool[mesh.V.size()];

	std::cout << "mesh vertex size = " << mesh.V.size() << std::endl;

	int is_shadow = 0;
	for (unsigned int i = 0; i < mesh.V.size (); i++)
	{
		is_eclaired[i] = true;
		const Vertex & v = mesh.V[i];
		Vec3d pos = Vec3d(v.p);
		// std::cout << "mesh triangle size = " << mesh.T.size() << std::endl;
		Ray ray(pos, light_source - pos);
		//std::cout << "ray.origin = " << ray.origin[0] << " ; " << ray.origin[1] << " ; " << ray.origin[2] << std::endl;
		//	std::cout << "ray.direction = " << ray.direction[0] << " ; " << ray.direction[1] << " ; " << ray.direction[2] << std::endl;

		for(unsigned int k = 0;k<mesh.T.size();k++){
			// if(k == i) continue;//dont do the same triangle!
			const Vertex & v0 = mesh.V[mesh.T[k].v[0]];
			const Vertex & v1 = mesh.V[mesh.T[k].v[1]];
			const Vertex & v2 = mesh.V[mesh.T[k].v[2]];
			Vec3d vp0 = Vec3d(v0.p);
			Vec3d vp1 = Vec3d(v1.p);
			Vec3d vp2 = Vec3d(v2.p);
			if(ray.is_intersect_with(vp0,vp1,vp2))
			{
				is_eclaired[i] = false;
				//std::cout << "goes here!"  << std::endl;
				is_shadow++;
				break;
			}
		}
	}
	std::cerr << std::endl << is_shadow/**1.0/mesh.V.size()*/<< std::endl ;
}

void drawScene () {
	glBegin (GL_TRIANGLES);
	for (unsigned int i = 0; i < mesh.T.size (); i++)
	{
		for (unsigned int j = 0; j < 3; j++) {
			const Vertex & v = mesh.V[mesh.T[i].v[j]];
			int number = mesh.T[i].v[j];
			// EXERCISE : the following color response shall be replaced with a proper reflectance evaluation/shadow test/etc.
			//	glColor3f (1.0, 1.0, 1.0);
			//L(w0) = li(wi)*alpha*	(normal * direction de lumiaire)
			glNormal3f (v.n[0], v.n[1], v.n[2]); // Specifies current normal vertex
			double L;
			Vec3d pos = Vec3d(v.p);
			float c_x,c_y,c_z;
			camera.getPos(c_x,c_y,c_z);
			Vec3d cam_pos = Vec3d(c_x,c_y,c_z);
			Vec3d normal = Vec3d(v.n[0], v.n[1], v.n[2]);
			Vec3d w_i = normalize(light_source - pos);
			Vec3d w_o = normalize(cam_pos - pos);
			Vec3d w_h = normalize(w_o + w_i);
			if(is_eclaired[number] == false)
			{
				L = 0;
			}
			else {
				/*
				double lambert = 0.5;
				//double L = 0.2*lambert * (dot(normal,lumi));
				float c_x,c_y,c_z;
				camera.getPos(c_x,c_y,c_z);
				Vec3d cam_pos = Vec3d(c_x,c_y,c_z);
				double ks = 1;
				double s = 10;
				double phong = ks * pow(max(0.0,dot(normal, w_h)),s);
				//double L = 1 * (lambert + phong)*max(0.0,dot(normal,w_i));
				*/

				//Distribution GGX
				double alpha = 0.8;
				double D = pow(alpha,2)/(M_PI*pow(1 + (pow(alpha,2)-1)*pow(dot(normal,w_h),2.0) ,2));

				//Fresnel
				double F0 = 0.05;
				double F = F0 + (1-F0)*pow(1-max(0.0, dot(w_i,w_h)) ,5);

				// terme geometrique GGX
				double G = geometrique_ggx(alpha, normal, w_i)*geometrique_ggx(alpha, normal, w_o);

				//micro-facette
				double fs_mf = D*F*G/(4*dot(normal,w_i)*dot(normal,w_o));

				L = 100*fs_mf*max(0.0,dot(normal,w_i));
			}
			glColor3f (L, L, L);
			glVertex3f (v.p[0], v.p[1], v.p[2]); // Emit a vertex (one triangle is emitted each time 3 vertices are emitted)
		}
	}
	glEnd ();
}
double geometrique_ggx(double alpha,Vec3d normal,Vec3d w){
	double temp = max(0.0,dot(normal,w));
	return (2 * temp)/(temp + sqrt(alpha * alpha + (1 - alpha * alpha)* temp*temp ));
}

void reshape(int w, int h) {
	camera.resize (w, h);
}

void display () {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.apply ();
	drawScene ();
	glFlush ();
	glutSwapBuffers ();
}

void key (unsigned char keyPressed, int x, int y) {
	switch (keyPressed) {
		case 'f':
		if (fullScreen) {
			glutReshapeWindow (camera.getScreenWidth (), camera.getScreenHeight ());
			fullScreen = false;
		} else {
			glutFullScreen ();
			fullScreen = true;
		}
		break;
		case 'q':
		case 27:
		delete is_eclaired;
		exit (0);
		break;
		case 'w':
		GLint mode[2];
		glGetIntegerv (GL_POLYGON_MODE, mode);
		glPolygonMode (GL_FRONT_AND_BACK, mode[1] ==  GL_FILL ? GL_LINE : GL_FILL);
		break;
		break;
		default:
		printUsage ();
		break;
	}
}

void mouse (int button, int state, int x, int y) {
	camera.handleMouseClickEvent (button, state, x, y);
}

void motion (int x, int y) {
	camera.handleMouseMoveEvent (x, y);
}

void idle () {
	static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
	static unsigned int counter = 0;
	counter++;
	float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
	if (currentTime - lastTime >= 1000.0f) {
		FPS = counter;
		counter = 0;
		static char winTitle [128];
		unsigned int numOfTriangles = mesh.T.size ();
		sprintf (winTitle, "Number Of Triangles: %d - FPS: %d", numOfTriangles, FPS);
		glutSetWindowTitle (winTitle);
		lastTime = currentTime;
	}
	glutPostRedisplay ();
}

int main (int argc, char ** argv) {
	if (argc > 2) {
		printUsage ();
		exit (1);
	}
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
	window = glutCreateWindow (appTitle.c_str ());
	init (argc == 2 ? argv[1] : DEFAULT_MESH_FILE.c_str ());
	glutIdleFunc (idle);
	glutReshapeFunc (reshape);
	glutDisplayFunc (display);
	glutKeyboardFunc (key);
	glutMotionFunc (motion);
	glutMouseFunc (mouse);
	printUsage ();
	glutMainLoop ();
	return 0;
}
