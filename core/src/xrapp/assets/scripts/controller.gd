extends ARVRController

const LEFT_TOUCH_CONTROLLER_ID = 1
const RIGHT_TOUCH_CONTROLLER_ID = 2
const LEFT_HAND_ID = 3
const RIGHT_HAND_ID = 4

const CLICK_SUFFIX = "_click"
const LEFT_SCROLL_SUFFIX = "_left_scroll"
const RIGHT_SCROLL_SUFFIX = "_right_scroll"
const UP_SCROLL_SUFFIX = "_up_scroll"
const DOWN_SCROLL_SUFFIX = "_down_scroll"

const X_AXIS = 0
const Y_AXIS = 1

enum ControllerButtons {
	VR_BUTTON_BY = 1,
	VR_GRIP = 2,
	VR_BUTTON_MENU = 3,
	VR_BUTTON_AX = 7,
	VR_THUMBSTICK = 14,
	VR_TRIGGER = 15
}

enum FingerButtons {
	MIDDLE_PINCH = 1,
	LITTLE_FINGER_PINCH = 2,
	MENU_PRESSED = 3,
	INDEX_PINCH = 7,
	RING_PINCH = 15
}

export var hide_for_no_tracking_confidence = false

var current_joystick_id = -1
# This resource is only available at runtime.
var openxr_config = load("res://addons/godot-openxr/config/OpenXRConfig.gdns").new()

onready var back_button_action = "back_button_action"
onready var menu_button_action = "menu_button_action"
onready var click_action = _get_ray_cast_name() + CLICK_SUFFIX
onready var left_scroll_action = _get_ray_cast_name() + LEFT_SCROLL_SUFFIX
onready var right_scroll_action = _get_ray_cast_name() + RIGHT_SCROLL_SUFFIX
onready var up_scroll_action = _get_ray_cast_name() + UP_SCROLL_SUFFIX
onready var down_scroll_action = _get_ray_cast_name() + DOWN_SCROLL_SUFFIX

func _ready():
	if !InputMap.has_action(back_button_action):
		InputMap.add_action(back_button_action, 0.0)

	if !InputMap.has_action(menu_button_action):
		InputMap.add_action(menu_button_action, 0.0)

	if !InputMap.has_action(click_action):
		InputMap.add_action(click_action, 0.0)

	if !InputMap.has_action(left_scroll_action):
		InputMap.add_action(left_scroll_action, 0.0)

	if !InputMap.has_action(right_scroll_action):
		InputMap.add_action(right_scroll_action, 0.0)

	if !InputMap.has_action(up_scroll_action):
		InputMap.add_action(up_scroll_action, 0.0)

	if !InputMap.has_action(down_scroll_action):
		InputMap.add_action(down_scroll_action, 0.0)

func _process(delta):
	var should_be_visible = true
	if get_is_active():
		if current_joystick_id == -1:
			current_joystick_id = get_joystick_id()
			_load_input_map(current_joystick_id)

		if hide_for_no_tracking_confidence:
			# Get the tracking confidence
			if openxr_config:
				var tracking_confidence = openxr_config.get_tracking_confidence(controller_id)
				if tracking_confidence == 0:
					should_be_visible = false
	else:
		should_be_visible = false
		if current_joystick_id != -1:
			_unload_input_map(current_joystick_id)
			current_joystick_id = -1

	if visible != should_be_visible:
		visible = should_be_visible

func _get_ray_cast_name():
	if controller_id == LEFT_TOUCH_CONTROLLER_ID:
		return "TouchLeftRayCast"
	elif controller_id == RIGHT_TOUCH_CONTROLLER_ID:
		return "TouchRightRayCast"
	elif controller_id == LEFT_HAND_ID:
		return "HandLeftRayCast"
	elif controller_id == RIGHT_HAND_ID:
		return "HandRightRayCast"

func _load_input_map(joystick_id: int):
	if controller_id == LEFT_TOUCH_CONTROLLER_ID or controller_id == RIGHT_TOUCH_CONTROLLER_ID:
		_load_controller_input_map(joystick_id)
	elif controller_id == LEFT_HAND_ID or controller_id == RIGHT_HAND_ID:
		_load_hand_input_map(joystick_id)

func _unload_input_map(joystick_id: int):
	if controller_id == LEFT_TOUCH_CONTROLLER_ID or controller_id == RIGHT_TOUCH_CONTROLLER_ID:
		_unload_controller_input_map(joystick_id)
	elif controller_id == LEFT_HAND_ID or controller_id == RIGHT_HAND_ID:
		_unload_hand_input_map(joystick_id)

func _load_controller_input_map(joystick_id: int):
	_update_controller_input_map(joystick_id, true)

func _unload_controller_input_map(joystick_id: int):
	_update_controller_input_map(joystick_id, false)

func _update_controller_input_map(joystick_id: int, add: bool):
	# Back button events
	var back_button_event = InputEventJoypadButton.new()
	back_button_event.button_index = ControllerButtons.VR_BUTTON_BY
	back_button_event.device = joystick_id
	if add:
		InputMap.action_add_event(back_button_action, back_button_event)
	else:
		InputMap.action_erase_event(back_button_action, back_button_event)

	# Menu button events
	var menu_click_event = InputEventJoypadButton.new()
	menu_click_event.button_index = ControllerButtons.VR_BUTTON_MENU
	menu_click_event.device = joystick_id
	if add:
		InputMap.action_add_event(menu_button_action, menu_click_event)
	else:
		InputMap.action_erase_event(menu_button_action, menu_click_event)

	# Click events
	var ax_click_event = InputEventJoypadButton.new()
	ax_click_event.button_index = ControllerButtons.VR_BUTTON_AX
	ax_click_event.device = joystick_id
	if add:
		InputMap.action_add_event(click_action, ax_click_event)
	else:
		InputMap.action_erase_event(click_action, ax_click_event)

	var trigger_click_event = InputEventJoypadButton.new()
	trigger_click_event.button_index = ControllerButtons.VR_TRIGGER
	trigger_click_event.device = joystick_id
	if add:
		InputMap.action_add_event(click_action, trigger_click_event)
	else:
		InputMap.action_erase_event(click_action, trigger_click_event)

	# Left scroll event
	var left_scroll_event = InputEventJoypadMotion.new()
	left_scroll_event.device = joystick_id
	left_scroll_event.axis = X_AXIS
	left_scroll_event.axis_value = -1.0
	if add:
		InputMap.action_add_event(left_scroll_action, left_scroll_event)
	else:
		InputMap.action_erase_event(left_scroll_action, left_scroll_event)

	# Right scroll event
	var right_scroll_event = InputEventJoypadMotion.new()
	right_scroll_event.device = joystick_id
	right_scroll_event.axis = X_AXIS
	right_scroll_event.axis_value = 1.0
	if add:
		InputMap.action_add_event(right_scroll_action, right_scroll_event)
	else:
		InputMap.action_erase_event(right_scroll_action, right_scroll_event)

	# Up scroll event
	var up_scroll_event = InputEventJoypadMotion.new()
	up_scroll_event.device = joystick_id
	up_scroll_event.axis = Y_AXIS
	up_scroll_event.axis_value = -1.0
	if add:
		InputMap.action_add_event(up_scroll_action, up_scroll_event)
	else:
		InputMap.action_erase_event(up_scroll_action, up_scroll_event)

	# Down scroll event
	var down_scroll_event = InputEventJoypadMotion.new()
	down_scroll_event.device = joystick_id
	down_scroll_event.axis = Y_AXIS
	down_scroll_event.axis_value = 1.0
	if add:
		InputMap.action_add_event(down_scroll_action, down_scroll_event)
	else:
		InputMap.action_erase_event(down_scroll_action, down_scroll_event)

func _load_hand_input_map(joystick_id: int):
	_update_hand_input_map(joystick_id, true)

func _unload_hand_input_map(joystick_id: int):
	_update_hand_input_map(joystick_id, false)

func _update_hand_input_map(joystick_id: int, add: bool):
	# Back button events
	var back_button_event = InputEventJoypadButton.new()
	back_button_event.button_index = FingerButtons.MIDDLE_PINCH
	back_button_event.device = joystick_id
	if add:
		InputMap.action_add_event(back_button_action, back_button_event)
	else:
		InputMap.action_erase_event(back_button_action, back_button_event)

	# Menu button events
	var menu_click_event = InputEventJoypadButton.new()
	menu_click_event.button_index = FingerButtons.MENU_PRESSED
	menu_click_event.device = joystick_id
	if add:
		InputMap.action_add_event(menu_button_action, menu_click_event)
	else:
		InputMap.action_erase_event(menu_button_action, menu_click_event)


	# Click event
	var index_finger_click_event = InputEventJoypadButton.new()
	index_finger_click_event.button_index = FingerButtons.INDEX_PINCH
	index_finger_click_event.device = joystick_id
	if add:
		InputMap.action_add_event(click_action, index_finger_click_event)
	else:
		InputMap.action_erase_event(click_action, index_finger_click_event)
