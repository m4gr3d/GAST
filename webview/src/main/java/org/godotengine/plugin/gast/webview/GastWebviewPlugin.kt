package org.godotengine.plugin.gast.webview

import android.app.Activity
import android.view.View
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

    private val webPanelsList = ArrayList<WebPanel>()
    private lateinit var webPanelsContainerView: View

    override fun getPluginName() = "gast-webview"

    override fun getPluginMethods() = mutableListOf(
        "prepareWebview"
    )

    override fun onMainCreate(activity: Activity): View? {
        super.onMainCreate(activity)
        webPanelsContainerView = activity.layoutInflater.inflate(R.layout.web_panels_container, null)
        return webPanelsContainerView
    }

    override fun onMainResume() {
        super.onMainResume()
        for (webPanel in webPanelsList) {
            webPanel.onResume()
        }
    }

    override fun onMainPause() {
        super.onMainPause()
        for (webPanel in webPanelsList) {
            webPanel.onPause()
        }
    }

    override fun onMainDestroy() {
        super.onMainDestroy()
        for (webPanel in webPanelsList) {
            webPanel.onDestroy()
        }
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
