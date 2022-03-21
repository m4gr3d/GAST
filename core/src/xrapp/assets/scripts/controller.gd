extends ARVRController

func _process(delta):
	if get_is_active():
		if !visible:
			visible = true
	elif visible:
		visible = false
