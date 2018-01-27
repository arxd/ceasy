
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

////
