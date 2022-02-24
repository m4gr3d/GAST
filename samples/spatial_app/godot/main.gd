extends Spatial

var gast_loader = load("res://godot/plugin/v1/gast/GastLoader.gdns")
var gast = null

var interface : ARVRInterface
var start_passthrough = false
export (NodePath) var viewport = null

func _ready():
	#_initialise_openxr_interface()
	gast = gast_loader.new()
	gast.initialize()


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

		# Start passthrough?
		_start_passthrough()

		# Connect to our plugin signals
		_connect_plugin_signals()

		# Change our viewport so it is tied to our ARVR interface and renders to our HMD
		vp.arvr = true

		# Our interface will tell us whether we should keep our render buffer in linear color space
		vp.keep_3d_linear = $Configuration.keep_3d_linear()

		# increase our physics engine update speed
		var refresh_rate = $Configuration.get_refresh_rate()
		if refresh_rate == 0:
			# Only Facebook Reality Labs supports this at this time
			print("No refresh rate given by XR runtime")

			# Use something sufficiently high
			Engine.iterations_per_second = 144
		else:
			print("HMD refresh rate is set to " + str(refresh_rate))

			# Match our physics to our HMD
			Engine.iterations_per_second = refresh_rate

		emit_signal("initialised")
		return true
	else:
		emit_signal("failed_initialisation")
		return false

func _get_xr_viewport() -> Viewport:
	if viewport:
		var vp : Viewport = get_node(viewport)
		return vp
	else:
		return get_viewport()

func _start_passthrough():
	if start_passthrough:
		# make sure our viewports background is transparent
		_get_xr_viewport().transparent_bg = true

		# enable our passthrough
		$Configuration.start_passthrough()

func _connect_plugin_signals():
	ARVRServer.connect("openxr_session_begun", self, "_on_openxr_session_begun")
	ARVRServer.connect("openxr_session_ending", self, "_on_openxr_session_ending")
	ARVRServer.connect("openxr_focused_state", self, "_on_openxr_focused_state")
	ARVRServer.connect("openxr_visible_state", self, "_on_openxr_visible_state")
	ARVRServer.connect("openxr_pose_recentered", self, "_on_openxr_pose_recentered")

func _on_openxr_session_begun():
	print("OpenXR session begun")
	emit_signal("session_begun")

func _on_openxr_session_ending():
	print("OpenXR session ending")
	emit_signal("session_ending")

func _on_openxr_focused_state():
	print("OpenXR focused state")
	emit_signal("focused_state")

func _on_openxr_visible_state():
	print("OpenXR visible state")
	emit_signal("visible_state")

func _on_openxr_pose_recentered():
	print("OpenXR pose recentered")
	emit_signal("pose_recentered")

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
