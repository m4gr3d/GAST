package org.godotengine.plugin.gast.webview

import android.app.Activity
import android.text.TextUtils
import android.util.Log
import android.view.View
import android.view.ViewGroup
import org.godotengine.godot.Godot
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.extension.GastExtension
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.atomic.AtomicInteger

/**
 * * GAST-Webview plugin
 *
 * This plugin is used to display a webview in the Godot engine.
 * It uses Android built-in webview API for its webview implementation.
 */
class GastWebviewPlugin(godot: Godot) : GastExtension(godot) {

    companion object {
        private val TAG = GastWebviewPlugin::class.java.simpleName
        private const val INVALID_WEBVIEW_ID = -1
    }

    private val webPanelsCounter = AtomicInteger(0)
    private val webPanelsById = ConcurrentHashMap<Int, WebPanel>()
    private lateinit var webPanelsContainerView: ViewGroup

    override fun getPluginName() = "gast-webview"

    override fun getPluginMethods() = listOf(
        "initializeWebView",
        "loadUrl",
        "setWebViewSize",
        "setWebViewDensity",
        "shutdownWebView"
    )

    override fun onMainCreate(activity: Activity): View? {
        super.onMainCreate(activity)

        webPanelsContainerView =
            activity.layoutInflater.inflate(R.layout.web_panels_container, null) as ViewGroup
        return webPanelsContainerView
    }

    override fun onMainResume() {
        super.onMainResume()
        for (webPanel in webPanelsById.values) {
            webPanel.onResume()
        }
    }

    override fun onMainPause() {
        super.onMainPause()
        for (webPanel in webPanelsById.values) {
            webPanel.onPause()
        }
    }

    override fun onMainDestroy() {
        super.onMainDestroy()
        for (webPanel in webPanelsById.values) {
            webPanel.onDestroy()
        }

        webPanelsById.clear()
    }

    @Suppress("unused")
    private fun initializeWebView(parentNodePath: String): Int {
        if (TextUtils.isEmpty(parentNodePath)) {
            Log.e(TAG, "Invalid parent node path value: $parentNodePath")
            return INVALID_WEBVIEW_ID
        }

        val parentActivity = activity ?: return INVALID_WEBVIEW_ID

        // Create a gast node in the given parent node.
        val gastNode = GastNode(gastManager, parentNodePath)

        // Generate the web panel
        val webPanel = WebPanel(parentActivity, webPanelsContainerView, gastManager, gastNode)
        val webPanelId = webPanelsCounter.getAndIncrement()

        webPanelsById[webPanelId] = webPanel

        return webPanelId
    }

    @Suppress("unused")
    private fun loadUrl(webViewId: Int, url: String) {
        val webPanel = webPanelsById[webViewId] ?: return
        webPanel.loadUrl(url)
    }

    @Suppress("unused")
    private fun setWebViewSize(webViewId: Int, width: Float, height: Float) {
        val webPanel = webPanelsById[webViewId] ?: return
        webPanel.setSize(width, height)
    }

    @Suppress("unused")
    private fun setWebViewDensity(webViewId: Int, density: Float) {
        val webPanel = webPanelsById[webViewId] ?: return
        webPanel.setDensity(density)
    }

    @Suppress("unused")
    private fun shutdownWebView(webViewId: Int) {
        val webPanel = webPanelsById.remove(webViewId) ?: return
        webPanel.onDestroy()
    }
}
