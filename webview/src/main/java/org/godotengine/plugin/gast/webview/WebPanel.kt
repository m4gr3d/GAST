package org.godotengine.plugin.gast.webview

import android.app.Activity
import android.view.View
import android.view.ViewGroup
import android.webkit.WebView

internal class WebPanel(activity: Activity, containerView: ViewGroup) {

    val panelView: View
    private val webView: WebView
    private val loadingProgress: View

    init {
        panelView = activity.layoutInflater.inflate(R.layout.web_panel, containerView, false)
        loadingProgress = panelView.findViewById(R.id.load_progress)
        webView = panelView.findViewById(R.id.webview)
        webView.scrollBarStyle = WebView.SCROLLBARS_INSIDE_INSET;
        webView.isScrollbarFadingEnabled = true;
    }

    fun onResume() {
        webView.onResume()
    }

    fun onPause() {
        webView.onPause()
    }

    fun onDestroy() {
        webView.destroy()
    }
}
