extends Spatial

var gast_loader = load("res://godot/plugin/v1/gast/GastLoader.gdns")
var gast = null
var gast_video_plugin = null

# Called when the node enters the scene tree for the first time.
func _ready():
	gast = gast_loader.new()
	gast.initialize()
	gast.connect("release_input_event", self, "_on_gast_release_input_event")

	if Engine.has_singleton("gast-video"):
		print("Setting video player...")
		gast_video_plugin = Engine.get_singleton("gast-video")
		gast_video_plugin.preparePlayer("/root/Spatial/VideoContainer", ["flight"])
		gast_video_plugin.play()
		gast_video_plugin.setRepeatMode(2)
		gast_video_plugin.setVideoScreenSize(3, 2)
	else:
		print("Unable to load gast-video singleton.")


func _process(delta):
	gast.on_process()


func _on_gast_release_input_event(node_path: String, event_origin_id: String, x_percent: float, y_percent: float):
	if gast_video_plugin.isPlaying():
		print("Pausing playback for " + node_path)
		gast_video_plugin.pause()
	else:
		print("Resuming playback for " + node_path)
		gast_video_plugin.play()

