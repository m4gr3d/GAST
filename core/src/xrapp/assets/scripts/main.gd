extends Spatial

# These resources are only available at runtime.
var openxr_config = load("res://addons/godot-openxr/config/OpenXRConfig.gdns").new()
var gast_loader = load("res://godot/plugin/v1/gast/GastLoader.gdns")

var gast = null

var interface : ARVRInterface
export (NodePath) var viewport = null

onready var floor_node = $Floor
onready var left_hand_nodes = $ARVROrigin/LeftHandNodes
onready var right_hand_nodes = $ARVROrigin/RightHandNodes

func _ready():
	if (_is_xr_enabled()):
		_initialise_openxr_interface()
	else:
		print("Skipping OpenXR initialization")
	gast = gast_loader.new()
	gast.initialize()

func _is_xr_enabled():
	var appPlugin = Engine.get_singleton("GastAppPlugin")
	if (appPlugin):
		return appPlugin.isXREnabled()
	else:
		print("App plugin is not available")
	return true

func _process(delta_t):
	_check_and_perform_runtime_config()

func _physics_process(delta_t):
	gast.on_physics_process()

func _initialise_openxr_interface() -> bool:
	if interface:
		# we are already initialised
		return true

	interface = ARVRServer.find_interface("OpenXR")
	if interface and interface.initialize():
		print("OpenXR Interface initialized")

		# Find the viewport we're using to render our XR output
		var vp : Viewport = _get_xr_viewport()

		# Connect to our plugin signals
		_connect_plugin_signals()

		# Change our viewport so it is tied to our ARVR interface and renders to our HMD
		vp.arvr = true

		_initialize_openxr_configuration()
		_configure_hands()

		emit_signal("initialised")
		return true
	else:
		emit_signal("failed_initialisation")
		return false

func _initialize_openxr_configuration():
	var vp : Viewport = _get_xr_viewport()

	# Our interface will tell us whether we should keep our render buffer in linear color space
	vp.keep_3d_linear = openxr_config.keep_3d_linear()

	# increase our physics engine update speed
	var refresh_rate = openxr_config.get_refresh_rate()
	if refresh_rate == 0:
		# Only Facebook Reality Labs supports this at this time
		print("No refresh rate given by XR runtime")

		# Use something sufficiently high
		Engine.iterations_per_second = 144
	else:
		print("HMD refresh rate is set to " + str(refresh_rate))

		# Match our physics to our HMD
		Engine.iterations_per_second = refresh_rate

func _configure_hands():
	left_hand_nodes.set_hand(0)
	right_hand_nodes.set_hand(1)

func _get_xr_viewport() -> Viewport:
	if viewport:
		var vp : Viewport = get_node(viewport)
		return vp
	else:
		return get_viewport()

func _start_passthrough():
	# make sure our viewports background is transparent
	_get_xr_viewport().transparent_bg = true

	# Hide the floor
	floor_node.visible = false

	# enable our passthrough
	openxr_config.start_passthrough()

func _stop_passthrough():
	# Revert our viewports' background transparency
	_get_xr_viewport().transparent_bg = false

	# Disable passthrough
	openxr_config.stop_passthrough()

	# Show the floor
	floor_node.visible = true

func _connect_plugin_signals():
	var appPlugin = Engine.get_singleton("GastAppPlugin")
	if (appPlugin):
		appPlugin.connect("start_passthrough", self, "_start_passthrough")
		appPlugin.connect("stop_passthrough", self, "_stop_passthrough")

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

	_performed_runtime_config = true
