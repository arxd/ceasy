
////V_PASSTHROUGH
#version 100
attribute vec2 aPos;
void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
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

////F_LAYER
#version 100
precision highp float;
uniform sampler2D uFramebuffer;
uniform vec2 uOffset; // offset of the map on the screen
uniform float uMap; // start of map
uniform vec2 uMapSize; //width/height of map
uniform float uTiles; // start of the tiles
uniform vec2 uTileSize; // width/height of a tile
uniform float uTileRowSize; 
uniform float uPalette; // start of the palette entries
uniform int uTileBits;
uniform bvec2 uClip;
uniform bvec2 uFlip;
uniform bvec2 uClamp;

float mem(float idx)
{
	return texture2D(uFramebuffer,  (vec2(mod(idx, 512.0), floor(idx/512.0))+0.5) / 512.0).a;
}

float memi(float idx)
{
	return floor(mem(idx)*(256.0 - 0.5));
}

vec4 palette(float idx)
{
	return vec4(mem(idx), mem(idx+1.0), mem(idx+2.0), mem(idx+3.0));
}

float tile_texel_4bit(float idx, vec2 subpix)
{
	float byte = memi(uTiles + (idx*uTileSize.y + subpix.y)*uTileRowSize + floor(subpix.x/2.0));
	if (mod(subpix.x, 2.0) == 0.0)
		return floor(byte/16.0);
	return mod(byte, 16.0);
}

float tile_texel_1bit(float idx, vec2 subpix)
{
	float byte = memi(uTiles + (idx*uTileSize.y + subpix.y)*uTileRowSize + floor(subpix.x/8.0));
	float bit = mod(subpix.x, 8.0);
	if (bit == 0.0)
		return floor(byte / 128.0);
	if (bit == 1.0)
		return mod(floor(byte / 64.0), 2.0);
	if (bit == 2.0)
		return mod(floor(byte / 32.0), 2.0);
	if (bit == 3.0)
		return mod(floor(byte / 16.0), 2.0);
	if (bit == 4.0)
		return mod(floor(byte / 8.0), 2.0);
	if (bit == 5.0)
		return mod(floor(byte / 4.0), 2.0);
	if (bit == 6.0)
		return mod(floor(byte / 2.0), 2.0);
	return mod(byte, 2.0);
}

float tile_texel_2bit(float idx, vec2 subpix)
{
	float byte = memi(uTiles + (idx*uTileSize.y + subpix.y)*uTileRowSize + floor(subpix.x/4.0));
	float bit = mod(subpix.x, 4.0);
	if (bit == 0.0)
		return floor(byte / 64.0);
	if (bit == 1.0)
		return mod(floor(byte / 16.0), 4.0);
	if (bit == 2.0)
		return mod(floor(byte / 4.0), 4.0);
	return mod(byte, 4.0);
}


void main()
{
	vec2 px = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y)) + uOffset;
	vec2 tile = floor(px / uTileSize);
	vec2 subpx = mod(px, uTileSize);
	bvec2 flip = bvec2(false, false);
	
	if (uClip.x && (tile.x >= uMapSize.x || tile.x < 0.0))
		discard;
	if (uClip.y && (tile.y >= uMapSize.y || tile.y < 0.0))
		discard;
	
	if (uClamp.x) {
		if (uFlip.x)
			flip.x = bool(mod(tile.x, 2.0));
		tile.x = (tile.x < 0.0) ? 0.0 : ((tile.x >= uMapSize.x)? uMapSize.x-1.0 : tile.x);
	} else {
		tile.x = mod(tile.x, uMapSize.x);
	}
		
	if (uClamp.y) {
		if (uFlip.y)
			flip.y = bool(mod(tile.y, 2.0));
		tile.y = (tile.y < 0.0) ? 0.0 : ((tile.y >= uMapSize.y)? uMapSize.y-1.0 : tile.y);
		
	} else {
		tile.y = mod(tile.y, uMapSize.y);
	}
	
	float clridx = 8.0;
	if (uTileSize.x*uTileSize.y == 1.0) {
		clridx = memi(uMap + uMapSize.x*tile.y + tile.x);
	} else {
		if (uTileBits == 4)
			clridx = tile_texel_4bit(memi(uMap + uMapSize.x*tile.y + tile.x), subpx);
		else if (uTileBits == 1)
			clridx = tile_texel_1bit(memi(uMap + uMapSize.x*tile.y + tile.x), subpx);
		else if (uTileBits == 2)
			clridx = tile_texel_2bit(memi(uMap + uMapSize.x*tile.y + tile.x), subpx);
	}
	gl_FragColor = palette(uPalette + 4.0*clridx);
}

////
