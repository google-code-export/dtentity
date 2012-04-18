uniform sampler2D diffuseTexture;
varying vec3 vNormal;
varying float vFog;
varying vec4 vFrontColor;


void lightContribution(vec3 normal, vec3 lightDir, vec3 diffuseLightSource, vec3 ambientLightSource, out vec3 lightContrib)
{
   // Light contribution considers the light impacting the surface. Since that 
   // is too dramatic, we weight the effect of light against a straight up vector.
   float diffuseSurfaceContrib = max(dot(normal, lightDir),0.0);
   float fUpContribution = max(dot(vec3(0.0, 0.0, 1.0), lightDir), -0.1);
   fUpContribution = (fUpContribution + 0.1) / 1.1;  // we use a bit past horizontal, else it darkens too soon.
   float diffuseContrib = fUpContribution * 0.42 + diffuseSurfaceContrib * 0.62;

   // Lit Color (Diffuse plus Ambient)
   vec3 diffuseLight = diffuseLightSource * diffuseContrib;
   lightContrib = vec3(diffuseLight + ambientLightSource);
}

void main(void)
{
   vec4 diffuseColor = texture2D(diffuseTexture, gl_TexCoord[0].st);
   vec3 lightDir = normalize(gl_LightSource[0].position);
   vec3 lightContrib;
   lightContribution(vNormal, lightDir, vec3(gl_LightSource[0].diffuse), vec3(gl_LightSource[0].ambient), lightContrib);

   gl_FragColor.rgb = diffuseColor.rgb * lightContrib;
   gl_FragColor.a = diffuseColor.a * vFrontColor.a ;
   gl_FragColor = vec4(mix(gl_FragColor.rgb, gl_Fog.color.rgb, vFog), gl_FragColor.a);
}
