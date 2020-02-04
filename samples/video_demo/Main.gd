extends Spatial

var gast_loader = load("res://godot/plugin/v2/gast/GastLoader.gdns")

# Called when the node enters the scene tree for the first time.
func _ready():
	var gast = gast_loader.new()
	gast.initialize()
