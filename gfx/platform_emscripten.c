

double timer_start;
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE g_context;

int g_cursor_cache_i;
int g_cursor_cache[32];


void death(const char *title, const char *err_str, int errid)
{
	if (title)
		printf("%s:%d: %s\n", title, errid, err_str);
	emscripten_webgl_destroy_context(g_context);
	emscripten_force_exit(errid);
}



void time_init(void)
{
	timer_start = emscripten_get_now();
}

double cur_time(void)
{
	return (emscripten_get_now() - timer_start)/1000.0;
}

int size_change(int x, const void *what, void *userdata)
{
	int fs;
	emscripten_get_canvas_size(&fb_width, &fb_height, &fs);
	printf("Resize %d %d\n", fb_width, fb_height);
	
	return 1;
	
}

EM_BOOL fullscreenchange_callback(int type, const EmscriptenFullscreenChangeEvent *evt, void *userData)
{
	printf("Full screen %d,  %d (enabled %d)  (%d, %d),  (%d, %d)\n", type, evt->isFullscreen, evt->fullscreenEnabled,
		evt->elementWidth, evt->elementHeight, evt->screenWidth, evt->screenHeight);
	
	return 1;
}

void do_fullscreen()
{
	EmscriptenFullscreenStrategy strategy;
	strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_ASPECT;
	strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF;
	strategy.filteringMode =  EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST;
	strategy.canvasResizedCallback = size_change;
	strategy.canvasResizedCallbackUserData = NULL;
	emscripten_request_fullscreen_strategy("canvas",1, &strategy);
	
}

EM_BOOL key_callback(int type, const EmscriptenKeyboardEvent *evt, void *userData)
{
	if (evt->key[0] == 'F' && evt->key[1] == '1' && evt->key[2] == '1')  {
		do_fullscreen();
	} else if (evt->key[0] == 'E' && evt->key[1] == 's' && evt->key[2] == 'c')  {
		death("DONE", "", 0);
	}		
	
	
	return 1;
	
}

EM_BOOL mouse_callback(int type, const EmscriptenMouseEvent *evt, void *userData)
{
	double xpos, ypos;
	int fs;
	emscripten_get_canvas_size(&fb_width, &fb_height, &fs);
	ypos = fb_height - evt->canvasY;
	ypos = (ypos) * 18.0 / fb_height;
	xpos = evt->canvasX * 32.0 / fb_width;
	cur_xy = V(xpos, ypos);

	if (type == EMSCRIPTEN_EVENT_MOUSEDOWN && evt->button == 0) {
		if (g_cursor_cache_i == 32)
			death("EVENT", "Mouse event cache overflow!", -1);
		g_cursor_cache[g_cursor_cache_i++] = PRESS;
	} else if (type ==  EMSCRIPTEN_EVENT_MOUSEUP && evt->button == 0) {
		if (g_cursor_cache_i == 32)
			death("EVENT", "Mouse event cache overflow!", -1);
		g_cursor_cache[g_cursor_cache_i++] = RELEASE;
	}
	return 1;
}

void init_platform()
{
	bg_width = EM_ASM_INT_V(return screen.width;);
	bg_height = EM_ASM_INT_V(return screen.height;);
	
	printf("%d %d\n", bg_width, bg_height);
	
	emscripten_set_element_css_size("canvas", 1280, 720);
	double ratio = emscripten_get_device_pixel_ratio();
	printf("Pixel ration = %f\n", ratio);
	emscripten_set_canvas_size(960,540);
	
	int fs;
	emscripten_get_canvas_size(&fb_width, &fb_height, &fs);
	
	int r;
	EmscriptenWebGLContextAttributes attrs;
	emscripten_webgl_init_context_attributes(&attrs);
	attrs.alpha = 0;
	attrs.premultipliedAlpha = 0;
	//~ attrs.majorVersion=2;
	//~ attrs.minorVersion=0;
	
	if ((g_context = emscripten_webgl_create_context("canvas", &attrs)) <= 0) {
		printf("emscripten_webgl_create_context: %d\n", g_context);
		exit(1);
	}
	
	if ( (r=emscripten_webgl_make_context_current(g_context)) < 0) {
		printf("emscripten_webgl_make_context_current: %d\n", r);
		exit(1);
	}
	
	g_cursor_cache_i = 0;
	cur_state = HOVER;
	g_drawing_bg = 0;
	
	emscripten_set_mousemove_callback("canvas", NULL, 1, mouse_callback);
	emscripten_set_mouseup_callback("canvas", NULL, 1, mouse_callback);
	emscripten_set_mousedown_callback("canvas", NULL, 1, mouse_callback);
	emscripten_set_keypress_callback("#window", NULL, 1, key_callback);
	//emscripten_set_keydown_callback("#window", NULL, 1, key_callback);
	emscripten_set_fullscreenchange_callback("canvas", NULL, 1, fullscreenchange_callback);
	//emscripten_set_keyup_callback("#window", NULL, 1, key_callback);

}

void render_loop()
{
	if (g_cursor_cache_i) {
		cur_state = g_cursor_cache[--g_cursor_cache_i];
	} else {
		if (cur_state == PRESS)
			cur_state = HELD;
		else if (cur_state == RELEASE)
			cur_state = HOVER;
	}
	reset_gl();
	draw_background();
	render();
}

int main()
{
	init_platform();
	init_objects();
	init_gl();
	init();
	time_init();
        emscripten_set_main_loop(render_loop, 0, 1);
	death(NULL, NULL, 0);
	
	return 0;
}

