//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() );
	((TraceUI*)(o->user_data()))->raytracer->getScene()->setDepth(((TraceUI*)(o->user_data()))->m_nDepth);
}

//distance attenuation
void TraceUI::cb_attenuationConstantSlides(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->getScene()->setConstant(double(((Fl_Slider *)o)->value()));
}
void TraceUI::cb_attenuationLinearSlides(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->getScene()->setLinear(double(((Fl_Slider *)o)->value()));
}
void TraceUI::cb_attenuationQuadricSlides(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->getScene()->setQuadric(double(((Fl_Slider *)o)->value()));
}
//ambient light
void TraceUI::cb_ambientLightSlides(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->getScene()->setAmbientLight(double(((Fl_Slider *)o)->value()));
}

//anti-aliasing
void TraceUI::cb_Anti_Super_button(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setSupperSamping(!pUI->raytracer->getSupperSamping());
	if(pUI->raytracer->getSupperSamping())
		pUI->m_AntiAdaptSampButton->deactivate();
	else
		pUI->m_AntiAdaptSampButton->activate();
}
void TraceUI::cb_Anti_Adaptive_button(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setAdaptive(!pUI->raytracer->getAdaptive());
	if (pUI->raytracer->getAdaptive())
		pUI->m_AntiSuperSampButton->deactivate();
	else
		pUI->m_AntiSuperSampButton->activate();
}
void TraceUI::cb_antiAliasingGridSizeSlides(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setGridSize(int(((Fl_Slider *)o)->value()));
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_mainWindow = new Fl_Window(100, 40, 350, 400, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 350, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		// install slider attenuation constant
		m_attenuationConstantSlider = new Fl_Value_Slider(10, 80, 180, 20, "Attenuation, Constant");
		m_attenuationConstantSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenuationConstantSlider->type(FL_HOR_NICE_SLIDER);
		m_attenuationConstantSlider->labelfont(FL_COURIER);
		m_attenuationConstantSlider->labelsize(12);
		m_attenuationConstantSlider->minimum(0);
		m_attenuationConstantSlider->maximum(1.0);
		m_attenuationConstantSlider->step(0.01);
		m_attenuationConstantSlider->value(0.25);
		m_attenuationConstantSlider->align(FL_ALIGN_RIGHT);
		m_attenuationConstantSlider->callback(cb_attenuationConstantSlides);

		// install slider attenuation linear
		m_attenuationLinearSlider = new Fl_Value_Slider(10, 105, 180, 20, "Atteunation, Linear");
		m_attenuationLinearSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenuationLinearSlider->type(FL_HOR_NICE_SLIDER);
		m_attenuationLinearSlider->labelfont(FL_COURIER);
		m_attenuationLinearSlider->labelsize(12);
		m_attenuationLinearSlider->minimum(0);
		m_attenuationLinearSlider->maximum(1);
		m_attenuationLinearSlider->step(0.01);
		m_attenuationLinearSlider->value(0.25);
		m_attenuationLinearSlider->align(FL_ALIGN_RIGHT);
		m_attenuationLinearSlider->callback(cb_attenuationLinearSlides);

		// install slider quadric
		m_attenuationQuadricSlider = new Fl_Value_Slider(10, 130, 180, 20, "Attenuation, Quadric");
		m_attenuationQuadricSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenuationQuadricSlider->type(FL_HOR_NICE_SLIDER);
		m_attenuationQuadricSlider->labelfont(FL_COURIER);
		m_attenuationQuadricSlider->labelsize(12);
		m_attenuationQuadricSlider->minimum(0);
		m_attenuationQuadricSlider->maximum(1);
		m_attenuationQuadricSlider->step(0.01);
		m_attenuationQuadricSlider->value(0.5);
		m_attenuationQuadricSlider->align(FL_ALIGN_RIGHT);
		m_attenuationQuadricSlider->callback(cb_attenuationQuadricSlides);

		// install slider ambient light
		m_ambientLightSlider = new Fl_Value_Slider(10, 155, 180, 20, "Ambient Light");
		m_ambientLightSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightSlider->labelfont(FL_COURIER);
		m_ambientLightSlider->labelsize(12);
		m_ambientLightSlider->minimum(0);
		m_ambientLightSlider->maximum(1);
		m_ambientLightSlider->step(0.01);
		m_ambientLightSlider->value(0.2);
		m_ambientLightSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightSlider->callback(cb_ambientLightSlides);

		m_renderButton = new Fl_Button(270, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(270, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		//anti aliasing by supper sampling button
		m_AntiSuperSampButton = new Fl_Light_Button(10, 180, 130, 25, "&Supper Sampling");
		m_AntiSuperSampButton->user_data((void*)(this));
		m_AntiSuperSampButton->callback(cb_Anti_Super_button);
		m_AntiSuperSampButton->value(false);

		//anti aliasing by supper sampling button
		m_AntiAdaptSampButton = new Fl_Light_Button(150, 180, 190, 25, "&Adaptive Supper Sampling");
		m_AntiAdaptSampButton->user_data((void*)(this));
		m_AntiAdaptSampButton->callback(cb_Anti_Adaptive_button);
		m_AntiAdaptSampButton->value(false);

		//anti aliasing grid size slider
		// install slider ambient light
		m_ambientLightSlider = new Fl_Value_Slider(10, 210, 180, 20, "Number of sup-pixel");
		m_ambientLightSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightSlider->labelfont(FL_COURIER);
		m_ambientLightSlider->labelsize(12);
		m_ambientLightSlider->minimum(1);
		m_ambientLightSlider->maximum(5);
		m_ambientLightSlider->step(1);
		m_ambientLightSlider->value(3);
		m_ambientLightSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightSlider->callback(cb_antiAliasingGridSizeSlides);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}