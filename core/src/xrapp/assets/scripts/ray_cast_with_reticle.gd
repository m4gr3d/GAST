extends RayCast

onready var ray_reticle = $RayReticle
onready var ray_pointer = $RayPointer

func _physics_process(delta):
	ray_reticle.visible = is_colliding()
	ray_pointer.visible = ray_reticle.visible
	if (ray_reticle.visible):
		ray_reticle.translation = to_local(get_collision_point())

