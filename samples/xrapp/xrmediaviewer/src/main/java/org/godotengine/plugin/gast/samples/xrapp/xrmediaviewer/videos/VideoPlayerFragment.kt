package org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.videos

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.R
import org.godotengine.plugin.gast.xr.view.XrSurfaceView

/**
 * Sets up and provides the rendering surface for the video player to render into.
 */
class VideoPlayerFragment : VideoFragment() {

    private val surfaceView: XrSurfaceView by lazy {
        requireView().findViewById(R.id.xr_video_surface)
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        bundle: Bundle?
    ): View? {
        return inflater.inflate(R.layout.video_player_fragment, container, false)
    }

    override fun onResume() {
        super.onResume()
        getPlayer()?.setVideoSurfaceHolder(surfaceView.getHolder())
    }

    override fun onStop() {
        super.onStop()
        getPlayer()?.setVideoSurfaceHolder(null)
    }

}

