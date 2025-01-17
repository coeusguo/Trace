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
		pUI->m_BumpButton->deactivate();
		pUI->m_nBump = false;
		pUI->m_BumpButton->value(false);
		pUI->m_textureButton->deactivate();
		pUI->m_nTexture = false;
		pUI->m_textureButton->value(false);
		pUI->m_mainWindow->label(buf);
		pUI->raytracer->getScene()->setDepth(pUI->m_nDepth);
		pUI->raytracer->getScene()->setQuadric(pUI->m_nQuadric);
		pUI->raytracer->getScene()->setLinear(pUI->m_nLinear);
		pUI->raytracer->getScene()->setConstant(pUI->m_nConstant);
		pUI->raytracer->getScene()->setAmbientLightFactor(pUI->m_nAmbientLight);
		pUI->raytracer->getScene()->setUsingTexture(pUI->m_nTexture);
		pUI->raytracer->getScene()->setUsingBump(pUI->m_nBump);
		pUI->raytracer->getScene()->setEnableOctree(pUI->m_nEnableOctree);
		pUI->raytracer->getScene()->iniOctree(pUI->m_nOctreeX, pUI->m_nOctreeY, pUI->m_nOctreeZ, pUI->m_nOctreeXSize, pUI->m_nOctreeYSize, pUI->m_nOctreeZSize, pUI->m_nOctreeDepth);
		pUI->raytracer->getScene()->setCaustic(pUI->m_nEnableCaustic);
	}
}
//load backgroung image
void TraceUI::cb_background_image(Fl_Menu_* o, void* v) {
	TraceUI* pUI = whoami(o);
	char* newfile = fl_file_chooser("Load Background Image?", "*.bmp", NULL);
	if (newfile != NULL) {
		pUI->raytracer->loadBackgroundImage(newfile);
		pUI->m_backgroundButton->activate();

	}
}

//load height field image
void TraceUI::cb_height_field_map(Fl_Menu_* o, void* v){
	TraceUI* pUI = whoami(o);
	char* newfile = fl_file_chooser("Load Height Field Image?", "*.bmp", NULL);
	if (newfile != NULL) {
		pUI->raytracer->getScene()->loadHeightFieldMap(newfile);
	}
}

//load texture image
void TraceUI::cb_texture_image(Fl_Menu_* o, void* v) {
	TraceUI* pUI = whoami(o);
	if (!pUI->raytracer->sceneLoaded())
		return;
	char* newfile = fl_file_chooser("Load Texture Image?", "*.bmp", NULL);
	if (newfile != NULL) {
		pUI->raytracer->getScene()->loadTextureImage(newfile);
		pUI->m_textureButton->activate();
		pUI->m_BumpButton->activate();
	}
}

void TraceUI::cb_load_normal_map(Fl_Menu_* o, void* v) {
	TraceUI* pUI = whoami(o);
	if (!pUI->raytracer->sceneLoaded())
		return;
	char* newfile = fl_file_chooser("Load Texture Image?", "*.bmp", NULL);
	if (newfile != NULL) {
		pUI->raytracer->getScene()->loadNormalMap(newfile);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
		//pUI->raytracer->getScene()->saveImage(savefile);
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
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nDepth=int( ((Fl_Slider *)o)->value() );
	if (pUI->raytracer->sceneLoaded()) {
		pUI->raytracer->getScene()->setDepth(pUI->m_nDepth);
	}
}

//distance attenuation
void TraceUI::cb_attenuationConstantSlides(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nConstant = double(((Fl_Slider *)o)->value());
	if(pUI->raytracer->sceneLoaded())
		pUI->raytracer->getScene()->setConstant(double(((Fl_Slider *)o)->value()));
}
void TraceUI::cb_attenuationLinearSlides(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nLinear = double(((Fl_Slider *)o)->value());
	if (pUI->raytracer->sceneLoaded())
		pUI->raytracer->getScene()->setLinear(double(((Fl_Slider *)o)->value()));
}
void TraceUI::cb_attenuationQuadricSlides(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nQuadric = double(((Fl_Slider *)o)->value());
	if (pUI->raytracer->sceneLoaded())
		pUI->raytracer->getScene()->setQuadric(double(((Fl_Slider *)o)->value()));
}
//ambient light
void TraceUI::cb_ambientLightSlides(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nAmbientLight = double(((Fl_Slider *)o)->value());
	if (pUI->raytracer->sceneLoaded())
		pUI->raytracer->getScene()->setAmbientLightFactor(double(((Fl_Slider *)o)->value()));
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

void TraceUI::cb_Jitter_button(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setJitter(!pUI->raytracer->getAdaptive());
}

//background image
void TraceUI::cb_Background_button(Fl_Widget* o, void* v){
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setUsingBackgroundImage(!pUI->raytracer->getUisingBackgroundImage());
}

//texture image
void TraceUI::cb_Texture_button(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->getScene()->setUsingTexture(!pUI->raytracer->getScene()->getUsingTexture());
}
//bump mapping
void TraceUI::cb_Bump_mapping_button(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->getScene()->setUsingBump(!pUI->raytracer->getScene()->getUsingBump());
}

//depth of field
void TraceUI::cb_depth_of_field_button(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setEnableDepthOfField(!pUI->raytracer->getEnableDepthofField());
}
void TraceUI::cb_focal_length_Slides(Fl_Widget* o, void* v){
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nFocalLength = ((Fl_Slider *)o)->value();
	pUI->raytracer->setFocalLength(pUI->m_nFocalLength);
}
void TraceUI::cb_aperture_size_slider(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nApertureSize = ((Fl_Slider *)o)->value();
	pUI->raytracer->setApertureSize(pUI->m_nApertureSize);
}

//glossy reflection
void TraceUI::cb_glossy_reflection(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setEnableGlossy(!pUI->raytracer->getEnableGlossy());
}

//soft shadow
void TraceUI::cb_soft_shadow(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setEnableSoftShadow(!pUI->raytracer->getEnableSoftShadow());
}

//motion blur
void TraceUI::cb_motion_blur(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nEnableMotionBlur = !pUI->raytracer->getEnableMotionBlur();
	pUI->raytracer->setEnableMotionBlur(pUI->m_nEnableMotionBlur);
}
//octree
void TraceUI::cb_octree(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nEnableOctree = ((Fl_Button *)o)->value();
	pUI->raytracer->getScene()->setEnableOctree(pUI->m_nEnableOctree);
}
void TraceUI::cb_octree_depth(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nOctreeDepth = ((Fl_Slider *)o)->value();
}
void TraceUI::cb_octree_X(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nOctreeX = ((Fl_Slider *)o)->value();
}
void TraceUI::cb_octree_Y(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nOctreeY = ((Fl_Slider *)o)->value();
}
void TraceUI::cb_octree_Z(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nOctreeZ = ((Fl_Slider *)o)->value();
}
void TraceUI::cb_octree_XS(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nOctreeXSize = ((Fl_Slider *)o)->value();
}
void TraceUI::cb_octree_YS(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nOctreeYSize = ((Fl_Slider *)o)->value();
}
void TraceUI::cb_octree_ZS(Fl_Widget* o, void* v) {
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_nOctreeZSize = ((Fl_Slider *)o)->value();
}
void TraceUI::cb_octree_rebuild(Fl_Widget* o, void* v) {
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	if (pUI->raytracer->sceneLoaded())
		pUI->raytracer->getScene()->iniOctree(pUI->m_nOctreeX, pUI->m_nOctreeY, pUI->m_nOctreeZ, pUI->m_nOctreeXSize, pUI->m_nOctreeYSize, pUI->m_nOctreeZSize, pUI->m_nOctreeDepth);
}

//caustics
void TraceUI::cb_caustics_button(Fl_Widget* o, void* v) {
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	pUI->m_nEnableCaustic = ((Fl_Button *)o)->value();
	if (pUI->raytracer->sceneLoaded())
		pUI->raytracer->getScene()->setCaustic(pUI->m_nEnableCaustic);
}
void TraceUI::cb_built_photonMap(Fl_Widget* o, void* v) {
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	if (pUI->raytracer->sceneLoaded())
		pUI->raytracer->getScene()->initPhotonMap();
}

//adaptive termination
void TraceUI::cb_adaptive_termination(Fl_Widget* o, void* v) {
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	pUI->m_nThreshold = ((Fl_Slider *)o)->value();
	pUI->raytracer->setThreshold(pUI->m_nThreshold);
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
		{ "&Load Bacground Image",	FL_ALT + 'b', (Fl_Callback *)TraceUI::cb_background_image },
		{ "&Load Texture Image",	FL_ALT + 't', (Fl_Callback *)TraceUI::cb_texture_image },
		{ "&Load Normal Map",	FL_ALT + 'm', (Fl_Callback *)TraceUI::cb_load_normal_map },
		{ "&Load HField Map",	FL_ALT + 'h', (Fl_Callback *)TraceUI::cb_height_field_map},
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
	m_mainWindow = new Fl_Window(100, 40, 350, 480, "Ray <Not Loaded>");
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
		m_nConstant = 0.25;
		m_nLinear = 0.25;
		m_nQuadric = 0.5;
		m_nAmbientLight = 0.2;

		m_attenuationConstantSlider = new Fl_Value_Slider(10, 80, 180, 20, "Attenuation, Constant");
		m_attenuationConstantSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenuationConstantSlider->type(FL_HOR_NICE_SLIDER);
		m_attenuationConstantSlider->labelfont(FL_COURIER);
		m_attenuationConstantSlider->labelsize(12);
		m_attenuationConstantSlider->minimum(0);
		m_attenuationConstantSlider->maximum(1.0);
		m_attenuationConstantSlider->step(0.01);
		m_attenuationConstantSlider->value(m_nConstant);
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
		m_attenuationLinearSlider->value(m_nLinear);
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
		m_attenuationQuadricSlider->value(m_nQuadric);
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
		m_ambientLightSlider->value(m_nAmbientLight);
		m_ambientLightSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightSlider->callback(cb_ambientLightSlides);

		// install slider ambient light
		m_nThreshold = 0.00;
		m_ThresholdSlider = new Fl_Value_Slider(10, 180, 180, 20, "Threshold");
		m_ThresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ThresholdSlider->type(FL_HOR_NICE_SLIDER);
		m_ThresholdSlider->labelfont(FL_COURIER);
		m_ThresholdSlider->labelsize(12);
		m_ThresholdSlider->minimum(0);
		m_ThresholdSlider->maximum(1);
		m_ThresholdSlider->step(0.01);
		m_ThresholdSlider->value(m_nThreshold);
		m_ThresholdSlider->align(FL_ALIGN_RIGHT);
		m_ThresholdSlider->callback(cb_adaptive_termination);

		m_renderButton = new Fl_Button(270, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(270, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		//anti aliasing by supper sampling button
		m_nAdaptive = false;
		m_nJitter = false;
		m_nSuperSampling = false;
		m_AntiSuperSampButton = new Fl_Light_Button(10, 205, 120, 25, "&Supper Samp");
		m_AntiSuperSampButton->user_data((void*)(this));
		m_AntiSuperSampButton->callback(cb_Anti_Super_button);
		m_AntiSuperSampButton->value(m_nSuperSampling);

		//anti aliasing by supper sampling button
		m_AntiAdaptSampButton = new Fl_Light_Button(140, 205, 130, 25, "&Adapt Supper");
		m_AntiAdaptSampButton->user_data((void*)(this));
		m_AntiAdaptSampButton->callback(cb_Anti_Adaptive_button);
		m_AntiAdaptSampButton->value(m_nAdaptive);

		m_JitterButton = new Fl_Light_Button(280, 205, 60, 25, "&Jitter");
		m_JitterButton->user_data((void*)(this));
		m_JitterButton->callback(cb_Jitter_button);
		m_JitterButton->value(m_nJitter);

		//anti aliasing grid size slider
		// install slider ambient light
		m_ambientLightSlider = new Fl_Value_Slider(10, 235, 180, 20, "Number of sup-pixel");
		m_ambientLightSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightSlider->labelfont(FL_COURIER);
		m_ambientLightSlider->labelsize(12);
		m_ambientLightSlider->minimum(1);
		m_ambientLightSlider->maximum(5);
		m_ambientLightSlider->step(1);
		m_ambientLightSlider->value(2);
		m_ambientLightSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightSlider->callback(cb_antiAliasingGridSizeSlides);

		m_nbackground = false;
		m_backgroundButton = new Fl_Light_Button(10, 260, 110, 25, "&Background");
		m_backgroundButton->user_data((void*)(this));
		m_backgroundButton->callback(cb_Background_button);
		m_backgroundButton->value(m_nbackground);
		m_backgroundButton->deactivate();

		m_nBump = false;
		m_nTexture = false;
		m_textureButton = new Fl_Light_Button(130, 260, 80, 25, "&Texture");
		m_textureButton->user_data((void*)(this));
		m_textureButton->callback(cb_Texture_button);
		m_textureButton->value(m_nTexture);
		m_textureButton->deactivate();

		m_BumpButton = new Fl_Light_Button(220, 260, 120, 25, "&Bump mapping");
		m_BumpButton->user_data((void*)(this));
		m_BumpButton->callback(cb_Bump_mapping_button);
		m_BumpButton->value(m_nBump);
		m_BumpButton->deactivate();

		//depth of field
		m_nDepthOfField = false;
		m_nFocalLength = 2.0;
		m_nApertureSize = 2;
		m_DepthFieldButton = new Fl_Light_Button(10, 290, 60, 25, "&DOF");
		m_DepthFieldButton->user_data((void*)(this));
		m_DepthFieldButton->callback(cb_depth_of_field_button);
		m_DepthFieldButton->value(m_nDepthOfField);

		//glossy reflection
		m_nEnableGlossy = false;
		m_GlossyReflectionButton = new Fl_Light_Button(80, 290, 70, 25, "&Glossy");
		m_GlossyReflectionButton->user_data((void*)(this));
		m_GlossyReflectionButton->callback(cb_glossy_reflection);
		m_GlossyReflectionButton->value(m_nEnableGlossy);

		//soft shadow
		m_nEnableSoftShadow = false;
		m_SoftShadowButton = new Fl_Light_Button(160, 290, 100, 25, "&Soft Shadow");
		m_SoftShadowButton->user_data((void*)(this));
		m_SoftShadowButton->callback(cb_soft_shadow);
		m_SoftShadowButton->value(m_nEnableSoftShadow);

		//motion blur
		m_nEnableMotionBlur = false;
		m_MotionBlurButton = new Fl_Light_Button(270, 290, 70, 25, "&Motion");
		m_MotionBlurButton->user_data((void*)(this));
		m_MotionBlurButton->callback(cb_motion_blur);
		m_MotionBlurButton->value(m_nEnableSoftShadow);

		m_FocalLengthSlider = new Fl_Value_Slider(10, 320, 180, 20, "Focal Length");
		m_FocalLengthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_FocalLengthSlider->type(FL_HOR_NICE_SLIDER);
		m_FocalLengthSlider->labelfont(FL_COURIER);
		m_FocalLengthSlider->labelsize(12);
		m_FocalLengthSlider->minimum(1);
		m_FocalLengthSlider->maximum(5);
		m_FocalLengthSlider->step(0.01);
		m_FocalLengthSlider->value(m_nFocalLength);
		m_FocalLengthSlider->align(FL_ALIGN_RIGHT);
		m_FocalLengthSlider->callback(cb_focal_length_Slides);

		m_ApertureSizeSlider = new Fl_Value_Slider(10, 345, 180, 20, "Aperture Size");
		m_ApertureSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ApertureSizeSlider->type(FL_HOR_NICE_SLIDER);
		m_ApertureSizeSlider->labelfont(FL_COURIER);
		m_ApertureSizeSlider->labelsize(12);
		m_ApertureSizeSlider->minimum(1);
		m_ApertureSizeSlider->maximum(5);
		m_ApertureSizeSlider->step(1);
		m_ApertureSizeSlider->value(m_nApertureSize);
		m_ApertureSizeSlider->align(FL_ALIGN_RIGHT);
		m_ApertureSizeSlider->callback(cb_aperture_size_slider);

		//octree
		m_nEnableOctree = false;
		m_nOctreeDepth = 7;
		m_nOctreeX = -5;
		m_nOctreeY = -5;
		m_nOctreeZ = -5;
		m_nOctreeXSize = 10;
		m_nOctreeYSize = 10;
		m_nOctreeZSize = 10;

		m_OctreeButton = new Fl_Light_Button(10, 370, 80, 25, "&Octree");
		m_OctreeButton->user_data((void*)(this));
		m_OctreeButton->callback(cb_octree);
		m_OctreeButton->value(m_nEnableOctree);

		m_octreeDepthSlider = new Fl_Value_Slider(180, 372, 120, 20, "Depth");
		m_octreeDepthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_octreeDepthSlider->type(FL_HOR_NICE_SLIDER);
		m_octreeDepthSlider->labelfont(FL_COURIER);
		m_octreeDepthSlider->labelsize(12);
		m_octreeDepthSlider->minimum(1);
		m_octreeDepthSlider->maximum(7);
		m_octreeDepthSlider->step(1);
		m_octreeDepthSlider->value(m_nOctreeDepth);
		m_octreeDepthSlider->align(FL_ALIGN_RIGHT);
		m_octreeDepthSlider->callback(cb_octree_depth);

		m_BuildOctreeButton = new Fl_Button(100, 370, 70, 25, "&Rebuild");
		m_BuildOctreeButton->user_data((void*)(this));
		m_BuildOctreeButton->callback(cb_octree_rebuild);

		m_octreeXSlider = new Fl_Value_Slider(10, 400, 90, 20, "X");
		m_octreeXSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_octreeXSlider->type(FL_HOR_NICE_SLIDER);
		m_octreeXSlider->labelfont(FL_COURIER);
		m_octreeXSlider->labelsize(12);
		m_octreeXSlider->minimum(-5.0);
		m_octreeXSlider->maximum(5.0);
		m_octreeXSlider->step(0.01);
		m_octreeXSlider->value(m_nOctreeX);
		m_octreeXSlider->align(FL_ALIGN_RIGHT);
		m_octreeXSlider->callback(cb_octree_X);

		m_octreeYSlider = new Fl_Value_Slider(120, 400, 90, 20, "Y");
		m_octreeYSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_octreeYSlider->type(FL_HOR_NICE_SLIDER);
		m_octreeYSlider->labelfont(FL_COURIER);
		m_octreeYSlider->labelsize(12);
		m_octreeYSlider->minimum(-5);
		m_octreeYSlider->maximum(5);
		m_octreeYSlider->step(0.01);
		m_octreeYSlider->value(m_nOctreeY);
		m_octreeYSlider->align(FL_ALIGN_RIGHT);
		m_octreeYSlider->callback(cb_octree_Y);

		m_octreeZSlider = new Fl_Value_Slider(230, 400, 90, 20, "Z");
		m_octreeZSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_octreeZSlider->type(FL_HOR_NICE_SLIDER);
		m_octreeZSlider->labelfont(FL_COURIER);
		m_octreeZSlider->labelsize(12);
		m_octreeZSlider->minimum(-5);
		m_octreeZSlider->maximum(5);
		m_octreeZSlider->step(0.01);
		m_octreeZSlider->value(m_nOctreeZ);
		m_octreeZSlider->align(FL_ALIGN_RIGHT);
		m_octreeZSlider->callback(cb_octree_Z);

		m_octreeXSizeSlider = new Fl_Value_Slider(10, 425, 90, 20, "XS");
		m_octreeXSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_octreeXSizeSlider->type(FL_HOR_NICE_SLIDER);
		m_octreeXSizeSlider->labelfont(FL_COURIER);
		m_octreeXSizeSlider->labelsize(12);
		m_octreeXSizeSlider->minimum(0);
		m_octreeXSizeSlider->maximum(10);
		m_octreeXSizeSlider->step(0.01);
		m_octreeXSizeSlider->value(m_nOctreeXSize);
		m_octreeXSizeSlider->align(FL_ALIGN_RIGHT);
		m_octreeXSizeSlider->callback(cb_octree_XS);

		m_octreeYSizeSlider = new Fl_Value_Slider(120, 425, 90, 20, "YS");
		m_octreeYSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_octreeYSizeSlider->type(FL_HOR_NICE_SLIDER);
		m_octreeYSizeSlider->labelfont(FL_COURIER);
		m_octreeYSizeSlider->labelsize(12);
		m_octreeYSizeSlider->minimum(0);
		m_octreeYSizeSlider->maximum(10);
		m_octreeYSizeSlider->step(0.01);
		m_octreeYSizeSlider->value(m_nOctreeYSize);
		m_octreeYSizeSlider->align(FL_ALIGN_RIGHT);
		m_octreeYSizeSlider->callback(cb_octree_YS);

		m_octreeZSizeSlider = new Fl_Value_Slider(230, 425, 90, 20, "ZS");
		m_octreeZSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_octreeZSizeSlider->type(FL_HOR_NICE_SLIDER);
		m_octreeZSizeSlider->labelfont(FL_COURIER);
		m_octreeZSizeSlider->labelsize(12);
		m_octreeZSizeSlider->minimum(0);
		m_octreeZSizeSlider->maximum(10);
		m_octreeZSizeSlider->step(0.01);
		m_octreeZSizeSlider->value(m_nOctreeZSize);
		m_octreeZSizeSlider->align(FL_ALIGN_RIGHT);
		m_octreeZSizeSlider->callback(cb_octree_ZS);

		m_nEnableCaustic = false;
		m_EnableCausticButton = new Fl_Light_Button(120, 450, 130, 25, "&Enable Caustics");
		m_EnableCausticButton->user_data((void*)(this));
		m_EnableCausticButton->callback(cb_caustics_button);
		m_EnableCausticButton->value(m_nEnableCaustic);

		m_BuildPhotonMapButton = new Fl_Button(10, 450, 100, 25, "&Init Caustics");
		m_BuildPhotonMapButton->user_data((void*)(this));
		m_BuildPhotonMapButton->callback(cb_built_photonMap);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);

}