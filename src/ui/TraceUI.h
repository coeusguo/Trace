//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;

	Fl_Slider*			m_attenuationConstantSlider;
	Fl_Slider*			m_attenuationLinearSlider;
	Fl_Slider*			m_attenuationQuadricSlider;
	Fl_Slider*			m_ambientLightSlider;
	Fl_Slider*			m_AntiAliasingGridSiseSlider;


	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;
	Fl_Button*			m_AntiSuperSampButton;
	Fl_Button*			m_AntiAdaptSampButton;
	Fl_Button*			m_JitterButton;
	Fl_Button*			m_backgroundButton;
	Fl_Button*			m_textureButton;
	Fl_Button*			m_BumpButton;



	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;

	//distanceAttenuation
	double		m_nQuadric;
	double		m_nLinear;
	double		m_nConstant;

	//ambient light
	double		m_nAmbientLight;

	//anti-aliasing
	bool		m_nSuperSampling;
	bool		m_nAdaptive;
	bool		m_nJitter;

	//background
	bool		m_nbackground;
	//texture mapping
	bool		m_nTexture;
	bool		m_nBump;
// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);
	static void cb_background_image(Fl_Menu_* o, void* v);
	static void cb_texture_image(Fl_Menu_* o, void* v);
	static void cb_load_normal_map(Fl_Menu_* o, void* v);
	static void cb_height_field_map(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);
	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_attenuationConstantSlides(Fl_Widget* o, void* v);
	static void cb_attenuationLinearSlides(Fl_Widget* o, void* v);
	static void cb_attenuationQuadricSlides(Fl_Widget* o, void* v);
	static void cb_ambientLightSlides(Fl_Widget* o, void* v);
	static void cb_Anti_Super_button(Fl_Widget* o, void* v);
	static void cb_Anti_Adaptive_button(Fl_Widget* o, void* v);
	static void cb_antiAliasingGridSizeSlides(Fl_Widget* o, void* v);
	static void cb_Jitter_button(Fl_Widget* o, void* v);
	static void cb_Background_button(Fl_Widget* o, void* v);
	static void cb_Texture_button(Fl_Widget* o, void* v);
	static void cb_Bump_mapping_button(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
