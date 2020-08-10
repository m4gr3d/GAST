extends Spatial

var gast_loader = load("res://godot/plugin/v1/gast/GastLoader.gdns")
var gast = null
var gast_webview_plugin = null
var webview_id = null

# Called when the node enters the scene tree for the first time.
func _ready():
	gast = gast_loader.new()
	gast.initialize()
	
	if Engine.has_singleton("gast-webview"):
		print("Setting webview...")
		gast_webview_plugin = Engine.get_singleton("gast-webview")
		gast_webview_plugin.updateTextureSize(1024, 1024)
		webview_id = gast_webview_plugin.initializeWebView("/root/Spatial/WebViewContainer")
		print("Initialized webview " + str(webview_id))
		
		gast_webview_plugin.loadUrl(webview_id, "https://news.ycombinator.com/")
		
		yield(get_tree().create_timer(2.0), "timeout")
		gast_webview_plugin.setWebViewDensity(webview_id, 2)
		gast_webview_plugin.setWebViewSize(webview_id, 4, 2)
		
		yield(get_tree().create_timer(2.0), "timeout")
		gast_webview_plugin.setWebViewDensity(webview_id, 1)
		gast_webview_plugin.setWebViewSize(webview_id, 1, 2)
		
		yield(get_tree().create_timer(2.0), "timeout")
		gast_webview_plugin.setWebViewDensity(webview_id, 0.5)
	else:
		print("Unable to load gast-webview singleton.")


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	gast.on_process()
