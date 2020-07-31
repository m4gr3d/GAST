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

internal class WebPanel(activity: Activity, containerView: ViewGroup, gastManager: GastManager, gastNode: GastNode) {

    val panelView: GastFrameLayout
    private val webView: WebView

    init {
        panelView = activity.layoutInflater.inflate(
            R.layout.web_panel,
            containerView,
            false
        ) as GastFrameLayout
        panelView.initialize(gastManager, gastNode)

        val loadingProgress: View = panelView.findViewById(R.id.load_progress)

        webView = panelView.findViewById(R.id.webview)
        webView.scrollBarStyle = WebView.SCROLLBARS_INSIDE_INSET
        webView.isScrollbarFadingEnabled = true
        webView.webViewClient = object: WebViewClient() {
            override fun onPageFinished(view: WebView, url: String) {
                loadingProgress.visibility = View.GONE
            }

            override fun onPageStarted(view: WebView, url: String, favicon: Bitmap) {
                loadingProgress.visibility = View.VISIBLE
            }
        }

        // For testing
        webView.loadUrl("https://www.google.com/")
    }

    fun loadUrl(url: String) {
        webView.loadUrl(url)
    }

    fun onResume() {
        webView.onResume()
    }

    fun onPause() {
        webView.onPause()
    }

    fun onDestroy() {
        webView.destroy()
        panelView.shutdown()
    }
}
