# This script is a loader for another script only available at runtime.
# After loading and setting the new script, it will stop executing. 
# As such NO ADDITIONAL LOGIC SHOULD BE ADDED HERE.
extends Spatial

func _enter_tree():
	# The loaded resource is only available at runtime.
	set_script(load("res://addons/godot-openxr/config/OpenXRHand.gdns"))
