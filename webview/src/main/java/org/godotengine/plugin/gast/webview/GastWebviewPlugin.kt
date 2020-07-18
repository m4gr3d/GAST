package org.godotengine.plugin.gast.webview

import android.app.Activity
import android.view.View
import android.webkit.WebView
import org.godotengine.godot.Godot
import org.godotengine.plugin.gast.extension.GastExtension

/**
 * * GAST-Webview plugin
 *
 * This plugin is used to display a webview in the Godot engine.
 * It uses Android built-in webview API for its webview implementation.
 */
class GastWebviewPlugin(godot: Godot) : GastExtension(godot) {

    companion object {
        private val TAG = GastWebviewPlugin::class.java.simpleName
    }

    private lateinit var webView: WebView
    private lateinit var loadProgress: View

    override fun getPluginName() = "gast-webview"

    override fun getPluginMethods() = mutableListOf(
        "prepareWebview"
    )

    override fun onMainCreate(activity: Activity): View? {
        super.onMainCreate(activity)

        val view = activity.layoutInflater.inflate(R.layout.webview_panel, null)
        loadProgress = view.findViewById(R.id.load_progress)
        webView = view.findViewById(R.id.webview)
        webView.scrollBarStyle = WebView.SCROLLBARS_INSIDE_INSET;
        webView.isScrollbarFadingEnabled = true;

        return view
    }

    override fun onMainResume() {
        super.onMainResume()
        webView.onResume()
    }

    override fun onMainPause() {
        super.onMainPause()
        webView.onPause()
    }

    override fun onMainDestroy() {
        super.onMainDestroy()
        webView.destroy()
    }

    override fun onMainInputHover(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        TODO("Relay to the webview")
    }

    override fun onMainInputPress(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        TODO("Relay to the webview")
    }

    override fun onMainInputRelease(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        TODO("Relay to the webview")
    }

    override fun onMainInputScroll(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float,
        horizontalDelta: Float,
        verticalDelta: Float
    ) {
        TODO("Relay to the webview")
    }
}
