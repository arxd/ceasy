////V_RECT
#version 100
precision mediump float;
attribute vec2 aPos;
uniform vec2 uScale;
uniform vec2 uSize;
uniform vec2 uOffset;

void main()
{
	gl_Position = vec4((uScale*aPos + uOffset)/uSize*2.0 - 1.0, 0.0, 1.0);
}

////V_PLAYER
#version 100
precision mediump float;
attribute vec2 aPos;
uniform float uScale;
uniform vec2 uSize;
uniform vec2 uOffset;
uniform float uAngle;

void main()
{
	float c = cos(uAngle);
	float s = sin(uAngle);
	vec2 pos = mat2(c, s, -s, c)*aPos;
	
	gl_Position = vec4((uScale*pos + uOffset)/uSize*2.0 - 1.0, 0.0, 1.0);
}


////F_PLAYER
#version 100
#define M_PI 3.1415926535897932384626433832795

precision mediump float;
uniform float uScale;
uniform vec2 uOffset;
//~ uniform float uSize; // circle radius
//~ uniform float thrust;
//~ uniform float thrust_angle;
uniform float uAngle;
uniform float uThrust;
uniform float uGrab;

void main()
{
	vec2 diff = gl_FragCoord.xy - uOffset;
	float angle = mod(atan(diff.y, diff.x)-uAngle+M_PI, 2.0*M_PI);
	float radius = sqrt(diff.x*diff.x + diff.y*diff.y) / uScale;
	
	// grab
	vec4 glow_color = vec4(1.0, 0.0, 0.0, 1.0);
	float glow = step(1.0, radius) * smoothstep(1.0001+uGrab*0.5, 1.0, radius);
	
	// thrust
	vec4 thrust_color = vec4(0.0, 0.0, 1.0, 1.0);
	float thrust = smoothstep(M_PI*170.0/180.0,M_PI*175.0/180.0,angle)
		* smoothstep(M_PI*170.0/180.0,M_PI*175.0/180.0, 2.0*M_PI-angle)
		*step(1.0, radius)
		*smoothstep(1.0001+uThrust*6.0, 1.0, radius);

	// inside
	vec4 inside_color = vec4(0.1, 0.5, 0.2, 1.0);
	float inside = 1.0-step(1.0, radius);
	
	
	gl_FragColor =  thrust*thrust_color + glow*glow_color + inside*inside_color;
	
	//~ if (radius > 1.0) {
		//~ float alpha = ;
		//~ gl_FragColor = vec4(vec3(1.0, 0.0, 0.00), alpha);
	//~ } else if (radius > 0.95) {
		//~ gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	//~ } else {
		//~ float alpha = radius;
		//~ gl_FragColor = vec4(0.3, 0.8, 0.1, alpha);
	//~ }
}

////F_CIRCLE
#version 100
precision mediump float;
uniform vec2 uScale;
uniform float uInnerRadius;
uniform vec3 uColor;
uniform vec2 uOffset;

void main()
{
	vec2 diff = gl_FragCoord.xy - uOffset;
	float radius = sqrt(diff.x*diff.x + diff.y*diff.y) / uScale.x;
	gl_FragColor = vec4(uColor, 1.0)*(1.0-step(1.0, radius));
	
	
}

////F_RECT
#version 100
precision mediump float;
//~ uniform float uScale;
//~ uniform float uInnerRadius;
uniform vec3 uColor;
//~ uniform vec2 uOffset;

void main()
{
	//~ vec2 diff = gl_FragCoord.xy - uOffset;
	//~ float radius = sqrt(diff.x*diff.x + diff.y*diff.y) / uScale;
	gl_FragColor = vec4(uColor, 1.0);
	
	
}

////
