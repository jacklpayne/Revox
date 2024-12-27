#version 330 core

in vec3 Normal;
in vec3 frag_pos;
in vec3 color;

out vec4 FragColor;

uniform vec3 light_color;
uniform vec3 light_pos;

void main()
{
    // Take difference between unit normal vector and light position
    // To get vector between the two
    vec3 norm = normalize(Normal); 
    vec3 light_dir = normalize(light_pos - frag_pos);

    // Take dot product and multiply by color to get diffused light color
    float diff = max(dot(norm,light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // Add light color to ambient and multiply to get obj color
    vec3 ambient_color = {0.2,0.2,0.2};
    vec3 result = (ambient_color + diffuse) * color;

    FragColor = vec4(result, 1.0);
} 