extends Spatial

# these will be initialized in the _ready() function; but they will be only available
# on device
# the init config is needed for setting parameters that are needed before the VR system starts up
var ovr_init_config = null;

# the other APIs are available during runtime; details about the exposed functions can be found
# in the *.h files in https://github.com/GodotVR/godot_oculus_mobile/tree/master/src/config
var ovr_performance = null;

# some of the Oculus VrAPI constants are defined in this file. Have a look into it to learn more
var ovrVrApiTypes = load("res://addons/godot_ovrmobile/OvrVrApiTypes.gd").new();

var gast_loader = load("res://godot/plugin/v1/gast/GastLoader.gdns")
var gast = null
var gast_video_plugin = null

var curved = false

func _ready():
	_initialize_ovr_mobile_arvr_interface()
	gast = gast_loader.new()
	gast.initialize()
	gast.connect("release_input_event", self, "_on_gast_release_input_event")

	if Engine.has_singleton("gast-video"):
		print("Setting video player...")
		gast_video_plugin = Engine.get_singleton("gast-video")
		gast_video_plugin.preparePlayer("/root/Main/VideoContainer", ["flight"])
		gast_video_plugin.play()
		gast_video_plugin.setRepeatMode(2)
		gast_video_plugin.setVideoScreenSize(3.54, 2)
	else:
		print("Unable to load gast-video singleton.")


func _process(delta_t):
	gast.on_process()
	_check_inputs()
	_check_and_perform_runtime_config()


func _check_inputs():
	if Input.is_action_pressed("ui_cancel"):
		if gast_video_plugin.isPlaying():
			print("Pausing playback")
			gast_video_plugin.pause()
		else:
			print("Resuming playback")
			gast_video_plugin.play()


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
		# the init config needs to be done before arvr_interface.initialize()
		ovr_init_config = load("res://addons/godot_ovrmobile/OvrInitConfig.gdns");
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
