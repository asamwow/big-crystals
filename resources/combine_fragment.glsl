#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform int bloom;
uniform float exposure;

void main()
{             
    const float gamma = 2.2f;
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    if(bloom == 1)
        hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0f) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0f / gamma));
    // result = vec3(texture(depth, TexCoords).x / 10.f);
    FragColor = vec4(result, 1.0f);
}
