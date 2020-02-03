extends Spatial

var gast_loader = load("res://plugin_artifacts/addons/gastlib/GastLoader.gdns")

# Called when the node enters the scene tree for the first time.
func _ready():
	var gast = gast_loader.new()
	gast.initialize()
