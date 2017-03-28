//=============================================================================
// The following tree diagrams the execution path for the ray tracer (when it
// is run as a console application, the GUI complicates things a little).  Only
// the most salient features are shown here, the loadScene method, for example,
// has many, many sub-routines.  A good place to start with this project is a
// quick tour of these methods.  The ones that need to be completed by you are
// noted as such.  A quick explanation of the various methods follows the
// diagram.
//
// main
//  |
//  +- RayTracer::loadScene
//  |
//  +- RayTracer::traceSetup
//  |
//  +- RayTracer::traceLines
//        |
//        +- RayTracer::tracePixel
//              |
//              +- RayTracer::trace
//                    |
//                    +- Camera::rayThrough
//                    |
//                    +- RayTracer::traceRay
//                          |
//                          +- Scene::intersect
//                          |     |
//                          |     +- <Geometry>::intersect
//                          |           |
//                          |           +- <Geometry>::intersectLocal
//                          |
//                          +- isect::getMaterial
//                          |
//                          +- Material::shade
//
// The loadScene and traceSetup methods load a file and set up all the internal
// buffers necessary to render the scene.  The traceLines method begins the
// process of actually rendering the image, one scanline at a time.  It does
// this by calling tracePixel for each pixel in the image.  tracePixel is given
// a coordinate pair which is converted into an (x,y) screen coordinate and
// passed to trace.  The trace method calculates a ray from the camera position
// through the (x,y) coordinate and then calls traceRay to see if this ray
// actually intersects any objects in the scene.  The intersect method in
// Scene calls intersect on each object in the scene (part of your assignment
// is an acceleration or culling process that cuts this down significantly).
// Each object in the scene is a descendant of Geometry and has its own
// intersectLocal routine (you need to fill this method in for the Box class).
// The intersect method actually converts the ray into the coordinate frame
// of the object where intersectLocal can check for an intersection.
// Finally, if an intersection was found, the material for that object is
// obtained and used to shade the scene (you will need to provide the code to
// figure out the correct color for the shading).
//
//=============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <FL/Fl.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include <FL/fl_ask.h>

#include "ui/TraceUI.h"
#include "RayTracer.h"

#include "fileio/bitmap.h"

// ***********************************************************
// from getopt.cpp 
// it should be put in an include file.
//
extern int getopt(int argc, char **argv, char *optstring);
extern char* optarg;
extern int optind, opterr, optopt;
// ***********************************************************

RayTracer* theRayTracer;
TraceUI* traceUI;

//
// options from program parameters
//
int recursion_depth = 0;
int g_height;
int g_width = 150;
bool bReport = false;
char *progname, *rayName, *imgName;

void usage()
{
#ifdef WIN32
	fl_alert( "usage: %s [-r <#> -w <#> -t] [input.ray output.bmp]\n", progname );
#else
	fprintf( stderr, "usage: %s [options] [input.ray output.bmp]\n", progname );
	fprintf( stderr, "  -r <#>      set recurssion level (default %d)\n", recursion_depth );
	fprintf( stderr, "  -w <#>      set output image width (default %d)\n", g_width );
	fprintf( stderr, "  -t			report time statistics\n" );
#endif
}

bool processArgs(int argc, char **argv) {
	int i;

    while ( (i = getopt( argc, argv, "tr:w:h:" )) != EOF )
	{
		switch ( i )
		{
			case 't':
			bReport = true;
			break;
	    
			case 'r':
			recursion_depth = atoi( optarg );
			break;
	    
			case 'w':
			g_width = atoi( optarg );
			break;
	    
			case 'h':
			g_height = atoi( optarg );
			break;

			default:
			return false;
		}
    }

    if ( optind >= argc-1 )
    {
		fprintf( stderr, "no input and/or output name.\n" );
		return false;
    }

    rayName = argv[optind];
    imgName = argv[optind+1];

	return true;
}

// usage : ray [option] in.ray out.bmp
// Simply keying in ray will invoke a graphics mode version.
// Use "ray --help" to see the detailed usage.
// OK. I am lying. any illegal option such as "ray blahbalh" will print
// out the usage
//
// Graphics mode will be substantially slower than text mode because of
// event handling overhead.
int main(int argc, char **argv) {
	progname=argv[0];

	if (argc!=1) {
		// text mode
		if (!processArgs(argc, argv)) {
			usage();
			exit(1);
		}
		
		theRayTracer=new RayTracer();
		theRayTracer->loadScene(rayName);
	
		if (theRayTracer->sceneLoaded()) {
			g_height = (int)(g_width / theRayTracer->aspectRatio() + 0.5);

			theRayTracer->traceSetup(g_width, g_height);
		
			clock_t start, end;
			start=clock();

			theRayTracer->traceLines(0, g_height);
		
			end=clock();

			// save image
			unsigned char* buf;

			theRayTracer->getBuffer(buf, g_width, g_height);
			if (buf)
				writeBMP(imgName, g_width, g_height, buf); 

			if (bReport) {
				double t=(double)(end-start)/CLOCKS_PER_SEC;
#ifdef WIN32
				fl_message( "total time = %.3f seconds\n", t); 
#else
				fprintf( stderr, "total time = %.3f seconds\n", t); 
#endif
			}
		}

		return 1;
	} else {
		// graphics mode
		traceUI=new TraceUI();
		theRayTracer=new RayTracer();

		traceUI->setRayTracer(theRayTracer);

		Fl::visual(FL_DOUBLE|FL_INDEX);

		traceUI->show();

		return Fl::run();
	}
}
