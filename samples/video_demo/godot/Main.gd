extends Spatial

# these will be initialized in the _ready() function; but they will be only available
# on device
# the init config is needed for setting parameters that are needed before the VR system starts up
var ovr_init_config = null;

# the other APIs are available during runtime; details about the exposed functions can be found
# in the *.h files in https://github.com/GodotVR/godot_oculus_mobile/tree/master/src/config
var ovr_performance = null;

# some of the Oculus VrAPI constants are defined in this file. Have a look into it to learn more
var ovrVrApiTypes = null

var gast_loader = load("res://godot/plugin/v1/gast/GastLoader.gdns")
var gast = null
var gast_video_plugin = null

var curved = false

onready var second_screen: MeshInstance = $SecondScreen

func _ready():
	_initialize_second_screen_shader()
	_initialize_ovr_mobile_arvr_interface()
	gast = gast_loader.new()
	gast.initialize()
	gast.connect("release_input_event", self, "_on_gast_release_input_event")

	if Engine.has_singleton("gast-video"):
		print("Setting video player...")
		gast_video_plugin = Engine.get_singleton("gast-video")
		gast_video_plugin.preparePlayer("/root/Main/VideoContainer", "video-player", ["flight"])
		gast_video_plugin.play()
		gast_video_plugin.setRepeatMode(2)
		gast_video_plugin.setVideoScreenSize(3.54, 2)
	else:
		print("Unable to load gast-video singleton.")


func _process(delta_t):
	_check_inputs()
	_check_and_perform_runtime_config()
	_setup_second_screen()


func _physics_process(delta_t):
	gast.on_physics_process()


func _initialize_second_screen_shader():
	var shader_material : ShaderMaterial = second_screen.get_surface_material(0)
	var shader : Shader = shader_material.shader
	print("Setting shader custom defines")
	shader.custom_defines = """#ifdef ANDROID_ENABLED
	#extension GL_OES_EGL_image_external : enable
	#extension GL_OES_EGL_image_external_essl3 : enable
	#else
	#define samplerExternalOES sampler2D
	#endif"""


var _check_for_external_texture = false
func _setup_second_screen():
	if (_check_for_external_texture):
		return

	print("Checking for external texture.")
	var external_texture: ExternalTexture = gast.get_external_texture("video-player")
	if (external_texture == null):
		return

	print("Valid external texture with id " + str(external_texture.get_external_texture_id()))
	_check_for_external_texture = true

	# Setup the second screen
	second_screen.visible = true
	var shader_material : ShaderMaterial = second_screen.get_surface_material(0)
	print("Setting shader params.")
	shader_material.set_shader_param("external_texture", external_texture)

func _update_shader_material_scrim_brightness(brightness : float):
	print("Updating shader material scrim brightness to " + str(brightness))
	var shader_materials : Array = gast.get_shader_materials("video-player")
	if (shader_materials == null or shader_materials.empty()):
		print("Unable to retrieve shader materials")
		return

	for shader_material in shader_materials:
		shader_material.set_shader_param("scrim_brightness", brightness)

func _check_inputs():
	if Input.is_action_pressed("ui_cancel"):
		if gast_video_plugin.isPlaying():
			print("Pausing playback")
			gast_video_plugin.pause()
			_update_shader_material_scrim_brightness(0.5)
		else:
			print("Resuming playback")
			gast_video_plugin.play()
			_update_shader_material_scrim_brightness(1.0)


func _on_gast_release_input_event(node_path: String, event_origin_id: String, x_percent: float, y_percent: float):
	curved = !curved
	gast_video_plugin.setVideoScreenCurved(curved)


# this code check for the OVRMobile inteface; and if successful also initializes the
# .gdns APIs used to communicate with the VR device
func _initialize_ovr_mobile_arvr_interface():
	# Find the OVRMobile interface and initialise it if available
	var arvr_interface = ARVRServer.find_interface("OVRMobile")
	if !arvr_interface:
		print("Couldn't find OVRMobile interface")
	else:
		ovrVrApiTypes = load("res://addons/godot_ovrmobile/OvrVrApiTypes.gd").new()

		# the init config needs to be done before arvr_interface.initialize()
		ovr_init_config = load("res://addons/godot_ovrmobile/OvrInitConfig.gdns")
		if (ovr_init_config):
			ovr_init_config = ovr_init_config.new()
			ovr_init_config.set_render_target_size_multiplier(1) # setting to 1 here is the default

		# Configure the interface init parameters.
		if arvr_interface.initialize():
			get_viewport().arvr = true
			Engine.iterations_per_second = 72 # Quest

			# load the .gdns classes.
			ovr_performance = load("res://addons/godot_ovrmobile/OvrPerformance.gdns");
			if (ovr_performance): ovr_performance = ovr_performance.new()

			print("Loaded OVRMobile")
		else:
			print("Failed to enable OVRMobile")


# many settings should only be applied once when running; this variable
# gets reset on application start or when it wakes up from sleep
var _performed_runtime_config = false

# here we can react on the android specific notifications
# reacting on NOTIFICATION_APP_RESUMED is necessary as the OVR context will get
# recreated when the Android device wakes up from sleep and then all settings wil
# need to be reapplied
func _notification(what):
	if (what == NOTIFICATION_APP_RESUMED):
		_performed_runtime_config = false # redo runtime config

func _check_and_perform_runtime_config():
	if _performed_runtime_config: return

	if (ovr_performance):
		# these are some examples of using the ovr .gdns APIs
		ovr_performance.set_clock_levels(1, 1)
		ovr_performance.set_extra_latency_mode(ovrVrApiTypes.OvrExtraLatencyMode.VRAPI_EXTRA_LATENCY_MODE_OFF)
		ovr_performance.set_foveation_level(0);  # 0 == off; 4 == highest

	_performed_runtime_config = true
