extends Spatial

var gast_loader = load("res://godot/plugin/v1/gast/GastLoader.gdns")
var gast = null
var gast_video_plugin = null

# Called when the node enters the scene tree for the first time.
func _ready():
	gast = gast_loader.new()
	gast.initialize()

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


func _physics_process(_delta):
	if Input.is_action_just_released("play_pause_player") && gast_video_plugin != null:
		if gast_video_plugin.isPlaying():
			print("Pausing playback")
			gast_video_plugin.pause()
		else:
			print("Resuming playback")
			gast_video_plugin.play()

