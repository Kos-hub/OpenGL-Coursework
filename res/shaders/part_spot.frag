// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light
{
	vec4 light_colour;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float power;
};
#endif

// Material data
#ifndef MATERIAL
#define MATERIAL
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};
#endif

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
	// Calculate direction to the light
	vec3 light_dir = normalize(spot.position - position);
	// Calculate distance to light
	float d = distance(spot.position, position);
	// Calculate attenuation value :  (constant + (linear * d) + (quadratic * d * d)
	float divider = pow(max(dot(-1.0* spot.direction, light_dir), 0.0), spot.power);	
	// Calculate spot light intensity :  (max( dot(light_dir, -direction), 0))^power
	float attenuation = divider / (spot.constant + (d * spot.linear) + (pow(d, 2.0) * spot.quadratic));
	// Calculate light colour:  (intensity / attenuation) * light_colour
	vec4 light_colour = spot.light_colour * attenuation;
	// Now use standard phong shading but using calculated light colour and direction
	vec4 diffuse = (mat.diffuse_reflection * light_colour) * max(dot(normal, light_dir), 0.0);
	vec3 half_vector = normalize(light_dir + view_dir);
	vec4 specular = (mat.specular_reflection * light_colour) * pow(max(dot(normal, half_vector), 0.0), mat.shininess);
	
	vec4 colour = ((mat.emissive + diffuse) * tex_colour) + specular;
	colour.a = 1.0;

	return colour;
}