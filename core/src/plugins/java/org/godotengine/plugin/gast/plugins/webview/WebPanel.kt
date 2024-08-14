package org.godotengine.plugin.gast.plugins.webview

import android.app.Activity
import android.graphics.Bitmap
import android.view.View
import android.view.ViewGroup
import android.webkit.WebView
import android.webkit.WebViewClient
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.R
import org.godotengine.plugin.gast.xr.view.XrFrameLayout
import org.godotengine.plugin.gast.xr.view.XrViewRenderManager

internal class WebPanel(
    private val activity: Activity,
    private val containerView: ViewGroup,
    private val gastManager: GastManager,
    private val gastNode: GastNode
) {

    private lateinit var panelView: XrFrameLayout
    private lateinit var webView: WebView

    init {
        initializeViews()
    }

    private fun initializeViews() {
        activity.runOnUiThread {
            panelView = activity.layoutInflater.inflate(R.layout.web_panel, containerView, false) as XrFrameLayout
            containerView.addView(panelView)
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
        activity.runOnUiThread {
            val updatedWebViewWidth = XrViewRenderManager.fromGodotDimensionsToPixels(activity, width)
            val updatedWebViewHeight = XrViewRenderManager.fromGodotDimensionsToPixels(activity, height)

            panelView.layoutParams.width = updatedWebViewWidth.toInt()
            panelView.layoutParams.height = updatedWebViewHeight.toInt()
            panelView.requestLayout()
        }
    }
}
