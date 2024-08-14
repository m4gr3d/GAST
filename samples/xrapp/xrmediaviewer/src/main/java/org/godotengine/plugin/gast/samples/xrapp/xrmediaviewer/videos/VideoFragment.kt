package org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.videos

import com.google.android.exoplayer2.SimpleExoPlayer
import org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.XrMediaViewerApplication
import org.godotengine.plugin.gast.xrapp.XrFragment

/**
 * Base video fragment class.
 */
abstract class VideoFragment: XrFragment() {

    protected fun getPlayer(): SimpleExoPlayer? {
        val applicationContext = context?.applicationContext
        if (applicationContext is XrMediaViewerApplication) {
            return applicationContext.videoPlayer
        }

        return null
    }
}
