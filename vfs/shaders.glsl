
////V_PASSTHROUGH
#version 100
attribute vec2 aPos;
void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
}

////F_VRAM
#version 100 
precision mediump float;
uniform sampler2D uFramebuffer; 

vec4 color_lookup(int idx) 
{
	vec2 clr = vec2( mod(float(idx), 64.0)*4.0 + 0.5, floor(float(idx)/64.0) + 144.5);
	vec4 rgba=vec4(0.0,0.0,0.0,0.0);
	rgba.r = texture2D(uFramebuffer, clr/256.0).a;
	clr.x += 1.0;
	rgba.g = texture2D(uFramebuffer, clr/256.0).a;
	clr.x += 1.0;
	rgba.b = texture2D(uFramebuffer, clr/256.0).a;
	clr.x += 1.0;
	rgba.a = texture2D(uFramebuffer, clr/256.0).a;
	return rgba;
}

int lookup(int r, int c)
{
	vec4 pix = texture2D(uFramebuffer, vec2(float(c)+0.5, float(r)+0.5)/256.0 );
	return int(pix.a*255.99);
}

void main()
{ 
	int r = int(gl_FragCoord.y);
	int c = int(gl_FragCoord.x);
	int index = lookup(r, c);
	gl_FragColor = color_lookup(index);
}

////F_NEAREST
#version 100 
precision mediump float;
uniform sampler2D uFramebuffer; 
uniform vec2 uSize;
void main() { 
	int c = int(gl_FragCoord.x *256.0 / uSize.x);
	int r = 143-int(gl_FragCoord.y * 144.0/uSize.y);//144.0
	vec4 s =  texture2D(uFramebuffer, vec2((float(c)+0.5)/256.0, (float(r)+0.5)/256.0));
	gl_FragColor = s;
}

////F_TILE
#version 100
precision highp float;
uniform sampler2D uFramebuffer;
uniform vec2 uSize;
uniform vec2 uOffset;
uniform float uAddress;
uniform bvec2 uClip;
uniform bvec2 uClamp;
uniform bvec2 uFlip;

vec4 palette_lookup(float index)
{
	vec2 xy = (vec2(mod(index,64.0)*4.0, floor(index/64.0)+144.0)+0.5)/256.0;
	vec4 rgba;
	rgba.r = texture2D(uFramebuffer, xy).a;
	xy.x += 1.0/256.0;
	rgba.g = texture2D(uFramebuffer, xy).a;
	xy.x += 1.0/256.0;
	rgba.b = texture2D(uFramebuffer, xy).a;
	xy.x += 1.0/256.0;
	rgba.a = texture2D(uFramebuffer, xy).a;
	return rgba;
}

float get_data(float index)
{
	return floor(texture2D(uFramebuffer,  (vec2(mod(index, 256.0), floor(index/256.0))+0.5) / 256.0).a*255.99);
}

vec2 get_mapel(vec2 tile)
{
	float address = (tile.y*uSize.x + tile.x)*2.0 + uAddress;
	return vec2(get_data(address), get_data(address+1.0));
}

float get_sprite_texel(float sprite_id, vec2 px)
{
	float byte = get_data(148.0*256.0 + 32.0*sprite_id + px.y*4.0 + floor(px.x/2.0));
	if (px.x == 0.0 || px.x == 2.0 || px.x == 4.0 || px.x == 6.0)
		return floor(byte / 16.0);
	return mod(byte,16.0);
}

void main() {
	vec2 px = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y)) + uOffset;
	vec2 subpx= mod(px, 8.0);
	vec2 tile = floor(px/8.0);
	if (uClip.x && (tile.x >= uSize.x || tile.x < 0.0))
		discard;
	if (uClip.y && (tile.y >= uSize.y || tile.y < 0.0))
		discard;
	
	bvec2 flip = bvec2(false, false);
	
	if (uClamp.x) {
		if (uFlip.x)
			flip.x = bool(mod(tile.x, 2.0));
		tile.x = (tile.x < 0.0) ? 0.0 : ((tile.x >= uSize.x)? uSize.x-1.0 : tile.x);
	} else {
		tile.x = mod(tile.x, uSize.x);
	}
		
	if (uClamp.y) {
		if (uFlip.y)
			flip.y = bool(mod(tile.y, 2.0));
		tile.y = (tile.y < 0.0) ? 0.0 : ((tile.y >= uSize.y)? uSize.y-1.0 : tile.y);
		
	} else {
		tile.y = mod(tile.y, uSize.y);
	}
	
	vec2 mapel = get_mapel(tile);
	if (mapel.y >= 128.0) 
		flip.x = !flip.x;
	if ( mod(mapel.y, 128.0) >= 64.0) 
		flip.y = !flip.y;
	
	if (flip.x)
		subpx.x = 7.0-subpx.x;
	if (flip.y)
		subpx.y = 7.0-subpx.y;
	
	gl_FragColor = palette_lookup(get_sprite_texel(mapel.x, subpx));//vec4(clri/4.0, 0.0, 0.0 , 0.0);
}

////
