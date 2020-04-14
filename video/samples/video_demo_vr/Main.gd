extends Spatial

# these will be initialized in the _ready() function; but they will be only available
# on device
# the init config is needed for setting parameters that are needed before the VR system starts up
var ovr_init_config = null;

# the other APIs are available during runtime; details about the exposed functions can be found
# in the *.h files in https://github.com/GodotVR/godot_oculus_mobile/tree/master/src/config
var ovr_performance = null;
var ovr_display_refresh_rate = null;
var ovr_guardian_system = null;
var ovr_tracking_transform = null;
var ovr_utilities = null;
var ovr_vr_api_proxy = null;


# some of the Oculus VrAPI constants are defined in this file. Have a look into it to learn more
var ovrVrApiTypes = load("res://addons/godot_ovrmobile/OvrVrApiTypes.gd").new();

var gast_loader = load("res://godot/plugin/v1/gast/GastLoader.gdns")

func _ready():
	_initialize_ovr_mobile_arvr_interface()
	var gast = gast_loader.new()
	gast.initialize()
	
	if Engine.has_singleton("gast-video"):
		print("Setting video player...")
		var gast_video_plugin = Engine.get_singleton("gast-video")
		gast_video_plugin.preparePlayer("/root/Main/StaticBody/CollisionShape/MeshInstance", ["flight"])
		gast_video_plugin.play()
		gast_video_plugin.setRepeatMode(2)
		gast_video_plugin.setVideoScreenSize(3, 2)
	else:
		print("Unable to load gast-video singleton.")

func _process(delta_t):
	_check_and_perform_runtime_config()
	
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
			ovr_display_refresh_rate = load("res://addons/godot_ovrmobile/OvrDisplayRefreshRate.gdns");
			ovr_guardian_system = load("res://addons/godot_ovrmobile/OvrGuardianSystem.gdns");
			ovr_performance = load("res://addons/godot_ovrmobile/OvrPerformance.gdns");
			ovr_tracking_transform = load("res://addons/godot_ovrmobile/OvrTrackingTransform.gdns");
			ovr_utilities = load("res://addons/godot_ovrmobile/OvrUtilities.gdns");
			ovr_vr_api_proxy = load("res://addons/godot_ovrmobile/OvrVrApiProxy.gdns");

			# and now instance the .gdns classes for use if load was successfull
			if (ovr_display_refresh_rate): ovr_display_refresh_rate = ovr_display_refresh_rate.new()
			if (ovr_guardian_system): ovr_guardian_system = ovr_guardian_system.new()
			if (ovr_performance): ovr_performance = ovr_performance.new()
			if (ovr_tracking_transform): ovr_tracking_transform = ovr_tracking_transform.new()
			if (ovr_utilities): ovr_utilities = ovr_utilities.new()
			if (ovr_vr_api_proxy): ovr_vr_api_proxy = ovr_vr_api_proxy.new()

			print("Loaded OVRMobile")
			return true
		else:
			print("Failed to enable OVRMobile")
			return false



# many settings should only be applied once when running; this variable
# gets reset on application start or when it wakes up from sleep
var _performed_runtime_config = false

# here we can react on the android specific notifications
# reacting on NOTIFICATION_APP_RESUMED is necessary as the OVR context will get
# recreated when the Android device wakes up from sleep and then all settings wil
# need to be reapplied
func _notification(what):
	if (what == NOTIFICATION_APP_PAUSED):
		pass
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
