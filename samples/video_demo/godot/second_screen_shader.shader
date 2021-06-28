shader_type spatial;

render_mode unshaded;

uniform samplerExternalOES external_texture;

void fragment() {
	ALBEDO = texture(external_texture, UV).rgb;
}
