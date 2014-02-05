#version 120

uniform sampler2D texture1;
uniform float width;
uniform float height;

varying vec2 uv;

//out vec4 fragColor;

void main(void) {
  float dx = 1.0 / width;
  float dy = 1.0 / height;

  // Harris detector
  //  float m1 = abs ( texture2D ( texture1, uv + vec2(dx,dy) ) 
  //		   - texture2D( texture1, uv ) );
  //  float m2 = abs ( texture2D ( texture1, uv + vec2(dx,0) ) 
  //		   - texture2D( texture1, uv + vec2(dy,0) ) );

  //  float x1 = min (uv.x+dx, 1.0);
  //  float y1 = min (uv.y+dy, 1.0);
  //  float x1 = clamp (uv.x+dx, 0.0, 1.0);
  //  float y1 = clamp (uv.y+dy, 0.0, 1.0);
  vec2 xy = clamp(uv + vec2(dx,dy), 0.0, 1.0);

  const vec4 w = vec4 ( 0.2125, 0.7154, 0.0721 , 0.0);

  float m1 = dot (   texture2D ( texture1, uv ) - texture2D ( texture1, xy ), 
		     w );

  float m2 = dot (   texture2D ( texture1, vec2(uv.x, xy.y) ) - texture2D ( texture1, vec2(xy.x, uv.y) ),
		     w ); 

  //  float m1 = length (   texture2D ( texture1, uv ) 
  //			- texture2D ( texture1, xy ) );
  //
  //  float m2 = length (   texture2D ( texture1, vec2(uv.x, xy.y) )
  //			- texture2D ( texture1, vec2(xy.x, uv.y) ) ); 

  // float m1 = length ( texture2D ( texture1, uv + vec2(dx,dy) ) 
  // 		      - texture2D( texture1, uv ) );
  // float m2 = length ( texture2D ( texture1, uv + vec2(dx,0) ) 
  // 		      - texture2D( texture1, uv + vec2(0,dy) ) );

  float mag = sqrt(m1*m1+m2*m2);

  //  gl_FragColor = vec4(mag,mag,mag,1.0);
  //  gl_FragColor = vec4(m1,m2,mag,1.0);

  gl_FragColor = vec4(mag*(m1+0.5),
		      mag*(m2+0.5),
		      mag,
		      1.0);

}
