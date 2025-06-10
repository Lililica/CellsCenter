#version 330 core


precision mediump float;

// twigl: https://twigl.app?ol=true&ss=-OSL30EEeldP-salQBTQ

// Sorties du shader
in vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
in vec3 vNormal_vs; // Normale du sommet transformé dans l'espace View
in vec2 vTexCoords; // Coordonnées de texture du sommet

in vec2 mNormal_vs; // Normale du sommet transformé dans l'espace View


uniform vec2 uScreenSize; // Résolution de l'écran, utilisée pour les calculs de projection

out vec4 oColor; // Couleur de sortie pour chaque pixel

// This is the main shader function that runs for every pixel on screen
// O = output color for this pixel, C = pixel coordinates (0,0 to screen width/height)
void main() {
    vec2 C = vTexCoords.xy*10; // Get the current pixel coordinates
  // Variable declarations:
  // d = distance to closest surface, s = scale factor, j/i = loop counters
  // z = depth along ray, N = which string iteration was closest
  // D = temporary distance, k = coordinate scaling factor, t = current time
  float t = vPosition_vs.x + vPosition_vs.y; // Use pixel coordinates to create a time variable
  float d,s,j,i,z,N,D,k;
  
  // o = accumulated color/brightness, p = 3D position, U = color constants
  vec4 o,p,U=vec4(3,1,2,0);
  
  // Main raymarching loop
  // r = screen resolution, used to convert pixel coords to camera direction
  for (vec2 q,r=uScreenSize.xy; ++i<70.; z += .5*d+1E-3) {
    
    // Calculate 3D ray direction from 2D pixel coordinates
    // Center coordinates around (0,0) and normalize to create camera ray
    p = vec4(z*normalize(vec3(C-.5*r, r.y)), 0);
    
    // Move camera back 3 units from origin
    p.z -= 3.;
    
    // Rotate the 3D space to an interesting camera angle
    p.xz *= mat2(.8,.6,-.8,.6);
    
    // Apply "inversion" transformation aka @mla inversion
    p *= k = 8./dot(p,p);
    
    // Take 2D slice of 3D position for string calculation
    q = p.xy;
    
    // Create repeating pattern by subtracting rounded grid positions
    // This tiles the fractal across space in 5x5 unit cells
    q -= round(q/5.)*5.;
    
    // Create rotation matrix that changes over time and space
    // log2(k) makes rotation depend on inversiondepth, creating swirling motion
    mat2 R = mat2(cos(.5*t+log2(k)+11.*U.wxyw));
    
    // Inner loop: Generate fractal shape by iterating a formula
    for (
        d=s=j=1.          // Initialize: distance=1, scale=1, counter=1
      ; ++j<6.            
      ; s *= .5           // Halve the scale each iteration
      )
         q = abs(q*R) - 2.*s/j     // Fold space, rotate, and offset
      ,  D = length(q) - s/8.      // Calculate distance to current shape
      ,  D < d ? N = j, d = D : 0. // Keep track of closest distance and which iteration
      ;
    
    // Correct distance accounting for the inversion transformation
    //  Abs makes the strings translucent
    d = abs(d)/k;
    
    // Calculate color based on position and string properties
    // sin() creates oscillating colors, N determines which string layer
    p = 1.+sin(p.z+U.zywz-t+N);
    
    // Accumulate color and lighting:
    o += 
        p.w/max(d, 1E-3)*p        // Add color inversely proportional to distance (closer = brighter)
      + exp(.3*k)*6.*U            // Add glow effect that increases with depth
      ;
    
  }
  
  // Final color output: normalize accumulated brightness and apply tone mapping
  // tanh() prevents colors from becoming too bright (tone mapping)
  oColor = tanh(o/3e4);
}