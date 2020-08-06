package org.godotengine.plugin.gast.webview

import android.app.Activity
import android.graphics.Bitmap
import android.view.View
import android.view.ViewGroup
import android.webkit.WebView
import android.webkit.WebViewClient
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.view.GastFrameLayout

internal class WebPanel(
    private val activity: Activity,
    private val containerView: ViewGroup,
    private val gastManager: GastManager,
    private val gastNode: GastNode
) {

    companion object {
        private const val DEFAULT_NODE_WIDTH = 3F // in meters
        private const val DEFAULT_NODE_HEIGHT = 1.5F // in meters
        private const val DEFAULT_PANEL_DENSITY = 1f
    }

    private lateinit var panelView: GastFrameLayout
    private lateinit var webView: WebView

    private var defaultPanelWidth = 0f
    private var defaultPanelHeight = 0f
    private var panelDensity = DEFAULT_PANEL_DENSITY

    init {
        initializeViews()
        gastNode.updateSize(DEFAULT_NODE_WIDTH, DEFAULT_NODE_HEIGHT)
    }

    private fun initializeViews() {
        activity.runOnUiThread {
            defaultPanelWidth = activity.resources.getDimension(R.dimen.web_panel_width)
            defaultPanelHeight = activity.resources.getDimension(R.dimen.web_panel_height)

            activity.layoutInflater.inflate(R.layout.web_panel, containerView)

            panelView = containerView.findViewById(R.id.web_panel)
            panelView.initialize(gastManager, gastNode)

            val loadingProgress: View = panelView.findViewById(R.id.load_progress)

            webView = panelView.findViewById(R.id.webview)
            webView.scrollBarStyle = WebView.SCROLLBARS_INSIDE_INSET
            webView.isScrollbarFadingEnabled = true
            webView.webViewClient = object : WebViewClient() {
                override fun onPageFinished(view: WebView, url: String) {
                    loadingProgress.visibility = View.GONE
                }

                override fun onPageStarted(view: WebView, url: String, favicon: Bitmap?) {
                    loadingProgress.visibility = View.VISIBLE
                }
            }
        }
    }

    fun loadUrl(url: String) {
        activity.runOnUiThread {
            webView.loadUrl(url)
        }
    }

    fun onResume() {
        activity.runOnUiThread {
            webView.onResume()
        }
    }

    fun onPause() {
        activity.runOnUiThread {
            webView.onPause()
        }
    }

    fun onDestroy() {
        activity.runOnUiThread {
            webView.destroy()
            panelView.shutdown()
        }
    }

    fun setSize(width: Float, height: Float) {
        gastNode.updateSize(width, height)
        activity.runOnUiThread {
            val updatedWebViewWidth = width * defaultPanelWidth / DEFAULT_NODE_WIDTH
            val updatedWebViewHeight = height * defaultPanelHeight / DEFAULT_NODE_HEIGHT

            panelView.layoutParams.width = (updatedWebViewWidth * panelDensity).toInt()
            panelView.layoutParams.height = (updatedWebViewHeight * panelDensity).toInt()
            panelView.requestLayout()
        }
    }

    fun setTextureSize(width: Int, height: Int) {
        activity.runOnUiThread {
            panelView.setTextureSize(width, height)
        }
    }

    fun setDensity(density: Float) {
        activity.runOnUiThread {
            if (density == 0f || density == panelDensity) {
                return@runOnUiThread
            }

            panelView.layoutParams.width =
                (panelView.layoutParams.width * density / panelDensity).toInt()
            panelView.layoutParams.height =
                (panelView.layoutParams.height * density / panelDensity).toInt()
            panelView.requestLayout()

            panelDensity = density
        }
    }
}
